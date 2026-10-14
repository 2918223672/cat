#include "jy901s.h"
#include <stdio.h>

uint8_t receive_buff[USART_RECEIVE_LENGTH];						//接收数组
uint8_t receive_complete_flag = 0;										//接收完成标志位
uint16_t receive_data;																//数据帧16位数据缓存
uint16_t parity_bit;																	//数据帧校验位

uint8_t frame1 = 0xFF;
uint8_t frame2 = 0xAA;
/*定义结构体变量*/
SAcc jy901_Acc = {{0, 0, 0}, 0};       //加速度
SGyro jy901_Gyro = {{0, 0, 0}, 0};     //角速度
SAngle jy901_Angle = {{0, 0, 0}, 0};   //角度
SMag jy901_Mag = {{0, 0, 0}, 0};       //磁场
SParity Parity = {0,0,0,0};

//-----------------------------自定义函数--------------------------------
void JY901S_Init(void){
//   HAL_Delay(1500);
    // 启动 DMA 空闲接收
    HAL_UARTEx_ReceiveToIdle_DMA(&JY901S_UART_HANDLE, receive_buff, USART_RECEIVE_LENGTH);
#if JY901S_DISABLE_DMA_HT_IRQ
    __HAL_DMA_DISABLE_IT(JY901S_UART_HANDLE.hdmarx, DMA_IT_HT);
#endif
}

void JY901S_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if (huart != &JY901S_UART_HANDLE) {
        return;
    }

    if (size >= USART_RECEIVE_LENGTH) {
        receive_complete_flag = 1;
    }

    HAL_UARTEx_ReceiveToIdle_DMA(&JY901S_UART_HANDLE, receive_buff, USART_RECEIVE_LENGTH);
#if JY901S_DISABLE_DMA_HT_IRQ
    __HAL_DMA_DISABLE_IT(JY901S_UART_HANDLE.hdmarx, DMA_IT_HT);
#endif
}

/*JY901S数据处理函数*/
void vJY901S_Data_Proc(void){
	if(receive_complete_flag == 1){
		receive_complete_flag = 0;
		/*校验*/
		Parity.Acc_parity_bit = (receive_buff[0] + receive_buff[1] + receive_buff[2] + receive_buff[3] + receive_buff[4] + receive_buff[5]+ receive_buff[6] + receive_buff[7] + receive_buff[8] + receive_buff[9]) & 0x00ff;
		Parity.Gyro_parity_bit = (receive_buff[11] + receive_buff[12] + receive_buff[13] + receive_buff[14] + receive_buff[15] + receive_buff[16]+ receive_buff[17] + receive_buff[18] + receive_buff[19] + receive_buff[20]) & 0x00ff;
		Parity.Angle_parity_bit = (receive_buff[22] + receive_buff[23] + receive_buff[24] + receive_buff[25] + receive_buff[26] + receive_buff[27]+ receive_buff[28] + receive_buff[29] + receive_buff[30] + receive_buff[31]) & 0x00ff;
		Parity.Mag_parity_bit = (receive_buff[33] + receive_buff[34] + receive_buff[35] + receive_buff[36] + receive_buff[37] + receive_buff[38]+ receive_buff[39] + receive_buff[40] + receive_buff[41] + receive_buff[42]) & 0x00ff;
		/*如果校验正确就读取数据*/
		if(Parity.Acc_parity_bit == receive_buff[10]){
			jy901_Acc.a[0] = (short)(((short)receive_buff[3] << 8) | receive_buff[2]) / 32768.0 * 16.0 * 9.8;/*加速度x*/
			jy901_Acc.a[1] = (short)(((short)receive_buff[5] << 8) | receive_buff[4]) / 32768.0 * 16.0 * 9.8;/*加速度y*/
			jy901_Acc.a[2] = (short)(((short)receive_buff[7] << 8) | receive_buff[6]) / 32768.0 * 16.0 * 9.8;/*加速度z*/
			jy901_Acc.T = ((receive_buff[9] << 8) | receive_buff[8]) / 100.0;								   /*温度*/
		}
		if(Parity.Gyro_parity_bit == receive_buff[21]){
			jy901_Gyro.w[0] = (short)(((short)receive_buff[14] << 8) | receive_buff[13]) / 32768.0 * 2000.0;/*角速度x*/
			jy901_Gyro.w[1] = (short)(((short)receive_buff[16] << 8) | receive_buff[15]) / 32768.0 * 2000.0;/*角速度y*/
			jy901_Gyro.w[2] = (short)(((short)receive_buff[18] << 8) | receive_buff[17]) / 32768.0 * 2000.0;/*角速度z*/
		}
		if(Parity.Angle_parity_bit == receive_buff[32]){
			jy901_Angle.Angle[0] = (short)(((short)receive_buff[25] << 8) | receive_buff[24]) / 32768.0 * 180.0;/*角度Roll*/
			jy901_Angle.Angle[1] = (short)(((short)receive_buff[27] << 8) | receive_buff[26]) / 32768.0 * 180.0;/*角度Pitch*/
			jy901_Angle.Angle[2] = (short)(((short)receive_buff[29] << 8) | receive_buff[28]) / 32768.0 * 180.0;/*角度Yaw*/
			jy901_Angle.Version = (short)(((short)receive_buff[31] << 8) | receive_buff[30]);                   /*版本号version*/
		}
		if(Parity.Mag_parity_bit == receive_buff[43]){
			jy901_Mag.h[0] = (short)(((short)receive_buff[36] << 8) | receive_buff[35]) * 0.00833;/*磁场x*/
			jy901_Mag.h[1] = (short)(((short)receive_buff[38] << 8) | receive_buff[37]) * 0.00833;/*磁场y*/
			jy901_Mag.h[2] = (short)(((short)receive_buff[40] << 8) | receive_buff[39]) * 0.00833;/*磁场z*/
		}
		//memset(receive_buff,0,USART_RECEIVE_LENGTH);
	}
}

/*写指令函数*/
//void vJY901S_Send_Command(uint8_t ADDR,uint8_t DATAL,uint8_t DATAH){
//	HAL_UART_Transmit(&huart2,(uint8_t*)&frame1,1,100); 					//帧头1
//	HAL_UART_Transmit(&huart2,(uint8_t*)&frame2,1,100);						//帧头2
//	HAL_UART_Transmit(&huart2,(uint8_t*)&ADDR,1,100);							//寄存器地址
//	HAL_UART_Transmit(&huart2,(uint8_t*)&DATAL,1,100);						//数据第低八位
//	HAL_UART_Transmit(&huart2,(uint8_t*)&DATAH,1,100);						//数据高八位
//}

void JY901S_Task(void)
{
	vJY901S_Data_Proc();
	printf("%d,%d,%d\r\n", jy901_Angle.Angle[0],jy901_Angle.Angle[1],jy901_Angle.Angle[2]);

}

#if JY901S_ENABLE_TRACK_HELPER
int32_t JY901S_GetTrackError(Track_State_t *sensor, uint16_t *normal_data, Track_Ctrl_t *ctrl, float gyro_angle)
{
    int32_t track_error = track_steering(normal_data, 1);

    // 转向死区：小偏差不纠偏，消除直线蛇形
    if (track_error > -200 && track_error < 200) {
        track_error = 0;
        ctrl->turn_speed = 0.0f;
    } else {
        float norm_error = (float)track_error / 1024.0f;
        float norm_last  = (float)ctrl->last_error / 1024.0f;

        ctrl->turn_speed = (ctrl->track_Kp * norm_error)
                         + (ctrl->track_Kd * (norm_error - norm_last));
    }

    ctrl->last_error = track_error;
    return track_error;
}
#endif
