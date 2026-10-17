#include "Schedular_app.h"

// 任务结构体
typedef struct {
  void (*task_func)(void);  // 任务函数指针
  uint32_t rate_ms;         // 执行周期（毫秒）
  uint32_t last_run;        // 上次执行时间（初始化为 0，每次运行时刷新）
} scheduler_task_t;

// 全局变量，用于存储任务数量
uint8_t task_num;

/**
 * @brief 用户初始化函数
 * 非HAL库硬件初始化函数
 */
void System_Init()
{
  // 初始化编码器
	Encoder_Init();

  // 初始化电机pwm通道
  motor_init();

  // 初始化PID系统
  PID_System_Init();

  // 初始化LCD显示屏
  LCD_Init();
  LCD_Fill(0, 0, LCD_W, LCD_H, RED);
  HAL_Delay(500);
  LCD_Fill(0, 0, LCD_W, LCD_H, BLACK);
  seyes_proc();

  // Ping + 初始化循迹传感器（3s 超时保护，防止传感器离线卡死）
  {
      uint32_t t0 = HAL_GetTick();
      while (Ping()) {
          if (HAL_GetTick() - t0 > 3000) break;
          HAL_Delay(100);
      }
      Task_init(&my_track_sensor, env_white, env_black);
  }


}

// 静态任务数组，每个任务包含任务函数、执行周期（毫秒）和上次运行时间（毫秒）
static scheduler_task_t scheduler_task[] =
{
  {key_proc,       10, 0},   // 按键扫描
  {running_proc,   10, 0},   // 循迹 + PID（时序敏感）
  {seyes_proc,     50, 0},   // LCD 显示（50ms 实时刷新）
};


/**
 * @brief 调度器初始化函数
 * 计算任务数组的元素个数，并将结果存储在 task_num 中
 */
void Scheduler_Init(void)
{
  System_Init();
  // 计算任务数组的元素个数，并将结果存储在 task_num 中
  task_num = sizeof(scheduler_task) / sizeof(scheduler_task_t); // 数组大小 / 数组成员大小 = 数组元素个数
}

/**
 * @brief 调度器运行函数
 * 遍历任务数组，检查是否有任务需要执行。如果当前时间已经超过任务的执行周期，则执行该任务并更新上次运行时间
 */
void Scheduler_Run(void)
{
//    static uint32_t startup_tick = 0;
//    static uint8_t  startup_done = 0;

//  // 上电后延时 2 秒再执行任务
//  if (!startup_done) {
//      if (startup_tick == 0) {
//          startup_tick = HAL_GetTick();
//      }
//      if (HAL_GetTick() - startup_tick < 1000) {
//          return;
//      }
//      startup_done = 1;
//  }
    
  // 遍历任务数组中的所有任务
  for (uint8_t i = 0; i < task_num; i++)
  {
    uint32_t now_time = HAL_GetTick();
    int32_t elapsed = (int32_t)(now_time - scheduler_task[i].last_run);

    // 检查是否到达（或超过）执行周期
    if (elapsed >= (int32_t)scheduler_task[i].rate_ms)
    {
      // ★ 锚定到理想时刻：last_run += rate_ms，而非 last_run = now_time
      //    避免任务执行耗时导致的累积漂移
      scheduler_task[i].last_run += scheduler_task[i].rate_ms;

      // 防落后保护：如果已经落后超过 2 个周期，重置到当前时间，避免追赶风暴
      if ((int32_t)(now_time - scheduler_task[i].last_run) > (int32_t)(scheduler_task[i].rate_ms * 2)) {
          scheduler_task[i].last_run = now_time;
      }

      scheduler_task[i].task_func();
    }
  }
}

