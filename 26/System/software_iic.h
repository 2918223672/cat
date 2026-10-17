#include "main.h"
#include "gw_grayscale_sensor.h"

/******************************************************************************
 * 循迹传感器：感为智能 带MCU 灰度模块（I2C 数字通信）
 *
 * 引脚定义：
 *   PA2  — SCL          软件 I2C 时钟线（CubeMX: GPIO_Output, Open-Drain, Pull-up）
 *   PA8  — SDA          软件 I2C 数据线（CubeMX: GPIO_Output, Open-Drain, Pull-up）
 *   I2C 地址：0x4C
 ******************************************************************************/

#define SDA_PIN  track_SDA_Pin
#define SDA_PORT track_SDA_GPIO_Port
#define SCL_PIN  track_SCL_Pin
#define SCL_PORT track_SCL_GPIO_Port

/* 基本I2C操作宏 */
#define SDA_HIGH() HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_SET)
#define SDA_LOW()  HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_RESET)
#define SCL_HIGH() HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_SET)
#define SCL_LOW()  HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_RESET)
#define READ_SDA() HAL_GPIO_ReadPin(SDA_PORT, SDA_PIN)

unsigned char Ping(void);
unsigned char IIC_Get_Digtal(void);
unsigned char IIC_Get_Anolog(unsigned char * Result,unsigned char len);
unsigned char IIC_Get_Single_Anolog(unsigned char Channel);
unsigned char IIC_Anolog_Normalize(uint8_t Normalize_channel);
unsigned short IIC_Get_Offset(void);
