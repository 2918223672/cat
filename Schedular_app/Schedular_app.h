#ifndef __SCHEDULER_APP_H__
#define __SCHEDULER_APP_H__

#include "Myconfig.h"

/* 调度周期与初始化超时统一放在头文件中配置。 */
#define SCHED_KEY_PERIOD_MS             10U
#define SCHED_RUNNING_PERIOD_MS         10U
#define SCHED_DISPLAY_PERIOD_MS         50U
#define SCHED_LCD_SPLASH_MS             500U
#define SCHED_SENSOR_PING_TIMEOUT_MS    3000U
#define SCHED_SENSOR_PING_RETRY_MS      100U
#define SCHED_MAX_LAG_PERIODS           2U

/* 应用任务名称映射，项目函数改名时只修改这里。 */
#define SCHED_KEY_TASK                  key_proc
#define SCHED_RUNNING_TASK              running_proc
#define SCHED_DISPLAY_TASK              seyes_proc

void System_Init(void);
void Scheduler_Init(void);
void Scheduler_Run(void);
  
#endif
