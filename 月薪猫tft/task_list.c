#include "task_list.h"
#include "gif_data_new.h"

uint16_t track_normal_data[8];

// 电机控制任务
void running_proc(void)
{
    // ================== 第 1 步：获取传感器数据 ==================
    // 刷新循迹传感器数据
    track_task(&my_track_sensor);

    // 获取归一化数据 -OK
    if (Get_track_Normal_value(&my_track_sensor, track_normal_data) == 0) return;

    // ================== 第 2 步：外环 - 结构体 PD 控制 ==================
    // 计算轨迹偏差 
    int32_t track_error = track_steering(track_normal_data, 1);

    // 使用结构体参数计算转向差速 (turn_speed)
    track_ctrl.turn_speed = (track_ctrl.track_Kp * track_error) + 
                            track_ctrl.track_Kd * (track_error - track_ctrl.last_error);

    // 更新历史偏差，供下一次 10ms 中断使用
    track_ctrl.last_error = track_error;

    // ================== 第 3 步：内外环结合 ==================
    // 分配差速
    PID_SetTarget(&left_pid, track_ctrl.base_speed - track_ctrl.turn_speed);
    PID_SetTarget(&right_pid, track_ctrl.base_speed + track_ctrl.turn_speed);

    // ================== 第 4 步：内环 - 速度 PID ==================
    // 读取编码器速度
    Encoder_GetSpeed();

    // PID控制 target -- left_speed_cm_s
    float left_out = PID_Compute_Increment(&left_pid, left_speed_cm_s);
    float right_out = PID_Compute_Increment(&right_pid, right_speed_cm_s);

    // 控制电机
    Left_Dirt((int16_t)left_out);
    Right_Dirt((int16_t)right_out);
        
}

void seyes_proc(void)
{
    static uint8_t frame_num = 0;

    LCD_CS_Set();
    
    // y=(160-64)/2=48 居中显示, w=128占满宽, h=64
    LCD_ShowMonoBitmap(0, 48, 128, 64, gif_frames_array[frame_num], WHITE, BLACK);

    frame_num++;
    if(frame_num >= GIF_FRAME_COUNT)
        frame_num = 0;
}

