#ifndef __MYCONFIG_H__
#define __MYCONFIG_H__

/* ========== HAL 库头文件 ========== */
#include "main.h"
#include "gpio.h"
#include "tim.h"
//#include "adc.h"
#include "usart.h"

/* ========== C 语言头文件 ========== */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

/* ========== 组件库头文件 ========== */
#include "pid.h"
#include "motor.h"
#include "encoder.h"
#include "track_algo.h"
#include "i2c.h"
#include "hardware_iic.h"
#include "lcd_init.h"
#include "zk_app.h"
/* ===========应用层头文件 ========== */
#include "task_list.h"
#include "adc_app.h"
#include "key_app.h"
#include "lcd_app.h"



/* ===========调度器头文件 ========== */
#include "Schedular_app.h"

#endif


