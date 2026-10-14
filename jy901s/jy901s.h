#ifndef _JY901S_H
#define _JY901S_H


#include "main.h"
#include "usart.h"
#include <stdint.h>

/* 硬件配置：移植到其他串口时只修改本头文件。 */
#define JY901S_UART_HANDLE          huart5
#define JY901S_RX_LENGTH            44U
#define USART_RECEIVE_LENGTH        JY901S_RX_LENGTH
#define JY901S_ENABLE_TRACK_HELPER  0U
#define JY901S_DISABLE_DMA_HT_IRQ   1U

#if JY901S_ENABLE_TRACK_HELPER
#include "track_algo.h"
#endif

extern uint8_t receive_buff[JY901S_RX_LENGTH];	//接收数组
extern uint8_t receive_complete_flag;			

#define SAVE 			0x00
#define CALSW 		0x01
#define RSW 			0x02
#define RRATE			0x03
#define BAUD 			0x04
#define AXOFFSET	0x05
#define AYOFFSET	0x06
#define AZOFFSET	0x07
#define GXOFFSET	0x08
#define GYOFFSET	0x09
#define GZOFFSET	0x0a
#define HXOFFSET	0x0b
#define HYOFFSET	0x0c
#define HZOFFSET	0x0d
#define D0MODE		0x0e
#define D1MODE		0x0f
#define D2MODE		0x10
#define D3MODE		0x11
#define D0PWMH		0x12
#define D1PWMH		0x13
#define D2PWMH		0x14
#define D3PWMH		0x15
#define D0PWMT		0x16
#define D1PWMT		0x17
#define D2PWMT		0x18
#define D3PWMT		0x19
#define IICADDR		0x1a
#define LEDOFF 		0x1b
#define GPSBAUD		0x1c

#define YYMM				0x30
#define DDHH				0x31
#define MMSS				0x32
#define MS					0x33
#define AX					0x34
#define AY					0x35
#define AZ					0x36
#define GX					0x37
#define GY					0x38
#define GZ					0x39
#define HX					0x3a
#define HY					0x3b
#define HZ					0x3c			
#define Roll				0x3d
#define Pitch				0x3e
#define Yaw					0x3f
#define TEMP				0x40
#define D0Status		0x41
#define D1Status		0x42
#define D2Status		0x43
#define D3Status		0x44
#define PressureL		0x45
#define PressureH		0x46
#define HeightL			0x47
#define HeightH			0x48
#define LonL				0x49
#define LonH				0x4a
#define LatL				0x4b
#define LatH				0x4c
#define GPSHeight   0x4d
#define GPSYAW      0x4e
#define GPSVL				0x4f
#define GPSVH				0x50
#define q0          0x51
#define q1          0x52
#define q2          0x53
#define q3          0x54

#define KEY         0x69
#define GYROCALITHR 0x61
      
#define DIO_MODE_AIN 0
#define DIO_MODE_DIN 1
#define DIO_MODE_DOH 2
#define DIO_MODE_DOL 3
#define DIO_MODE_DOPWM 4
#define DIO_MODE_GPS 5		

typedef struct 
{
	unsigned char ucYear;
	unsigned char ucMonth;
	unsigned char ucDay;
	unsigned char ucHour;
	unsigned char ucMinute;
	unsigned char ucSecond;
	unsigned short usMiliSecond;
}STime;

typedef struct 
{
	short a[3];    // 三轴加速度原始数据
	float T;       // 温度值
	
	/*加速度计温度用于补偿
    if(jy901_Acc.T > 25.0) {}
    高温补偿算法*/
	
}SAcc;

typedef struct 
{
	short w[3];     // 三轴角速度原始数据
	short Volt;     // 电压值
}SGyro;

typedef struct 
{
	short Angle[3];    // 三轴角度值
	short Version;     // 版本号
}SAngle;

typedef struct 
{
	short h[3];    // 三轴磁场强度
	short T;       // 温度值
	
	/* 磁力计温度用于地磁补偿
    if(jy901_Mag.T < 0) {}
    低温地磁补偿*/

}SMag;

typedef struct 
{
	short sDStatus[4];
}SDStatus;   //状态信息

typedef struct 
{
	long lPressure;    // 气压值
	long lAltitude;    // 海拔高度
}SPress;

typedef struct 
{
	long lLon;    // 经度
	long lLat;    // 纬度
}SLonLat;

typedef struct 
{
	short sGPSHeight;      // GPS高度
	short sGPSYaw;         // GPS航向角
	long lGPSVelocity;     // GPS速度
}SGPSV;

typedef struct 
{ 
	short q[4];    // 四元数
}SQ;



/*校验位结构体*/
typedef struct{
	char Acc_parity_bit;     // 加速度数据校验位
	char Gyro_parity_bit;    // 角速度数据校验位
	char Angle_parity_bit;   // 角度数据校验位
	char Mag_parity_bit;     // 磁场数据校验位
}SParity;

void JY901S_Init(void);
void JY901S_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size);

void vJY901S_Data_Proc(void);
//void vJY901S_Send_Command(uint8_t ADDR,uint8_t DATAL,uint8_t DATAH);
void JY901S_Task(void);
#if JY901S_ENABLE_TRACK_HELPER
int32_t JY901S_GetTrackError(Track_State_t *sensor, uint16_t *normal_data, Track_Ctrl_t *ctrl, float gyro_angle);
#endif
extern SAcc jy901_Acc;
extern SGyro jy901_Gyro;
extern SAngle jy901_Angle;
extern SMag jy901_Mag;
extern SParity Parity;
extern uint8_t frame1;
extern uint8_t frame2;

#endif

