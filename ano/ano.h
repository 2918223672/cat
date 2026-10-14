#ifndef __ANO_H
#define __ANO_H

#include "main.h"
#include "usart.h"
#include "pid.h"
#include <stdint.h>

/* 硬件与协议配置：移植时只修改本头文件中的宏。 */
#define ANO_UART_HANDLE          huart3
#define ANO_UART_INSTANCE        USART3
#define ANO_UART_TIMEOUT_MS      100U
#define ANO_BUFFER_SIZE          32U
#define ANO_FRAME_HEAD           0xAAU
#define ANO_FRAME_ADDRESS        0xFFU
#define ANO_ID_DATA              0xF1U
#define ANO_ID_READ_PARAMETER    0xE1U
#define ANO_ID_WRITE_PARAMETER   0xE2U
#define ANO_ID_CHECK             0x00U

extern float data[4];

/* 数据拆分宏定义，在发送大于1字节的数据类型时，比如int16、float等，需要把数据拆分成单独字节进行发送 */

#define BYTE0(dwTemp)       ( *( (uint8_t *)(&(dwTemp)) + 0) )
#define BYTE1(dwTemp)       ( *( (uint8_t *)(&(dwTemp)) + 1) )
#define BYTE2(dwTemp)       ( *( (uint8_t *)(&(dwTemp)) + 2) )
#define BYTE3(dwTemp)       ( *( (uint8_t *)(&(dwTemp)) + 3) )
	
void Data_send(int32_t _a,int32_t _b,int32_t _c,int32_t _d);

void Ano_SentPar(uint16_t id,int32_t data);
void Ano_Anl(uint8_t *Data_Get);
void Ano_GetByte(uint8_t data);
void Ano_SentCheck(uint8_t id,uint8_t _sc,uint8_t _ac);

#endif

