#include "task_list.h"
#include "gif_data_new.h"
#include <stdlib.h>
#include <stdio.h>

uint16_t track_normal_data[8];
int32_t now_error;
uint32_t race_cnt = 0;
uint32_t g_sensor_sum = 0;

// 电机控制任务
void running_proc(void)
{
    // ================== 状态机：空闲时断电停车 ==================
    if (run_mode == 0) {
        HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_RESET);
        Left_Dirt(0);
        Right_Dirt(0);
        left_pid.output = 0.0f;
        right_pid.output = 0.0f;
        left_pid.error[0] = left_pid.error[1] = left_pid.error[2] = 0.0f;
        right_pid.error[0] = right_pid.error[1] = right_pid.error[2] = 0.0f;
        return;
    }

    // ================== 第 1 步：获取传感器数据 ==================
    track_task_i2c(&my_track_sensor);

    if (!my_track_sensor.ok) return;

    // ★ ch0/ch4 偏暗，白平衡修正到与其它通道一致
    uint16_t corrected[8];
    float scale[8] = {1.35f, 1.0f, 1.0f, 1.0f, 1.35f, 1.0f, 1.0f, 1.0f};
    for (int i = 0; i < 8; i++) {
        float v = (float)my_track_sensor.Analog_value[i] * scale[i];
        corrected[i] = (v > 255.0f) ? 255 : (uint16_t)v;
    }
    g_sensor_sum = 0;
    for (int i = 0; i < 8; i++) g_sensor_sum += corrected[i];

    // 用修正后的值做归一化
    binaryzation(corrected, my_track_sensor.Gray_white, my_track_sensor.Gray_black, &my_track_sensor.Digtal);
    unification(corrected, my_track_sensor.Normal_factor, my_track_sensor.Calibrated_black,
                track_normal_data, my_track_sensor.bits);

    // 计算循迹偏差（白底黑线：field=1）
    now_error = track_steering(track_normal_data, 1);
    float track_error = (float)now_error / 1024.0f;

    // ★ 死区：直道微小偏差不转向，避免来回摆动；弯道偏差超阈值立刻 PD 响应
    #define STRAIGHT_DEADZONE  0.60f   // 降低死区，让车提前感知弯道，减少突变冲击
    if (fabsf(track_error) < STRAIGHT_DEADZONE) {
        track_ctrl.turn_speed = 0.0f;
    } else {
        // 外环 PD 转向量（只在弯道生效）
        float steering = track_ctrl.track_Kp * track_error +
                         track_ctrl.track_Kd * (track_error - (float)track_ctrl.last_error / 1024.0f);
        float raw_turn = -steering * (track_ctrl.base_speed / 10.0f);
        // 一阶低通平滑转向量，抑制弯道入口的冲击抖动（系数0.6：越小越平滑）
        track_ctrl.turn_speed = track_ctrl.turn_speed * 0.4f + raw_turn * 0.6f;
      }
        track_ctrl.last_error = now_error;

    // 限幅：turn_speed 不超过 base_speed，防止单轮倒转
    if (track_ctrl.turn_speed > track_ctrl.base_speed)  track_ctrl.turn_speed = track_ctrl.base_speed;
    if (track_ctrl.turn_speed < -track_ctrl.base_speed) track_ctrl.turn_speed = -track_ctrl.base_speed;

    // ================== 第 2 步：各任务的差异逻辑 ==================
    switch(run_mode)
    {
        case 0: HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_RESET);  break;
        case 1: {                                                                  // topic1
            static uint8_t stop_cnt = 0;
            track_ctrl.base_speed = 40.0f;

            // 统计当前帧黑色传感器数量（Digtal 中为 0 的位 = 黑色传感器）
            uint8_t black_count = 0;
            for (int i = 0; i < 8; i++) {
                if (!((my_track_sensor.Digtal >> i) & 1)) black_count++;
            }
            printf("b=%d r=%lu\r\n", black_count, race_cnt);

            // 停止线检测：black>=4 连续2帧触发
            // 实测：弯道噪声最高3(孤立单帧)，停止线最高7(连续7帧)，阈值4可完全区分
            if (race_cnt > 200 && black_count >= 4) {
                if (++stop_cnt >= 2) {
                    HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_RESET);
                    Left_Dirt(0); Right_Dirt(0);
                    run_mode = 0; stop_cnt = 0;
                    return;
                }
            } else {
                stop_cnt = 0;
            }

            // 超时保险：20s仍未停则强制停车（题目要求≤20s）
            if (race_cnt > 1700) {
                HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_RESET);
                Left_Dirt(0); Right_Dirt(0);
                run_mode = 0; stop_cnt = 0;
                return;
            }

            race_cnt++;
        }
        break;
        case 2:
            HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_RESET);
            Left_Dirt(0); Right_Dirt(0);
            return;
        case 3: {                                                                  // topic3: A→B计时，通过B后继续行驶
            static int8_t ab_frozen = 0;
            if (race_cnt == 0) ab_frozen = 0;   // 每次启动时重置

            // 0→40cm/s 梯度加速，500ms 内线性爬升
            track_ctrl.base_speed = (race_cnt < 50) ? (40.0f * (float)race_cnt / 50.0f) : 40.0f;

            // 第一次进入弯道（超出直线死区）时冻结计时，不停车
            if (!ab_frozen && race_cnt > 100 && fabsf(track_error) > 0.70f) {
                ab_frozen = 1;
            }

            if (!ab_frozen) {
                race_cnt++;   // 仅AB段计时，到达B后时间冻结
            }
        } break;
        case 4: {                                                                  // topic4
            static uint8_t black_cnt = 0;
            track_ctrl.base_speed = (race_cnt < 50) ? (30.0f * race_cnt / 50.0f) : 30.0f; // 30cm/s，≤30s
            // A点5cm黑线：8路全黑，连续5帧确认防误触
            if (my_track_sensor.Digtal == 0x00 && race_cnt > 200) {  // 至少跑2s，避免起点误判
                if (++black_cnt >= 5) {
                    HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_RESET);
                    Left_Dirt(0); Right_Dirt(0);
                    run_mode = 0; black_cnt = 0;
                    return;
                }
            } else black_cnt = 0;
            race_cnt++;
        } break;
        case 5: {                                                                  // topic5
            static uint8_t black_cnt = 0;
            track_ctrl.base_speed = (race_cnt < 50) ? (30.0f * race_cnt / 50.0f) : 30.0f;
            if (my_track_sensor.Digtal == 0x00 && race_cnt > 200) {
                if (++black_cnt >= 5) {
                    HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_RESET);
                    Left_Dirt(0); Right_Dirt(0);
                    run_mode = 0; black_cnt = 0;
                    return;
                }
            } else black_cnt = 0;
            race_cnt++;
        } break;
    }
    HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_SET); // 使能电机驱动
    // ================== 第 3 步：差速分配，内外环结合 ==================
    // 222 方向：Left = base - turn, Right = base + turn
    float left_target  = track_ctrl.base_speed - track_ctrl.turn_speed;
    float right_target = track_ctrl.base_speed + track_ctrl.turn_speed;

    // 目标符号翻转时，重置 PID 状态避免过零震荡
    if ((left_pid.target > 0 && left_target < 0) || (left_pid.target < 0 && left_target > 0)) {
      left_pid.output = 0.0f;
      left_pid.error[0] = left_pid.error[1] = left_pid.error[2] = 0.0f;
    }
    if ((right_pid.target > 0 && right_target < 0) || (right_pid.target < 0 && right_target > 0)) {
      right_pid.output = 0.0f;
      right_pid.error[0] = right_pid.error[1] = right_pid.error[2] = 0.0f;
    }
    // 分配差速
    PID_SetTarget(&left_pid,  left_target);
    PID_SetTarget(&right_pid, right_target);

    // ================== 第 4 步：内环 - 速度 PID ==================
    // 读取编码器速度
    Encoder_GetSpeed();

    // ★ 目标速度为 0 时直接停止电机，不跑 PID
    //    避免编码器噪声驱动 PID 随机游走导致的"自转 + 正反转交替"
     if (left_pid.target == 0.0f) {
      Left_Dirt(0);
      left_pid.output = 0.0f;
      left_pid.error[0] = left_pid.error[1] = left_pid.error[2] = 0.0f;
    }
    if (right_pid.target == 0.0f) {
      Right_Dirt(0);
      right_pid.output = 0.0f;
      right_pid.error[0] = right_pid.error[1] = right_pid.error[2] = 0.0f;
    }
    if (left_pid.target == 0.0f && right_pid.target == 0.0f) return;

    // PID控制 target -- left_speed_cm_s
    float left_out = PID_Compute_Increment(&left_pid, left_speed_cm_s);
    float right_out = PID_Compute_Increment(&right_pid, right_speed_cm_s);

     // ================== 第 5 步：驱动电机 ==================
    // 控制电机
    if(HAL_GPIO_ReadPin(STBY_GPIO_Port, STBY_Pin) == GPIO_PIN_RESET) {
        Left_Dirt(0);
        Right_Dirt(0);
        return;
    }
    else if(HAL_GPIO_ReadPin(STBY_GPIO_Port, STBY_Pin) == GPIO_PIN_SET) {
        Left_Dirt((int16_t)left_out);
        Right_Dirt((int16_t)right_out);
    }
}

/* 手写时间字符串（避免 MicroLIB sprintf 不支持 %lu 导致乱码）*/
/* race_cnt 每 10ms 递增一次，上限 99999999（≈277 小时），超出自动回绕 */
static void make_time_str(char *buf, uint32_t race_cnt)
{
    /* 限制最大显示值，防止 buffer 溢出和 '0'+n 非数字字符 */
    if (race_cnt > 99999999u) race_cnt = 99999999u;

    uint32_t sec = race_cnt / 100;            // 整秒
    uint32_t d10 = (race_cnt / 10) % 10;      // 十分位

    uint8_t pos = 0;

    /* 分解秒数到 8 位缓冲区：最多显示 6 位秒 + '.' + 1 位十分位 + 's' + '\0' */
    if (sec >= 100000) { buf[pos++] = '0' + (sec / 100000); }
    if (sec >= 10000)  { buf[pos++] = '0' + ((sec / 10000) % 10); }
    if (sec >= 1000)   { buf[pos++] = '0' + ((sec / 1000) % 10); }
    if (sec >= 100)    { buf[pos++] = '0' + ((sec / 100) % 10); }
    if (sec >= 10)     { buf[pos++] = '0' + ((sec / 10) % 10); }
    buf[pos++] = '0' + (sec % 10);

    buf[pos++] = '.';
    buf[pos++] = '0' + d10;
    buf[pos++] = 's';
    buf[pos] = '\0';
}

void seyes_proc(void)
{
    char buf[12];  // 最大 "999999.9s" = 9字符 + '\0'，留余量
    static uint8_t last_sys_mode = 0xFF;

    if (sys_mode != last_sys_mode) {
        if (sys_mode == 0)
            LCD_ShowIndexedBitmap(0, 0, 160, 128, gImage_22);
        else
            LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
        last_sys_mode = sys_mode;
    }

    if (sys_mode >= 1 && sys_mode <= 5) {
        const u8 *title;
        switch (sys_mode) {
            case 1: title = (const u8 *)"Topic1"; break;
            case 2: title = (const u8 *)"Topic2"; break;
            case 3: title = (const u8 *)"Topic3"; break;
            case 4: title = (const u8 *)"Topic4"; break;
            default:title = (const u8 *)"Topic5"; break;
        }
        LCD_ShowString((LCD_W - 6 * 16) / 2, 0, title, CYAN, BLACK, 32, 1);

        buf[0] = '0' + sys_mode;
        buf[1] = '.'; buf[2] = 't'; buf[3] = 'i'; buf[4] = 'm'; buf[5] = 'e'; buf[6] = ':'; buf[7] = '\0';
        LCD_ShowString(0, 32, (const u8 *)buf, CYAN, BLACK, 24, 1);

        if (run_mode == sys_mode)
            LCD_ShowString(90, 32, (const u8 *)"run", GREEN, BLACK, 24, 1);

        LCD_DrawRectangle(10, 60, 150, 118, WHITE);

        /* ★ 先填充黑色背景再写时间，防止旧字符残留导致乱码 */
        LCD_Fill(11, 74, 149, 100, BLACK);
        make_time_str(buf, race_cnt);
        /* 固定中心位置写时间：最大 9 字符 (999999.9s)，24px 字体宽 12px/char */
        LCD_ShowString((LCD_W - (uint16_t)strlen(buf) * 12) / 2, 77, (const u8 *)buf, CYAN, BLACK, 24, 0);
    }
}
