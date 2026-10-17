#ifndef track_algo_h
#define track_algo_h

#include "Myconfig.h"

typedef struct {
    uint16_t Analog_value[8];    // 原始模拟量值
    uint16_t Normal_value[8];   // 归一化后的值
    uint16_t Calibrated_white[8]; // 白校准基准值
    uint16_t Calibrated_black[8]; // 黑校准基准值

    uint16_t Gray_white[8];      // 白平衡阈值
    uint16_t Gray_black[8];      // 黑平衡阈值
    double Normal_factor[8];          // 归一化系数

    double bits;                      // ADC分辨率对应位数
    uint8_t Digtal;              // 数字输出状态
    uint16_t Tick;                // 时基计数器
    uint16_t ok;                  // 传感器就绪标志
} Track_State_t;


typedef struct
{
   float track_Kp;  // 决定转弯的力度
   float track_Kd;  // 决定抑制摇摆的力度 (通常是 Kp 的 3~10 倍)
   float gyro_Kp;     // 陀螺仪 P（偏航恢复力度）
   float gyro_Kd;    // 陀螺仪 D
   
   float base_speed;  // 必须新增基础速度参数！
   float turn_speed;  // 转向差速
    
   int32_t last_error; //上一次的位置偏差，用于微分计算 (程序自动更新，无需设定)
} Track_Ctrl_t;

extern uint16_t env_white[8];
extern uint16_t env_black[8];
extern Track_State_t my_track_sensor;
extern Track_Ctrl_t track_ctrl;

void binaryzation(uint16_t *adc_value,uint16_t *Gray_white,uint16_t *Gray_black,uint8_t *Digital);
void unification(uint16_t *adc_value,double *Normal_factor,uint16_t *Calibrated_black,uint16_t *result,double bits);
void Task_init_Frist(Track_State_t*sensor);
void Task_init(Track_State_t*sensor,uint16_t *white_data, uint16_t *black_data);
void track_task(Track_State_t*sensor);
void track_task_i2c(Track_State_t *sensor);
uint16_t Get_track_Digtal(Track_State_t*sensor);
uint16_t Get_track_Normal_value(Track_State_t*sensor,uint16_t* result);
uint16_t Get_track_Ok(Track_State_t*sensor,uint16_t *result);

int32_t track_steering(uint16_t Normal[8],uint8_t field);
void track_line_lost(Track_State_t*sensor , uint8_t field);

#endif

