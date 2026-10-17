#include "X_V2.h"

/**********************************************************
***	X_V2�����ջ���������
***	��д���ߣ�ZHANGDATOU
***	����֧�֣��Ŵ�ͷ�ջ��ŷ�
***	�Ա����̣�https://zhangdatou.taobao.com
***	CSDN���ͣ�http s://blog.csdn.net/zhangdatou666
***	qq����Ⱥ��262438510
**********************************************************/

__IO uint16_t MMCL_count = 0, MMCL_cmd[MMCL_LEN] = {0};

/**********************************************************
*** ������������
**********************************************************/
/**
  * @brief    ����������У׼
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Trig_Encoder_Cal(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x06;                       // ������
  cmd[2] =  0x45;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ��������
	HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 4);
}

/**
  * @brief    ���������X42S/Y42��
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Reset_Motor(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x08;                       // ������
  cmd[2] =  0x97;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ��������
	HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 4);
}

/**
  * @brief    ����ǰλ������
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Reset_CurPos_To_Zero(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x0A;                       // ������
  cmd[2] =  0x6D;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ��������
	HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 4);
}

/**
  * @brief    �����ת����
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Reset_Clog_Pro(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x0E;                       // ������
  cmd[2] =  0x52;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 4);
}

/**
  * @brief    �ָ���������
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Restore_Motor(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x0F;                       // ������
  cmd[2] =  0x5F;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ��������
	HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 4);
}

/**********************************************************
*** �˶���������
**********************************************************/
/**
  * @brief    �������X42S/Y42��
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Multi_Motor_Cmd(uint8_t addr)
{
  uint16_t i = 0, j = 0, len = 0; __IO static uint8_t cmd[MMCL_LEN] = {0};
  
	// ��������ȴ���0
	if(MMCL_count > 0)
	{
		// ������������ֽ���
		len = MMCL_count + 5;
		
		// װ������
		cmd[0] = addr;                       // ��ַ
		cmd[1] = 0xAA;                       // ������
		cmd[2] = (uint8_t)(len >> 8);				 // ���ֽ�����8λ
		cmd[3] = (uint8_t)(len); 		 				 // ���ֽ�����8λ
		for(i=0,j=4; i < MMCL_count; i++,j++) { cmd[j] = MMCL_cmd[i]; }
		cmd[j] = 0x6B; ++j;                  // У���ֽ�
		
		// ��������
		HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, j); MMCL_count = 0;
	}
	else
	{
		MMCL_count = 0;
	}
}

/**
  * @brief    ʹ���źſ���
  * @param    addr  �������ַ
  * @param    state ��ʹ��״̬     ��trueΪʹ�ܵ����falseΪ�رյ��
  * @param    snF   �����ͬ����־ ��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_En_Control(uint8_t addr, bool state, bool snF)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xF3;                       // ������
  cmd[2] =  0xAB;                       // ������
  cmd[3] =  (uint8_t)state;             // ʹ��״̬
  cmd[4] =  snF;                        // ���ͬ���˶���־
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 6);
}

/**
  * @brief    ����ģʽ
  * @param    addr  	�������ַ
  * @param    sign  	�����ţ�����		��0Ϊ����1Ϊ��
  * @param    t_ramp	������б��(Ma/s)	����Χ0 - 65535Ma/s
  * @param    torque	�����ص���(Ma)		����Χ0 - 6000Ma
  * @param    snF   	�����ͬ����־		��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Torque_Control(uint8_t addr, uint8_t sign, uint16_t t_ramp, uint16_t torque, bool snF)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xF5;                       // ������
  cmd[2] =  sign;                       // ���ţ�����
  cmd[3] =  (uint8_t)(t_ramp >> 8);     // ����б��(Ma/s)
  cmd[4] =  (uint8_t)(t_ramp >> 0);
  cmd[5] =  (uint8_t)(torque >> 8);     // ���ص���(Ma)
  cmd[6] =  (uint8_t)(torque >> 0);
  cmd[7] =  snF;                        // ���ͬ����־
  cmd[8] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 9);
}

/**
  * @brief    ����ģʽ���ٿ��ƣ�X42S/Y42��
  * @param    addr  	�������ַ
  * @param    sign  	�����ţ�����		��0Ϊ����1Ϊ��
  * @param    t_ramp	������б��(Ma/s)	����Χ0 - 65535Ma/s
  * @param    torque	�����ص���(Ma)		����Χ0 - 6000Ma
  * @param    snF   	�����ͬ����־		��falseΪ�����ã�trueΪ����
	* @param    maxVel	������ٶ�(RPM)	����Χ0.0 - 3000.0RPM
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Torque_LV_Control(uint8_t addr, uint8_t sign, uint16_t t_ramp, uint16_t torque, bool snF, float maxVel)
{
  __IO static uint8_t cmd[16] = {0}; uint16_t v = 0;

  // ���ٶȷŴ�10�����͹�ȥ
  v = (uint16_t)ABS(maxVel * 10.0f);
  
  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xC5;                      // ������
  cmd[2]  =  sign;                      // ���ţ�����
  cmd[3]  =  (uint8_t)(t_ramp >> 8);    // ����б��(Ma/s)
  cmd[4]  =  (uint8_t)(t_ramp >> 0);
  cmd[5]  =  (uint8_t)(torque >> 8);    // ���ص���(Ma)
  cmd[6]  =  (uint8_t)(torque >> 0);
  cmd[7]  =  snF;                       // ���ͬ����־
  cmd[8]  =  (uint8_t)(v >> 8);    			// ����ٶ�(RPM)
  cmd[9]  =  (uint8_t)(v >> 0);    
  cmd[10] =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 11);
}

/**
  * @brief    �ٶ�ģʽ
  * @param    addr  �������ַ
  * @param    dir   ������						��0ΪCW��1ΪCCW
  * @param    acc   �����ٶ�(RPM/s)	����Χ0 - 65535RPM/s
  * @param    vel		���ٶ�(RPM)			����Χ0.0 - 3000.0RPM
  * @param    snF   �����ͬ����־		��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Vel_Control(uint8_t addr, uint8_t dir, uint16_t acc, float vel, bool snF)
{
  __IO static uint8_t cmd[16] = {0}; uint16_t v = 0;

  // ���ٶȷŴ�10�����͹�ȥ
  v = (uint16_t)ABS(vel * 10.0f);

  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xF6;                       // ������
  cmd[2] =  dir;                        // ���ţ�����
  cmd[3] =  (uint8_t)(acc >> 8);     		// ���ٶ�(RPM/s)
  cmd[4] =  (uint8_t)(acc >> 0);
  cmd[5] =  (uint8_t)(v >> 8);        	// �ٶ�(RPM)
  cmd[6] =  (uint8_t)(v >> 0);
  cmd[7] =  snF;                        // ���ͬ���˶���־
  cmd[8] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 9);
}

/**
  * @brief    �ٶ�ģʽ�޵������ƣ�X42S/Y42��
  * @param    addr  �������ַ
  * @param    dir   ������						��0ΪCW��1ΪCCW
  * @param    acc   �����ٶ�(RPM/s)	����Χ0 - 65535RPM/s
  * @param    vel		���ٶ�(RPM)			����Χ0.0 - 3000.0RPM
  * @param    snF   �����ͬ����־		��falseΪ�����ã�trueΪ����
	* @param    maxCur��������(mA)		����Χ0 - 6000mA
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Vel_LC_Control(uint8_t addr, uint8_t dir, uint16_t acc, float vel, bool snF, uint16_t maxCur)
{
  __IO static uint8_t cmd[16] = {0}; uint16_t v = 0;

  // ���ٶȷŴ�10�����͹�ȥ
  v = (uint16_t)ABS(vel * 10.0f);

  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xC6;                      // ������
  cmd[2]  =  dir;                       // ���ţ�����
  cmd[3]  =  (uint8_t)(acc >> 8);     	// ���ٶ�(RPM/s)
  cmd[4]  =  (uint8_t)(acc >> 0);
  cmd[5]  =  (uint8_t)(v >> 8);        	// �ٶ�(RPM)
  cmd[6]  =  (uint8_t)(v >> 0);
  cmd[7]  =  snF;                       // ���ͬ���˶���־
	cmd[8]  =  (uint8_t)(maxCur >> 8);    // ������(mA)
  cmd[9]  =  (uint8_t)(maxCur >> 0);
  cmd[10] =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 11);
}

/**
  * @brief    ֱͨ����λ��ģʽ
  * @param    addr	�������ַ
  * @param    dir   ������								��0ΪCW��1ΪCCW
  * @param    vel		���˶��ٶ�(RPM)			����Χ0.0 - 3000.0RPM
  * @param    pos		��λ�ýǶ�(��)				����Χ0.0��- (2^32 - 1) / 10��
  * @param    raf   ����λ/�����˶���־	��0Ϊ�����һ������Ŀ��λ�ã�1Ϊ����λ�ã�2Ϊ��Ե�ǰʵʱλ��
  * @param    snF   �����ͬ����־				��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Bypass_Pos_LV_Control(uint8_t addr, uint8_t dir, float vel, float pos, uint8_t raf, bool snF)
{
  uint8_t cmd[16] = {0}; uint16_t v = 0; uint32_t p = 0;

  // ���ٶȺ�λ�÷Ŵ�10�����͹�ȥ
  v = (uint16_t)ABS(vel * 10.0f); p = (uint32_t)ABS(pos * 10.0f);

  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xFB;                      // ������
  cmd[2]  =  dir;                       // ���ţ�����
  cmd[3]  =  (uint8_t)(v >> 8);       	// �˶��ٶ�(RPM)
  cmd[4]  =  (uint8_t)(v >> 0);
  cmd[5]  =  (uint8_t)(p >> 24);      	// λ�ýǶ�
  cmd[6]  =  (uint8_t)(p >> 16);
  cmd[7]  =  (uint8_t)(p >> 8);
  cmd[8]  =  (uint8_t)(p >> 0);
  cmd[9]  =  raf;                       // ��λ/�����˶���־
  cmd[10] =  snF;                       // ���ͬ���˶���־
  cmd[11] =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 12);
}

/**
  * @brief    ֱͨ����λ��ģʽ�޵�������
  * @param    addr	�������ַ
  * @param    dir   ������								��0ΪCW��1ΪCCW
  * @param    vel		���˶��ٶ�(RPM)			����Χ0.0 - 3000.0RPM
  * @param    pos		��λ�ýǶ�(��)				����Χ0.0��- (2^32 - 1) / 10��
  * @param    raf   ����λ/�����˶���־	��0Ϊ�����һ������Ŀ��λ�ã�1Ϊ����λ�ã�2Ϊ��Ե�ǰʵʱλ��
  * @param    snF   �����ͬ����־				��falseΪ�����ã�trueΪ����
	* @param    maxCur��������(mA)		����Χ0 - 6000mA
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Bypass_Pos_LV_LC_Control(uint8_t addr, uint8_t dir, float vel, float pos, uint8_t raf, bool snF, uint16_t maxCur)
{
  uint8_t cmd[16] = {0}; uint16_t v = 0; uint32_t p = 0;

  // ���ٶȺ�λ�÷Ŵ�10�����͹�ȥ
  v = (uint16_t)ABS(vel * 10.0f); p = (uint32_t)ABS(pos * 10.0f);

  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xCB;                      // ������
  cmd[2]  =  dir;                       // ���ţ�����
  cmd[3]  =  (uint8_t)(v >> 8);       	// �˶��ٶ�(RPM)
  cmd[4]  =  (uint8_t)(v >> 0);
  cmd[5]  =  (uint8_t)(p >> 24);      	// λ�ýǶ�
  cmd[6]  =  (uint8_t)(p >> 16);
  cmd[7]  =  (uint8_t)(p >> 8);
  cmd[8]  =  (uint8_t)(p >> 0);
  cmd[9]  =  raf;                       // ��λ/�����˶���־
  cmd[10] =  snF;                       // ���ͬ���˶���־
	cmd[11] =  (uint8_t)(maxCur >> 8);    // ������(mA)
  cmd[12] =  (uint8_t)(maxCur >> 0);
  cmd[13] =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 14);
}

/**
  * @brief    �������߼Ӽ���λ��ģʽ����
  * @param    addr  �������ַ
  * @param    dir   ������								��0ΪCW������ֵΪCCW
  * @param    acc   �����ټ��ٶ�(RPM/s)	��0ΪCW������ֵΪCCW
  * @param    dec   �����ټ��ٶ�(RPM/s)	��0ΪCW������ֵΪCCW
  * @param    vel		������ٶ�(RPM)			����Χ0.0 - 3000.0RPM
  * @param    pos		��λ��(��)						����Χ0.0��- (2^32 - 1)��
  * @param    raf   ����λ/�����˶���־	��0Ϊ�����һ������Ŀ��λ�ã�1Ϊ����λ�ã�2Ϊ��Ե�ǰʵʱλ��
  * @param    snF   �����ͬ����־					��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Traj_Pos_Control(uint8_t addr, uint8_t dir, uint16_t acc, uint16_t dec, float vel, float pos, uint8_t raf, bool snF)
{
  uint8_t cmd[32] = {0}; uint16_t v = 0; uint32_t p = 0;

  // ���ٶȺ�λ�÷Ŵ�10�����͹�ȥ
  v = (uint16_t)ABS(vel * 10.0f); p = (uint32_t)ABS(pos * 10.0f);

  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xFD;                      // ������
  cmd[2]  =  dir;                       // ���ţ�����
  cmd[3]  =  (uint8_t)(acc >> 8);       // ���ټ��ٶ�(RPM/s)
  cmd[4]  =  (uint8_t)(acc >> 0);
  cmd[5]  =  (uint8_t)(dec >> 8);       // ���ټ��ٶ�(RPM/s)
  cmd[6]  =  (uint8_t)(dec >> 0);
  cmd[7]  =  (uint8_t)(v >> 8);       	// ����ٶ�(RPM)
  cmd[8]  =  (uint8_t)(v >> 0);
  cmd[9]  =  (uint8_t)(p >> 24);      	// λ�ýǶ�
  cmd[10] =  (uint8_t)(p >> 16);
  cmd[11] =  (uint8_t)(p >> 8);
  cmd[12] =  (uint8_t)(p >> 0);
  cmd[13] =  raf;                       // ��λ/�����˶���־
  cmd[14] =  snF;                       // ���ͬ���˶���־
  cmd[15] =  0x6B;                      // У���ֽ�

  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 16);
}

/**
  * @brief    �������߼Ӽ���λ��ģʽ�޵������ƣ�X42S/Y42��
  * @param    addr  �������ַ
  * @param    dir   ������								��0ΪCW������ֵΪCCW
  * @param    acc   �����ټ��ٶ�(RPM/s)	��0ΪCW������ֵΪCCW
  * @param    dec   �����ټ��ٶ�(RPM/s)	��0ΪCW������ֵΪCCW
  * @param    vel		������ٶ�(RPM)			����Χ0.0 - 3000.0RPM
  * @param    pos		��λ��(��)						����Χ0.0��- (2^32 - 1)��
  * @param    raf   ����λ/�����˶���־	��0Ϊ�����һ������Ŀ��λ�ã�1Ϊ����λ�ã�2Ϊ��Ե�ǰʵʱλ��
  * @param    snF   �����ͬ����־				��falseΪ�����ã�trueΪ����
	* @param    maxCur��������(mA)				����Χ0 - 6000mA
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Traj_Pos_LC_Control(uint8_t addr, uint8_t dir, uint16_t acc, uint16_t dec, float vel, float pos, uint8_t raf, bool snF, uint16_t maxCur)
{
  uint8_t cmd[32] = {0}; uint16_t v = 0; uint32_t p = 0;

  // ���ٶȺ�λ�÷Ŵ�10�����͹�ȥ
  v = (uint16_t)ABS(vel * 10.0f); p = (uint32_t)ABS(pos * 10.0f);

  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xCD;                      // ������
  cmd[2]  =  dir;                       // ���ţ�����
  cmd[3]  =  (uint8_t)(acc >> 8);       // ���ټ��ٶ�(RPM/s)
  cmd[4]  =  (uint8_t)(acc >> 0);
  cmd[5]  =  (uint8_t)(dec >> 8);       // ���ټ��ٶ�(RPM/s)
  cmd[6]  =  (uint8_t)(dec >> 0);
  cmd[7]  =  (uint8_t)(v >> 8);       	// ����ٶ�(RPM)
  cmd[8]  =  (uint8_t)(v >> 0);
  cmd[9]  =  (uint8_t)(p >> 24);      	// λ�ýǶ�
  cmd[10] =  (uint8_t)(p >> 16);
  cmd[11] =  (uint8_t)(p >> 8);
  cmd[12] =  (uint8_t)(p >> 0);
  cmd[13] =  raf;                       // ��λ/�����˶���־
  cmd[14] =  snF;                       // ���ͬ���˶���־
	cmd[15] =  (uint8_t)(maxCur >> 8);    // ������(mA)
  cmd[16] =  (uint8_t)(maxCur >> 0);
  cmd[17] =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 18);
}

/**
  * @brief    ���ÿ�����������λ��ģʽ���˶�������X42S/Y42��
  * @param    addr  �������ַ
  * @param    acc   �����ټ��ٶ�(RPM/s)	��0ΪCW������ֵΪCCW
  * @param    dec   �����ټ��ٶ�(RPM/s)	��0ΪCW������ֵΪCCW
  * @param    vel		������ٶ�(RPM)			����Χ0.0 - 3000.0RPM
  * @param    raf   ����λ/�����˶���־	��0Ϊ�����һ������Ŀ��λ�ã�1Ϊ����λ�ã�2Ϊ��Ե����ǰʵʱλ��
  * @param    snF   �����ͬ����־				��falseΪ�����ã�trueΪ����
	* @param    maxCur��������(mA)				����Χ0 - 6000mA
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Set_QTrajPos_Params(uint8_t addr, uint16_t acc, uint16_t dec, float vel, uint8_t raf, bool snF, uint16_t maxCur)
{
  uint8_t cmd[16] = {0}; uint16_t v = 0;

  // ���ٶȷŴ�10�����͹�ȥ
  v = (uint16_t)ABS(vel * 10.0f);

  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xF1;                      // ������
  cmd[2]  =  (uint8_t)(acc >> 8);       // ���ټ��ٶ�(RPM/s)
  cmd[3]  =  (uint8_t)(acc >> 0);
  cmd[4]  =  (uint8_t)(dec >> 8);       // ���ټ��ٶ�(RPM/s)
  cmd[5]  =  (uint8_t)(dec >> 0);
  cmd[6]  =  (uint8_t)(v >> 8);       	// ����ٶ�(RPM)
  cmd[7]  =  (uint8_t)(v >> 0);
  cmd[8]  =  raf;                       // ��λ/�����˶���־
  cmd[9]  =  snF;                       // ���ͬ���˶���־
	cmd[10] =  (uint8_t)(maxCur >> 8);    // ������(mA)
  cmd[11] =  (uint8_t)(maxCur >> 0);
  cmd[12] =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 13);
}

/**
  * @brief    ������������λ��ģʽ���ƣ�X42S/Y42��
  * @param    addr  �������ַ
  * @param    pos		��λ�ýǶ�(������)����λ��0.1��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_QTrajPos_LC_Control(uint8_t addr, float pos)
{
  uint8_t cmd[16] = {0}; int32_t p = 0;

  // ��λ�÷Ŵ�10�����͹�ȥ������һλС����
  p = (int32_t)ABS(pos * 10.0f);

  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xFC;                      // ������
  cmd[2]  =  (uint8_t)(p >> 24);      	// λ�ýǶ�
  cmd[3] =  (uint8_t)(p >> 16);
  cmd[4] =  (uint8_t)(p >> 8);
  cmd[5] =  (uint8_t)(p >> 0);
  cmd[6] =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 7);
}

/**
  * @brief    ����ֹͣ
  * @param    addr  �������ַ
  * @param    snF   �����ͬ����־��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Stop_Now(uint8_t addr, bool snF)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xFE;                       // ������
  cmd[2] =  0x98;                       // ������
  cmd[3] =  snF;                        // ���ͬ���˶���־
  cmd[4] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 5);
}

/**
  * @brief    ���ͬ���˶�
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Synchronous_motion(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xFF;                       // ������
  cmd[2] =  0x66;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 4);
}

/**********************************************************
*** ԭ���������
**********************************************************/
/**
  * @brief    ���õ�Ȧ��������λ��
  * @param    addr  �������ַ
  * @param    svF   ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Origin_Set_O(uint8_t addr, bool svF)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x93;                       // ������
  cmd[2] =  0x88;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 5);
}

/**
  * @brief    ��������
  * @param    addr   �������ַ
  * @param    o_mode ������ģʽ��0Ϊ��Ȧ�ͽ����㣬1Ϊ��Ȧ������㣬2Ϊ��Ȧ����λ��ײ���㣬3Ϊ��Ȧ����λ���ػ���
  * @param    snF   �����ͬ����־��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Origin_Trigger_Return(uint8_t addr, uint8_t o_mode, bool snF)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x9A;                       // ������
  cmd[2] =  o_mode;                     // ����ģʽ��0Ϊ��Ȧ�ͽ����㣬1Ϊ��Ȧ������㣬2Ϊ��Ȧ����λ��ײ���㣬3Ϊ��Ȧ����λ���ػ���
  cmd[3] =  snF;                        // ���ͬ���˶���־��falseΪ�����ã�trueΪ����
  cmd[4] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 5);
}

/**
  * @brief    ǿ���жϲ��˳�����
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Origin_Interrupt(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x9C;                       // ������
  cmd[2] =  0x48;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 4);
}

/**
  * @brief    ��ȡ�������
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Origin_Read_Params(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x22;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 3);
}

/**
  * @brief    �޸Ļ������
  * @param    addr  �������ַ
  * @param    svF   ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    o_mode ������ģʽ��0Ϊ��Ȧ�ͽ����㣬1Ϊ��Ȧ������㣬2Ϊ��Ȧ����λ��ײ���㣬3Ϊ��Ȧ����λ���ػ���
  * @param    o_dir  �����㷽��0ΪCW������ֵΪCCW
  * @param    o_vel  �������ٶȣ���λ��RPM��ת/���ӣ�
  * @param    o_tm   �����㳬ʱʱ�䣬��λ������
  * @param    sl_vel ������λ��ײ������ת�٣���λ��RPM��ת/���ӣ�
  * @param    sl_ma  ������λ��ײ�������������λ��Ma��������
  * @param    sl_ms  ������λ��ײ������ʱ�䣬��λ��Ms�����룩
  * @param    potF   ���ϵ��Զ��������㣬falseΪ��ʹ�ܣ�trueΪʹ��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Origin_Modify_Params(uint8_t addr, bool svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, bool potF)
{
  __IO static uint8_t cmd[32] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x4C;                       // ������
  cmd[2] =  0xAE;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  o_mode;                     // ����ģʽ��0Ϊ��Ȧ�ͽ����㣬1Ϊ��Ȧ������㣬2Ϊ��Ȧ����λ��ײ���㣬3Ϊ��Ȧ����λ���ػ���
  cmd[5] =  o_dir;                      // ���㷽��
  cmd[6]  =  (uint8_t)(o_vel >> 8);     // �����ٶ�(RPM)��8λ�ֽ�
  cmd[7]  =  (uint8_t)(o_vel >> 0);     // �����ٶ�(RPM)��8λ�ֽ� 
  cmd[8]  =  (uint8_t)(o_tm >> 24);     // ���㳬ʱʱ��(bit24 - bit31)
  cmd[9]  =  (uint8_t)(o_tm >> 16);     // ���㳬ʱʱ��(bit16 - bit23)
  cmd[10] =  (uint8_t)(o_tm >> 8);      // ���㳬ʱʱ��(bit8  - bit15)
  cmd[11] =  (uint8_t)(o_tm >> 0);      // ���㳬ʱʱ��(bit0  - bit7 )
  cmd[12] =  (uint8_t)(sl_vel >> 8);    // ����λ��ײ������ת��(RPM)��8λ�ֽ�
  cmd[13] =  (uint8_t)(sl_vel >> 0);    // ����λ��ײ������ת��(RPM)��8λ�ֽ� 
  cmd[14] =  (uint8_t)(sl_ma >> 8);     // ����λ��ײ���������(Ma)��8λ�ֽ�
  cmd[15] =  (uint8_t)(sl_ma >> 0);     // ����λ��ײ���������(Ma)��8λ�ֽ� 
  cmd[16] =  (uint8_t)(sl_ms >> 8);     // ����λ��ײ������ʱ��(Ms)��8λ�ֽ�
  cmd[17] =  (uint8_t)(sl_ms >> 0);     // ����λ��ײ������ʱ��(Ms)��8λ�ֽ�
  cmd[18] =  potF;                      // �ϵ��Զ��������㣬falseΪ��ʹ�ܣ�trueΪʹ��
  cmd[19] =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 20);
}

/**
  * @brief    ��ȡ��ײ���㷵�ؽǶȣ�X42S/Y42��
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Origin_Read_SL_RP(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x3F;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 3);
}

/**
  * @brief    �޸���ײ���㷵�ؽǶȣ�X42S/Y42��
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    sl_rp 	 ����ײ���㷵�ؽǶȣ���λ0.1�㣬����40������4.0��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Origin_Modify_SL_RP(uint8_t addr, bool svF, uint16_t sl_rp)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0x5C;                      // ������
  cmd[2]  =  0xAC;                      // ������
  cmd[3]  =  svF;                       // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4]  =  (uint8_t)(sl_rp >> 8);			// ��ײ���㷵�ؽǶȣ���λ0.1��
	cmd[5]  =  (uint8_t)(sl_rp >> 0);
  cmd[6]  =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 7);
}

/**********************************************************
*** ��ȡϵͳ��������
**********************************************************/
/**
  * @brief    ��ʱ������Ϣ���X42S/Y42��
  * @param    addr  	�������ַ
  * @param    s     	��ϵͳ��������
	* @param    time_ms ����ʱʱ��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Auto_Return_Sys_Params_Timed(uint8_t addr, SysParams_t s, uint16_t time_ms)
{
  uint8_t i = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[i] = addr; ++i;                   // ��ַ

  cmd[i] = 0x11; ++i;                   // ������

  cmd[i] = 0x18; ++i;                   // ������

  switch(s)                             // ��Ϣ������
  {
    case S_VBUS : cmd[i] = 0x24; ++i; break;	// ��ȡ���ߵ�ѹ
		case S_CBUS : cmd[i] = 0x26; ++i; break;	// ��ȡ���ߵ���
    case S_CPHA : cmd[i] = 0x27; ++i; break;	// ��ȡ�����
		case S_ENCO : cmd[i] = 0x29; ++i; break;	// ��ȡ������ԭʼֵ
		case S_CLKC : cmd[i] = 0x30; ++i; break;	// ��ȡʵʱ������
    case S_ENCL : cmd[i] = 0x31; ++i; break;	// ��ȡ�������Ի�У׼��ı�����ֵ
		case S_CLKI : cmd[i] = 0x32; ++i; break;	// ��ȡ����������
    case S_TPOS : cmd[i] = 0x33; ++i; break;	// ��ȡ���Ŀ��λ��
    case S_SPOS : cmd[i] = 0x34; ++i; break;	// ��ȡ���ʵʱ�趨��Ŀ��λ��
		case S_VEL  : cmd[i] = 0x35; ++i; break;	// ��ȡ���ʵʱת��
    case S_CPOS : cmd[i] = 0x36; ++i; break;	// ��ȡ���ʵʱλ��
    case S_PERR : cmd[i] = 0x37; ++i; break;	// ��ȡ���λ�����
		case S_VBAT : cmd[i] = 0x38; ++i; break;	// ��ȡ��Ȧ��������ص�ѹ��Y42��
		case S_TEMP : cmd[i] = 0x39; ++i; break;	// ��ȡ���ʵʱ�¶ȣ�X42S/Y42��
    case S_FLAG : cmd[i] = 0x3A; ++i; break;	// ��ȡ���״̬��־λ
    case S_OFLAG: cmd[i] = 0x3B; ++i; break;	// ��ȡ����״̬��־λ
		case S_OAF  : cmd[i] = 0x3C; ++i; break;	// ��ȡ���״̬��־λ + ����״̬��־λ��X42S/Y42��
		case S_PIN  : cmd[i] = 0x3D; ++i; break;	// ��ȡ����IO״̬��X42S/Y42��
    default: break;
  }
	
	cmd[i] = (uint8_t)(time_ms >> 8);  ++i;	// ��ʱʱ��
	cmd[i] = (uint8_t)(time_ms >> 0);  ++i;

  cmd[i] = 0x6B; ++i;                   	// У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, i);
}

/**
  * @brief    ��ȡϵͳ����
  * @param    addr  �������ַ
  * @param    s     ��ϵͳ��������
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Read_Sys_Params(uint8_t addr, SysParams_t s)
{
  uint8_t i = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[i] = addr; ++i;                   // ��ַ

  switch(s)                             // ������
  {
    case S_VBUS : cmd[i] = 0x24; ++i; break;	// ��ȡ���ߵ�ѹ
		case S_CBUS : cmd[i] = 0x26; ++i; break;	// ��ȡ���ߵ���
    case S_CPHA : cmd[i] = 0x27; ++i; break;	// ��ȡ�����
		case S_ENCO : cmd[i] = 0x29; ++i; break;	// ��ȡ������ԭʼֵ
		case S_CLKC : cmd[i] = 0x30; ++i; break;	// ��ȡʵʱ������
    case S_ENCL : cmd[i] = 0x31; ++i; break;	// ��ȡ�������Ի�У׼��ı�����ֵ
		case S_CLKI : cmd[i] = 0x32; ++i; break;	// ��ȡ����������
    case S_TPOS : cmd[i] = 0x33; ++i; break;	// ��ȡ���Ŀ��λ��
    case S_SPOS : cmd[i] = 0x34; ++i; break;	// ��ȡ���ʵʱ�趨��Ŀ��λ��
		case S_VEL  : cmd[i] = 0x35; ++i; break;	// ��ȡ���ʵʱת��
    case S_CPOS : cmd[i] = 0x36; ++i; break;	// ��ȡ���ʵʱλ��
    case S_PERR : cmd[i] = 0x37; ++i; break;	// ��ȡ���λ�����
		case S_VBAT : cmd[i] = 0x38; ++i; break;	// ��ȡ��Ȧ��������ص�ѹ��Y42��
		case S_TEMP : cmd[i] = 0x39; ++i; break;	// ��ȡ���ʵʱ�¶ȣ�X42S/Y42��
    case S_FLAG : cmd[i] = 0x3A; ++i; break;	// ��ȡ���״̬��־λ
    case S_OFLAG: cmd[i] = 0x3B; ++i; break;	// ��ȡ����״̬��־λ
		case S_OAF  : cmd[i] = 0x3C; ++i; break;	// ��ȡ���״̬��־λ + ����״̬��־λ��X42S/Y42��
		case S_PIN  : cmd[i] = 0x3D; ++i; break;	// ��ȡ����״̬��X42S/Y42��
		case S_SYS  : cmd[i] = 0x43; ++i; cmd[i] = 0x7A; ++i; break;	// ��ȡϵͳ״̬����
    default: break;
  }

  cmd[i] = 0x6B; ++i;                   // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, i);
}

/**********************************************************
*** ��д������������
**********************************************************/
/**
  * @brief    �޸ĵ��ID��ַ
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    id			 ��Ĭ�ϵ��IDΪ1�����޸�Ϊ1-255��0Ϊ�㲥��ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_Motor_ID(uint8_t addr, bool svF, uint8_t id)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xAE;                       // ������
  cmd[2] =  0x4B;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  id;                  				// Ĭ�ϵ��IDΪ1�����޸�Ϊ1-255��0Ϊ�㲥��ַ
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 6);
}

/**
  * @brief    �޸�ϸ��ֵ
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    mstep		 ��Ĭ��ϸ��Ϊ16�����޸�Ϊ1-2556��0Ϊ256ϸ��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_MicroStep(uint8_t addr, bool svF, uint8_t mstep)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x84;                       // ������
  cmd[2] =  0x8A;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  mstep;                 	 		// Ĭ��ϸ��Ϊ16�����޸�Ϊ1-2556��0Ϊ256ϸ��
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 6);
}

/**
  * @brief    �޸ĵ����־
  * @param    addr     �������ַ
  * @param    pdf		 	 �������־
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_PDFlag(uint8_t addr, bool pdf)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x50;                       // ������
  cmd[2] =  pdf;                 	 			// �����־
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 4);
}

/**
  * @brief    ��ȡѡ�����״̬��X42S/Y42��
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Read_Opt_Param_Sta(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x1A;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 3);
}

/**
  * @brief    �޸ĵ�����ͣ�X42S/Y42��
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    mottype	 ��������ͣ�Ĭ��Ϊ0��0��ʾ1.8�㲽�������1��ʾ0.9�㲽�����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_Motor_Type(uint8_t addr, bool svF, bool mottype)
{
  __IO static uint8_t cmd[16] = {0}; uint8_t MotType = 0;
  
	if(mottype) { MotType = 25; } else { MotType = 50; }
	
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xD7;                       // ������
  cmd[2] =  0x35;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  MotType;                 	 	// ������ͣ�0��ʾ0.9�㲽�������1��ʾ1.8�㲽�����
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 6);
}

/**
  * @brief    �޸Ĺ̼����ͣ�X42S/Y42��
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    fwtype	 ���̼����ͣ�Ĭ��Ϊ0��0ΪX�̼���1ΪEmm�̼�
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_Firmware_Type(uint8_t addr, bool svF, bool fwtype)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xD5;                       // ������
  cmd[2] =  0x69;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  fwtype;                 	 	// ������ͣ�25��ʾ0.9�㲽�������50��ʾ1.8�㲽�����
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 6);
}

/**
  * @brief    �޸Ŀ���/�ջ�����ģʽ��X42S/Y42��
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    ctrl_mode������ģʽ��Ĭ��Ϊ1,0Ϊ����ģʽ��1Ϊ�ջ�FOCģʽ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_Ctrl_Mode(uint8_t addr, bool svF, bool ctrl_mode)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x46;                       // ������
  cmd[2] =  0x69;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  ctrl_mode;                  // ����ģʽ��Ĭ��Ϊ1,0Ϊ����ģʽ��1Ϊ�ջ�FOCģʽ
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 6);
}

/**
  * @brief    �޸ĵ���˶�������X42S/Y42��
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    dir			 ������˶�������Ĭ��ΪCW��0ΪCW��˳ʱ�뷽�򣩣�1ΪCCW
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_Motor_Dir(uint8_t addr, bool svF, bool dir)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xD4;                       // ������
  cmd[2] =  0x60;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  dir;                  			// ����˶�������Ĭ��ΪCW��0ΪCW��˳ʱ�뷽�򣩣�1ΪCCW
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 6);
}

/**
  * @brief    �޸������������ܣ�X42S/Y42��
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    lock		 �������������ܣ�Ĭ��ΪDisable��0ΪDisable��1ΪEnable
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_Lock_Btn(uint8_t addr, bool svF, bool lock)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xD0;                       // ������
  cmd[2] =  0xB3;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  lock;                  			// �����������ܣ�Ĭ��ΪDisable��0ΪDisable����1ΪEnable
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 6);
}

/**
  * @brief    �޸�����λ�ýǶ��Ƿ������С10�����루X42S/Y42��
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    s_vel		 ������λ�ýǶ��Ƿ������С10�����룬Ĭ��ΪDisable��0ΪDisable��1ΪEnable
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_S_Vel(uint8_t addr, bool svF, bool s_vel)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x4F;                       // ������
  cmd[2] =  0x71;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  s_vel;                  		// ����λ�ýǶ��Ƿ������С10�����룬Ĭ��ΪDisable��0ΪDisable��1ΪEnable
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 6);
}

/**
  * @brief    �޸Ŀ���ģʽ��������
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    om_ma 	 ������ģʽ������������λmA
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_OM_mA(uint8_t addr, bool svF, uint16_t om_ma)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x44;                       // ������
  cmd[2] =  0x33;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  (uint8_t)(om_ma >> 8);			// ����ģʽ������������λmA
	cmd[5] =  (uint8_t)(om_ma >> 0);
  cmd[6] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 7);
}

/**
  * @brief    �޸ıջ�ģʽ������
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    foc_mA 	 ���ջ�ģʽ����������λmA
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_FOC_mA(uint8_t addr, bool svF, uint16_t foc_mA)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x45;                       // ������
  cmd[2] =  0x66;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  (uint8_t)(foc_mA >> 8);			// �ջ�ģʽ����������λmA
	cmd[5] =  (uint8_t)(foc_mA >> 0);
  cmd[6] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 7);
}

/**
  * @brief    ��ȡPID����
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Read_PID_Params(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x21;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 3);
}

/**
  * @brief    �޸�PID����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    pTkp 	 	 ����������λ�û�����ϵ����Ĭ��Ϊ126640
	* @param    pBkp 	 	 ��ֱͨ����λ�û�����ϵ����Ĭ��Ϊ126640
	* @param    vkp 	 	 ���ٶȻ�����ϵ����42Ĭ��Ϊ15600
	* @param    vki 	 	 ���ٶȻ�����ϵ����42Ĭ��Ϊ26
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_PID_Params(uint8_t addr, bool svF, uint32_t pTkp, uint32_t pBkp, uint32_t vkp, uint32_t vki)
{
  __IO static uint8_t cmd[32] = {0};
  
  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0x4A;                      // ������
  cmd[2]  =  0xC3;                      // ������
  cmd[3]  =  svF;                       // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4]  =  (uint8_t)(pTkp >> 24);			// pTkp
	cmd[5]  =  (uint8_t)(pTkp >> 16);
	cmd[6]  =  (uint8_t)(pTkp >> 8);
	cmd[7]  =  (uint8_t)(pTkp >> 0);
	cmd[8]  =  (uint8_t)(pBkp >> 24);			// pBkp
	cmd[9]  =  (uint8_t)(pBkp >> 16);
	cmd[10] =  (uint8_t)(pBkp >> 8);
	cmd[11] =  (uint8_t)(pBkp >> 0);
	cmd[12] =  (uint8_t)(vkp >> 24);			// vkp
	cmd[13] =  (uint8_t)(vkp >> 16);
	cmd[14] =  (uint8_t)(vkp >> 8);
	cmd[15] =  (uint8_t)(vkp >> 0);
	cmd[16] =  (uint8_t)(vki >> 24);			// vki
	cmd[17] =  (uint8_t)(vki >> 16);
	cmd[18] =  (uint8_t)(vki >> 8);
	cmd[19] =  (uint8_t)(vki >> 0);
  cmd[20] =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 21);
}

/**
  * @brief    ��ȡDMX512Э�������X42S/Y42��
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Read_DMX512_Params(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x49;                       // ������
	cmd[2] =  0x78;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 4);
}

/**
  * @brief    ��ȡDMX512Э�������X42S/Y42��
  * @param    addr  		�������ַ
  * @param    svF   		���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    tch				����ͨ������Ĭ��Ϊ192����ֵҪ������ DMX512 ����������ͨ����һ��
	* @param    nch				��ÿ�����ռ�õ�ͨ������Ĭ��Ϊ1��1Ϊ��ͨ��ģʽ,2Ϊ˫ͨ��ģʽ
	* @param    mode			���˶�ģʽ��Ĭ��Ϊ1��0��ʾ���λ��ģʽ�˶���1��ʾ��������ʽλ���˶�
	* @param    vel				����ͨ��ģʽ���˶��ٶȣ�Ĭ��ֵΪ1000�� ��λRPM�� ��1000RPM��
	* @param    acc				�����ٶȣ�acc=������ֵ/8=125������ʱ���˵���顰5.3.12 λ��ģʽ���ƣ�Emm����
	* @param    vel_step	��˫ͨ��ģʽ�ٶȲ�����Ĭ��ֵΪ 10�� ������˶��ٶ�Ϊ(ͨ��ֵ * 10)RPM
	* @param    pos_step	��˫ͨ��ģʽ�˶�������Ĭ��ֵΪ 100�� �����ת���Ƕ�Ϊ(ͨ��ֵ * 10.0)��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_DMX512_Params(uint8_t addr, bool svF, uint16_t tch, uint8_t nch, uint8_t mode, uint16_t vel, uint16_t acc, uint16_t vel_step, uint32_t pos_step)
{
  __IO static uint8_t cmd[32] = {0};
  
  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xD9;                      // ������
  cmd[2]  =  0x90;                      // ������
  cmd[3]  =  svF;                       // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4]  =  (uint8_t)(tch >> 8);     	// ��ͨ����
  cmd[5]  =  (uint8_t)(tch >> 0);
	cmd[6]  =  nch;                       // ÿ�����ռ�õ�ͨ����
	cmd[7]  =  mode;                      // �˶�ģʽ
	cmd[8]  =  (uint8_t)(vel >> 8);     	// ��ͨ��ģʽ���˶��ٶ�
  cmd[9]  =  (uint8_t)(vel >> 0);
	cmd[10] =  (uint8_t)(acc >> 8);     	// ˫ͨ��ģʽ�ٶȲ���
  cmd[11] =  (uint8_t)(acc >> 0);
	cmd[12] =  (uint8_t)(vel_step >> 8);  // ˫ͨ��ģʽ�ٶȲ���
  cmd[13] =  (uint8_t)(vel_step >> 0);
  cmd[14]  = (uint8_t)(pos_step >> 24);	// ˫ͨ��ģʽ�˶�����
  cmd[15]  = (uint8_t)(pos_step >> 16);
  cmd[16] =  (uint8_t)(pos_step >> 8);
  cmd[17] =  (uint8_t)(pos_step >> 0);
  cmd[18] =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 19);
}

/**
  * @brief    ��ȡλ�õ��ﴰ�ڣ�X42S/Y42��
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Read_Pos_Window(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x41;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 3);
}

/**
  * @brief    �޸�λ�õ��ﴰ�ڣ�X42S/Y42��
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    prw 	 	 ��λ�õ��ﴰ�ڣ�Ĭ��ֵΪ8����0.8��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_Pos_Window(uint8_t addr, bool svF, uint16_t prw)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xD1;                       // ������
  cmd[2] =  0x07;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  (uint8_t)(prw >> 8);				// λ�õ��ﴰ�ڣ�Ĭ��ֵΪ8����0.8��
	cmd[5] =  (uint8_t)(prw >> 0);
  cmd[6] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 7);
}

/**
  * @brief    ��ȡ���ȹ������������ֵ��X42S/Y42��
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Read_Otocp(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x13;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 3);
}

/**
  * @brief    �޸Ĺ��ȹ������������ֵ��X42S/Y42��
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    otp 	 	 �����ȱ��������ֵ��Ĭ��100��
	* @param    ocp 	 	 ���������������ֵ��Ĭ��6600mA
	* @param    time_ms  �����ȹ������ʱ�䣬Ĭ��1000ms
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_Otocp(uint8_t addr, bool svF, uint16_t otp, uint16_t ocp, uint16_t time_ms)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xD3;                      // ������
  cmd[2]  =  0x56;                      // ������
  cmd[3]  =  svF;                       // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4]  =  (uint8_t)(otp >> 8);				// ���ȱ��������ֵ
	cmd[5]  =  (uint8_t)(otp >> 0);
	cmd[6]  =  (uint8_t)(ocp >> 8);				// �������������ֵ
	cmd[7]  =  (uint8_t)(ocp >> 0);
	cmd[8]  =  (uint8_t)(time_ms >> 8);		// ���ȹ������ʱ��
	cmd[9]  =  (uint8_t)(time_ms >> 0);
  cmd[10] =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 11);
}

/**
  * @brief    ��ȡ������������ʱ�䣨X42S/Y42��
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Read_Heart_Protect(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x16;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 3);
}

/**
  * @brief    �޸�������������ʱ�䣨X42S/Y42��
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    hp 	 	 	 ����������ʱ�䣬��λ��ms
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_Heart_Protect(uint8_t addr, bool svF, uint32_t hp)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0x68;                      // ������
  cmd[2]  =  0x38;                      // ������
  cmd[3]  =  svF;                       // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4]  =  (uint8_t)(hp >> 24);				// ��������ʱ�䣬��λ��ms
	cmd[5]  =  (uint8_t)(hp >> 16);
	cmd[6]  =  (uint8_t)(hp >> 8);
	cmd[7]  =  (uint8_t)(hp >> 0);
  cmd[8]  =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 9);
}

/**
  * @brief    ��ȡ�����޷�/����ϵ����X42S/Y42��
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Read_Integral_Limit(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x23;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 3);
}

/**
  * @brief    �޸Ļ����޷�/����ϵ����X42S/Y42��
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    il 	 	 	 ������ϵ����X �̼�Ĭ��ΪX42S/Y42/388��X57S/Y57/512
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Modify_Integral_Limit(uint8_t addr, bool svF, uint32_t il)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0x4B;                      // ������
  cmd[2]  =  0x57;                      // ������
  cmd[3]  =  svF;                       // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4]  =  (uint8_t)(il >> 24);				// ����ϵ����X �̼�Ĭ��ΪX42S/Y42/388��X57S/Y57/512
	cmd[5]  =  (uint8_t)(il >> 16);
	cmd[6]  =  (uint8_t)(il >> 8);
	cmd[7]  =  (uint8_t)(il >> 0);
  cmd[8]  =  0x6B;                      // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 9);
}

/**********************************************************
*** ��ȡ����������������
**********************************************************/
/**
  * @brief    ��ȡϵͳ״̬����
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Read_System_State_Params(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x43;                       // ������
	cmd[2] =  0x7A;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 4);
}

/**
  * @brief    ��ȡ�������ò���
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_Read_Motor_Conf_Params(uint8_t addr)
{
  __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x42;                       // ������
	cmd[2] =  0x6C;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ��������
  HAL_UART_Transmit_DMA(&huart5, (uint8_t *)cmd, 4);
}



/**
***********************************************************
***********************************************************
*** 
***
*** @brief	�����ǰ���Ӧ������ص�X42S/Y42���������ϵĺ�����X42S/Y42��
***
*** 
***********************************************************
***********************************************************
***/
/**********************************************************
*** ������������
**********************************************************/
/**
  * @brief    ����������У׼ - ���ص�����ָ����
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Trig_Encoder_Cal(uint8_t addr)
{
  uint8_t j = 0, cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x06;                       // ������
  cmd[2] =  0x45;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 4; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ���������X42S/Y42�� - ���ص�����ָ����
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Reset_Motor(uint8_t addr)
{
  uint8_t j = 0, cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x08;                       // ������
  cmd[2] =  0x97;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 4; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ����ǰλ������ - ���ص�����ָ����
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Reset_CurPos_To_Zero(uint8_t addr)
{
  uint8_t j = 0, cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x0A;                       // ������
  cmd[2] =  0x6D;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 4; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �����ת���� - ���ص�����ָ����
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Reset_Clog_Pro(uint8_t addr)
{
  uint8_t j = 0, cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x0E;                       // ������
  cmd[2] =  0x52;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 4; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �ָ��������� - ���ص�����ָ����
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Restore_Motor(uint8_t addr)
{
  uint8_t j = 0, cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x0F;                       // ������
  cmd[2] =  0x5F;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 4; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**********************************************************
*** �˶���������
**********************************************************/
/**
  * @brief    ʹ���źſ��� - ���ص�����ָ����
  * @param    addr  �������ַ
  * @param    state ��ʹ��״̬     ��trueΪʹ�ܵ����falseΪ�رյ��
  * @param    snF   �����ͬ����־ ��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_En_Control(uint8_t addr, bool state, bool snF)
{
  uint8_t j = 0, cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xF3;                       // ������
  cmd[2] =  0xAB;                       // ������
  cmd[3] =  (uint8_t)state;             // ʹ��״̬
  cmd[4] =  snF;                        // ���ͬ���˶���־
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 6; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ����ģʽ - ���ص�����ָ����
  * @param    addr  	�������ַ
  * @param    sign  	�����ţ�����		��0Ϊ����1Ϊ��
  * @param    t_ramp	������б��(Ma/s)	����Χ0 - 65535Ma/s
  * @param    torque	�����ص���(Ma)		����Χ0 - 6000Ma
  * @param    snF   	�����ͬ����־		��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Torque_Control(uint8_t addr, uint8_t sign, uint16_t t_ramp, uint16_t torque, bool snF)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xF5;                       // ������
  cmd[2] =  sign;                       // ���ţ�����
  cmd[3] =  (uint8_t)(t_ramp >> 8);     // ����б��(Ma/s)
  cmd[4] =  (uint8_t)(t_ramp >> 0);
  cmd[5] =  (uint8_t)(torque >> 8);     // ���ص���(Ma)
  cmd[6] =  (uint8_t)(torque >> 0);
  cmd[7] =  snF;                        // ���ͬ����־
  cmd[8] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 9; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ����ģʽ���ٿ��ƣ�X42S/Y42�� - ���ص�����ָ����
  * @param    addr  	�������ַ
  * @param    sign  	�����ţ�����		��0Ϊ����1Ϊ��
  * @param    t_ramp	������б��(Ma/s)	����Χ0 - 65535Ma/s
  * @param    torque	�����ص���(Ma)		����Χ0 - 6000Ma
  * @param    snF   	�����ͬ����־		��falseΪ�����ã�trueΪ����
	* @param    maxVel	������ٶ�(RPM)	����Χ0.0 - 3000.0RPM
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Torque_LV_Control(uint8_t addr, uint8_t sign, uint16_t t_ramp, uint16_t torque, bool snF, float maxVel)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0}; uint16_t v = 0;

  // ���ٶȷŴ�10�����͹�ȥ
  v = (uint16_t)ABS(maxVel * 10.0f);
  
  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xC5;                      // ������
  cmd[2]  =  sign;                      // ���ţ�����
  cmd[3]  =  (uint8_t)(t_ramp >> 8);    // ����б��(Ma/s)
  cmd[4]  =  (uint8_t)(t_ramp >> 0);
  cmd[5]  =  (uint8_t)(torque >> 8);    // ���ص���(Ma)
  cmd[6]  =  (uint8_t)(torque >> 0);
  cmd[7]  =  snF;                       // ���ͬ����־
  cmd[8]  =  (uint8_t)(v >> 8);    			// ����ٶ�(RPM)
  cmd[9]  =  (uint8_t)(v >> 0);    
  cmd[10] =  0x6B;                      // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 11; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �ٶ�ģʽ - ���ص�����ָ����
  * @param    addr  �������ַ
  * @param    dir   ������						��0ΪCW��1ΪCCW
  * @param    acc   �����ٶ�(RPM/s)	����Χ0 - 65535RPM/s
  * @param    vel		���ٶ�(RPM)			����Χ0.0 - 3000.0RPM
  * @param    snF   �����ͬ����־		��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Vel_Control(uint8_t addr, uint8_t dir, uint16_t acc, float vel, bool snF)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0}; uint16_t v = 0;

  // ���ٶȷŴ�10�����͹�ȥ
  v = (uint16_t)ABS(vel * 10.0f);

  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xF6;                       // ������
  cmd[2] =  dir;                        // ���ţ�����
  cmd[3] =  (uint8_t)(acc >> 8);     		// ���ٶ�(RPM/s)
  cmd[4] =  (uint8_t)(acc >> 0);
  cmd[5] =  (uint8_t)(v >> 8);        	// �ٶ�(RPM)
  cmd[6] =  (uint8_t)(v >> 0);
  cmd[7] =  snF;                        // ���ͬ���˶���־
  cmd[8] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 9; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �ٶ�ģʽ�޵������ƣ�X42S/Y42�� - ���ص�����ָ����
  * @param    addr  �������ַ
  * @param    dir   ������						��0ΪCW��1ΪCCW
  * @param    acc   �����ٶ�(RPM/s)	����Χ0 - 65535RPM/s
  * @param    vel		���ٶ�(RPM)			����Χ0.0 - 3000.0RPM
  * @param    snF   �����ͬ����־		��falseΪ�����ã�trueΪ����
	* @param    maxCur��������(mA)		����Χ0 - 6000mA
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Vel_LC_Control(uint8_t addr, uint8_t dir, uint16_t acc, float vel, bool snF, uint16_t maxCur)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0}; uint16_t v = 0;

  // ���ٶȷŴ�10�����͹�ȥ
  v = (uint16_t)ABS(vel * 10.0f);

  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xC6;                      // ������
  cmd[2]  =  dir;                       // ���ţ�����
  cmd[3]  =  (uint8_t)(acc >> 8);     	// ���ٶ�(RPM/s)
  cmd[4]  =  (uint8_t)(acc >> 0);
  cmd[5]  =  (uint8_t)(v >> 8);        	// �ٶ�(RPM)
  cmd[6]  =  (uint8_t)(v >> 0);
  cmd[7]  =  snF;                       // ���ͬ���˶���־
	cmd[8]  =  (uint8_t)(maxCur >> 8);    // ������(mA)
  cmd[9]  =  (uint8_t)(maxCur >> 0);
  cmd[10] =  0x6B;                      // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 11; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ֱͨ����λ��ģʽ - ���ص�����ָ����
  * @param    addr	�������ַ
  * @param    dir   ������								��0ΪCW��1ΪCCW
  * @param    vel		���˶��ٶ�(RPM)			����Χ0.0 - 3000.0RPM
  * @param    pos		��λ�ýǶ�(��)				����Χ0.0��- (2^32 - 1) / 10��
  * @param    raf   ����λ/�����˶���־	��0Ϊ�����һ������Ŀ��λ�ã�1Ϊ����λ�ã�2Ϊ��Ե�ǰʵʱλ��
  * @param    snF   �����ͬ����־				��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Bypass_Pos_LV_Control(uint8_t addr, uint8_t dir, float vel, float pos, uint8_t raf, bool snF)
{
  uint8_t j = 0; uint8_t cmd[16] = {0}; uint16_t v = 0; uint32_t p = 0;

  // ���ٶȺ�λ�÷Ŵ�10�����͹�ȥ
  v = (uint16_t)ABS(vel * 10.0f); p = (uint32_t)ABS(pos * 10.0f);

  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xFB;                      // ������
  cmd[2]  =  dir;                       // ���ţ�����
  cmd[3]  =  (uint8_t)(v >> 8);       	// �˶��ٶ�(RPM)
  cmd[4]  =  (uint8_t)(v >> 0);
  cmd[5]  =  (uint8_t)(p >> 24);      	// λ�ýǶ�
  cmd[6]  =  (uint8_t)(p >> 16);
  cmd[7]  =  (uint8_t)(p >> 8);
  cmd[8]  =  (uint8_t)(p >> 0);
  cmd[9]  =  raf;                       // ��λ/�����˶���־
  cmd[10] =  snF;                       // ���ͬ���˶���־
  cmd[11] =  0x6B;                      // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 12; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ֱͨ����λ��ģʽ�޵������� - ���ص�����ָ����
  * @param    addr	�������ַ
  * @param    dir   ������								��0ΪCW��1ΪCCW
  * @param    vel		���˶��ٶ�(RPM)			����Χ0.0 - 3000.0RPM
  * @param    pos		��λ�ýǶ�(��)				����Χ0.0��- (2^32 - 1) / 10��
  * @param    raf   ����λ/�����˶���־	��0Ϊ�����һ������Ŀ��λ�ã�1Ϊ����λ�ã�2Ϊ��Ե�ǰʵʱλ��
  * @param    snF   �����ͬ����־				��falseΪ�����ã�trueΪ����
	* @param    maxCur��������(mA)		����Χ0 - 6000mA
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Bypass_Pos_LV_LC_Control(uint8_t addr, uint8_t dir, float vel, float pos, uint8_t raf, bool snF, uint16_t maxCur)
{
  uint8_t j = 0; uint8_t cmd[16] = {0}; uint16_t v = 0; uint32_t p = 0;

  // ���ٶȺ�λ�÷Ŵ�10�����͹�ȥ
  v = (uint16_t)ABS(vel * 10.0f); p = (uint32_t)ABS(pos * 10.0f);

  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xCB;                      // ������
  cmd[2]  =  dir;                       // ���ţ�����
  cmd[3]  =  (uint8_t)(v >> 8);       	// �˶��ٶ�(RPM)
  cmd[4]  =  (uint8_t)(v >> 0);
  cmd[5]  =  (uint8_t)(p >> 24);      	// λ�ýǶ�
  cmd[6]  =  (uint8_t)(p >> 16);
  cmd[7]  =  (uint8_t)(p >> 8);
  cmd[8]  =  (uint8_t)(p >> 0);
  cmd[9]  =  raf;                       // ��λ/�����˶���־
  cmd[10] =  snF;                       // ���ͬ���˶���־
	cmd[11] =  (uint8_t)(maxCur >> 8);    // ������(mA)
  cmd[12] =  (uint8_t)(maxCur >> 0);
  cmd[13] =  0x6B;                      // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 14; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �������߼Ӽ���λ��ģʽ���� - ���ص�����ָ����
  * @param    addr  �������ַ
  * @param    dir   ������								��0ΪCW������ֵΪCCW
  * @param    acc   �����ټ��ٶ�(RPM/s)	��0ΪCW������ֵΪCCW
  * @param    dec   �����ټ��ٶ�(RPM/s)	��0ΪCW������ֵΪCCW
  * @param    vel		������ٶ�(RPM)			����Χ0.0 - 3000.0RPM
  * @param    pos		��λ��(��)						����Χ0.0��- (2^32 - 1)��
  * @param    raf   ����λ/�����˶���־	��0Ϊ�����һ������Ŀ��λ�ã�1Ϊ����λ�ã�2Ϊ��Ե�ǰʵʱλ��
  * @param    snF   �����ͬ����־					��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Traj_Pos_Control(uint8_t addr, uint8_t dir, uint16_t acc, uint16_t dec, float vel, float pos, uint8_t raf, bool snF)
{
  uint8_t j = 0; uint8_t cmd[32] = {0}; uint16_t v = 0; uint32_t p = 0;

  // ���ٶȺ�λ�÷Ŵ�10�����͹�ȥ
  v = (uint16_t)ABS(vel * 10.0f); p = (uint32_t)ABS(pos * 10.0f);

  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xFD;                      // ������
  cmd[2]  =  dir;                       // ���ţ�����
  cmd[3]  =  (uint8_t)(acc >> 8);       // ���ټ��ٶ�(RPM/s)
  cmd[4]  =  (uint8_t)(acc >> 0);
  cmd[5]  =  (uint8_t)(dec >> 8);       // ���ټ��ٶ�(RPM/s)
  cmd[6]  =  (uint8_t)(dec >> 0);
  cmd[7]  =  (uint8_t)(v >> 8);       	// ����ٶ�(RPM)
  cmd[8]  =  (uint8_t)(v >> 0);
  cmd[9]  =  (uint8_t)(p >> 24);      	// λ�ýǶ�
  cmd[10] =  (uint8_t)(p >> 16);
  cmd[11] =  (uint8_t)(p >> 8);
  cmd[12] =  (uint8_t)(p >> 0);
  cmd[13] =  raf;                       // ��λ/�����˶���־
  cmd[14] =  snF;                       // ���ͬ���˶���־
  cmd[15] =  0x6B;                      // У���ֽ�

  // ���ص�ǰ�������������
  for(j=0; j < 16; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �������߼Ӽ���λ��ģʽ�޵������ƣ�X42S/Y42�� - ���ص�����ָ����
  * @param    addr  �������ַ
  * @param    dir   ������								��0ΪCW������ֵΪCCW
  * @param    acc   �����ټ��ٶ�(RPM/s)	��0ΪCW������ֵΪCCW
  * @param    dec   �����ټ��ٶ�(RPM/s)	��0ΪCW������ֵΪCCW
  * @param    vel		������ٶ�(RPM)			����Χ0.0 - 3000.0RPM
  * @param    pos		��λ��(��)						����Χ0.0��- (2^32 - 1)��
  * @param    raf   ����λ/�����˶���־	��0Ϊ�����һ������Ŀ��λ�ã�1Ϊ����λ�ã�2Ϊ��Ե�ǰʵʱλ��
  * @param    snF   �����ͬ����־				��falseΪ�����ã�trueΪ����
	* @param    maxCur��������(mA)				����Χ0 - 6000mA
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Traj_Pos_LC_Control(uint8_t addr, uint8_t dir, uint16_t acc, uint16_t dec, float vel, float pos, uint8_t raf, bool snF, uint16_t maxCur)
{
  uint8_t j = 0; uint8_t cmd[32] = {0}; uint16_t v = 0; uint32_t p = 0;

  // ���ٶȺ�λ�÷Ŵ�10�����͹�ȥ
  v = (uint16_t)ABS(vel * 10.0f); p = (uint32_t)ABS(pos * 10.0f);

  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xCD;                      // ������
  cmd[2]  =  dir;                       // ���ţ�����
  cmd[3]  =  (uint8_t)(acc >> 8);       // ���ټ��ٶ�(RPM/s)
  cmd[4]  =  (uint8_t)(acc >> 0);
  cmd[5]  =  (uint8_t)(dec >> 8);       // ���ټ��ٶ�(RPM/s)
  cmd[6]  =  (uint8_t)(dec >> 0);
  cmd[7]  =  (uint8_t)(v >> 8);       	// ����ٶ�(RPM)
  cmd[8]  =  (uint8_t)(v >> 0);
  cmd[9]  =  (uint8_t)(p >> 24);      	// λ�ýǶ�
  cmd[10] =  (uint8_t)(p >> 16);
  cmd[11] =  (uint8_t)(p >> 8);
  cmd[12] =  (uint8_t)(p >> 0);
  cmd[13] =  raf;                       // ��λ/�����˶���־
  cmd[14] =  snF;                       // ���ͬ���˶���־
	cmd[15] =  (uint8_t)(maxCur >> 8);    // ������(mA)
  cmd[16] =  (uint8_t)(maxCur >> 0);
  cmd[17] =  0x6B;                      // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 18; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ���ÿ�����������λ��ģʽ���˶�������X42S/Y42��
  * @param    addr  �������ַ
  * @param    acc   �����ټ��ٶ�(RPM/s)	��0ΪCW������ֵΪCCW
  * @param    dec   �����ټ��ٶ�(RPM/s)	��0ΪCW������ֵΪCCW
  * @param    vel		������ٶ�(RPM)			����Χ0.0 - 3000.0RPM
  * @param    raf   ����λ/�����˶���־	��0Ϊ�����һ������Ŀ��λ�ã�1Ϊ����λ�ã�2Ϊ��Ե����ǰʵʱλ��
  * @param    snF   �����ͬ����־				��falseΪ�����ã�trueΪ����
	* @param    maxCur��������(mA)				����Χ0 - 6000mA
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Set_QTrajPos_Params(uint8_t addr, uint16_t acc, uint16_t dec, float vel, uint8_t raf, bool snF, uint16_t maxCur)
{
  uint8_t j = 0; uint8_t cmd[32] = {0}; uint16_t v = 0;

  // ���ٶȷŴ�10�����͹�ȥ
  v = (uint16_t)ABS(vel * 10.0f);

  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xF1;                      // ������
  cmd[2]  =  (uint8_t)(acc >> 8);       // ���ټ��ٶ�(RPM/s)
  cmd[3]  =  (uint8_t)(acc >> 0);
  cmd[4]  =  (uint8_t)(dec >> 8);       // ���ټ��ٶ�(RPM/s)
  cmd[5]  =  (uint8_t)(dec >> 0);
  cmd[6]  =  (uint8_t)(v >> 8);       	// ����ٶ�(RPM)
  cmd[7]  =  (uint8_t)(v >> 0);
  cmd[8]  =  raf;                       // ��λ/�����˶���־
  cmd[9]  =  snF;                       // ���ͬ���˶���־
	cmd[10] =  (uint8_t)(maxCur >> 8);    // ������(mA)
  cmd[11] =  (uint8_t)(maxCur >> 0);
  cmd[12] =  0x6B;                      // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 13; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ������������λ��ģʽ���ƣ�X42S/Y42��
  * @param    addr  �������ַ
  * @param    pos		��λ�ýǶ�(������)����λ��0.1��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_QTrajPos_LC_Control(uint8_t addr, float pos)
{
  uint8_t j = 0; uint8_t cmd[32] = {0}; int32_t p = 0;

  // ��λ�÷Ŵ�10�����͹�ȥ������һλС����
  p = (int32_t)ABS(pos * 10.0f);

  // װ������
  cmd[0] =  addr;                      // ��ַ
  cmd[1] =  0xFC;                      // ������
  cmd[2] =  (uint8_t)(p >> 24);      	 // λ�ýǶ�
  cmd[3] =  (uint8_t)(p >> 16);
  cmd[4] =  (uint8_t)(p >> 8);
  cmd[5] =  (uint8_t)(p >> 0);
  cmd[6] =  0x6B;                      // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 7; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ����ֹͣ - ���ص�����ָ����
  * @param    addr  �������ַ
  * @param    snF   �����ͬ����־��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Stop_Now(uint8_t addr, bool snF)
{
  uint8_t j = 0, cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xFE;                       // ������
  cmd[2] =  0x98;                       // ������
  cmd[3] =  snF;                        // ���ͬ���˶���־
  cmd[4] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 5; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ���ͬ���˶� - ���ص�����ָ����
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Synchronous_motion(uint8_t addr)
{
  uint8_t j = 0, cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xFF;                       // ������
  cmd[2] =  0x66;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 4; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**********************************************************
*** ԭ���������
**********************************************************/
/**
  * @brief    ���õ�Ȧ��������λ�� - ���ص�����ָ����
  * @param    addr  �������ַ
  * @param    svF   ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Origin_Set_O(uint8_t addr, bool svF)
{
  uint8_t j = 0, cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x93;                       // ������
  cmd[2] =  0x88;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 5; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �������� - ���ص�����ָ����
  * @param    addr   �������ַ
  * @param    o_mode ������ģʽ��0Ϊ��Ȧ�ͽ����㣬1Ϊ��Ȧ������㣬2Ϊ��Ȧ����λ��ײ���㣬3Ϊ��Ȧ����λ���ػ���
  * @param    snF   �����ͬ����־��falseΪ�����ã�trueΪ����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Origin_Trigger_Return(uint8_t addr, uint8_t o_mode, bool snF)
{
  uint8_t j = 0, cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x9A;                       // ������
  cmd[2] =  o_mode;                     // ����ģʽ��0Ϊ��Ȧ�ͽ����㣬1Ϊ��Ȧ������㣬2Ϊ��Ȧ����λ��ײ���㣬3Ϊ��Ȧ����λ���ػ���
  cmd[3] =  snF;                        // ���ͬ���˶���־��falseΪ�����ã�trueΪ����
  cmd[4] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 5; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ǿ���жϲ��˳����� - ���ص�����ָ����
  * @param    addr  �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Origin_Interrupt(uint8_t addr)
{
  uint8_t j = 0, cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x9C;                       // ������
  cmd[2] =  0x48;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 4; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸Ļ������ - ���ص�����ָ����
  * @param    addr  �������ַ
  * @param    svF   ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    o_mode ������ģʽ��0Ϊ��Ȧ�ͽ����㣬1Ϊ��Ȧ������㣬2Ϊ��Ȧ����λ��ײ���㣬3Ϊ��Ȧ����λ���ػ���
  * @param    o_dir  �����㷽��0ΪCW������ֵΪCCW
  * @param    o_vel  �������ٶȣ���λ��RPM��ת/���ӣ�
  * @param    o_tm   �����㳬ʱʱ�䣬��λ������
  * @param    sl_vel ������λ��ײ������ת�٣���λ��RPM��ת/���ӣ�
  * @param    sl_ma  ������λ��ײ�������������λ��Ma��������
  * @param    sl_ms  ������λ��ײ������ʱ�䣬��λ��Ms�����룩
  * @param    potF   ���ϵ��Զ��������㣬falseΪ��ʹ�ܣ�trueΪʹ��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Origin_Modify_Params(uint8_t addr, bool svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, bool potF)
{
  uint8_t j = 0, cmd[32] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x4C;                       // ������
  cmd[2] =  0xAE;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  o_mode;                     // ����ģʽ��0Ϊ��Ȧ�ͽ����㣬1Ϊ��Ȧ������㣬2Ϊ��Ȧ����λ��ײ���㣬3Ϊ��Ȧ����λ���ػ���
  cmd[5] =  o_dir;                      // ���㷽��
  cmd[6]  =  (uint8_t)(o_vel >> 8);     // �����ٶ�(RPM)��8λ�ֽ�
  cmd[7]  =  (uint8_t)(o_vel >> 0);     // �����ٶ�(RPM)��8λ�ֽ� 
  cmd[8]  =  (uint8_t)(o_tm >> 24);     // ���㳬ʱʱ��(bit24 - bit31)
  cmd[9]  =  (uint8_t)(o_tm >> 16);     // ���㳬ʱʱ��(bit16 - bit23)
  cmd[10] =  (uint8_t)(o_tm >> 8);      // ���㳬ʱʱ��(bit8  - bit15)
  cmd[11] =  (uint8_t)(o_tm >> 0);      // ���㳬ʱʱ��(bit0  - bit7 )
  cmd[12] =  (uint8_t)(sl_vel >> 8);    // ����λ��ײ������ת��(RPM)��8λ�ֽ�
  cmd[13] =  (uint8_t)(sl_vel >> 0);    // ����λ��ײ������ת��(RPM)��8λ�ֽ� 
  cmd[14] =  (uint8_t)(sl_ma >> 8);     // ����λ��ײ���������(Ma)��8λ�ֽ�
  cmd[15] =  (uint8_t)(sl_ma >> 0);     // ����λ��ײ���������(Ma)��8λ�ֽ� 
  cmd[16] =  (uint8_t)(sl_ms >> 8);     // ����λ��ײ������ʱ��(Ms)��8λ�ֽ�
  cmd[17] =  (uint8_t)(sl_ms >> 0);     // ����λ��ײ������ʱ��(Ms)��8λ�ֽ�
  cmd[18] =  potF;                      // �ϵ��Զ��������㣬falseΪ��ʹ�ܣ�trueΪʹ��
  cmd[19] =  0x6B;                      // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 20; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ��ȡ��ײ���㷵�ؽǶȣ�X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Origin_Read_SL_RP(uint8_t addr)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x3F;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 3; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸���ײ���㷵�ؽǶȣ�X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    sl_rp 	 ����ײ���㷵�ؽǶȣ���λ0.1�㣬����40������4.0��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Origin_Modify_SL_RP(uint8_t addr, bool svF, uint16_t sl_rp)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0x5C;                      // ������
  cmd[2]  =  0xAC;                      // ������
  cmd[3]  =  svF;                       // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4]  =  (uint8_t)(sl_rp >> 8);			// ��ײ���㷵�ؽǶȣ���λ0.1��
	cmd[5]  =  (uint8_t)(sl_rp >> 0);
  cmd[6]  =  0x6B;                      // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 7; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**********************************************************
*** ��ȡϵͳ��������
**********************************************************/
/**
  * @brief    ��ʱ������Ϣ���X42S/Y42�� - ���ص�����ָ����
  * @param    addr  	�������ַ
  * @param    s     	��ϵͳ��������
	* @param    time_ms ����ʱʱ��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Auto_Return_Sys_Params_Timed(uint8_t addr, SysParams_t s, uint16_t time_ms)
{
  uint8_t i = 0, j = 0; uint8_t cmd[16] = {0};
  
  // װ������
  cmd[i] = addr; ++i;                   // ��ַ

  cmd[i] = 0x11; ++i;                   // ������

  cmd[i] = 0x18; ++i;                   // ������

  switch(s)                             // ��Ϣ������
  {
    case S_VBUS : cmd[i] = 0x24; ++i; break;	// ��ȡ���ߵ�ѹ
		case S_CBUS : cmd[i] = 0x26; ++i; break;	// ��ȡ���ߵ���
    case S_CPHA : cmd[i] = 0x27; ++i; break;	// ��ȡ�����
		case S_ENCO : cmd[i] = 0x29; ++i; break;	// ��ȡ������ԭʼֵ
		case S_CLKC : cmd[i] = 0x30; ++i; break;	// ��ȡʵʱ������
    case S_ENCL : cmd[i] = 0x31; ++i; break;	// ��ȡ�������Ի�У׼��ı�����ֵ
		case S_CLKI : cmd[i] = 0x32; ++i; break;	// ��ȡ����������
    case S_TPOS : cmd[i] = 0x33; ++i; break;	// ��ȡ���Ŀ��λ��
    case S_SPOS : cmd[i] = 0x34; ++i; break;	// ��ȡ���ʵʱ�趨��Ŀ��λ��
		case S_VEL  : cmd[i] = 0x35; ++i; break;	// ��ȡ���ʵʱת��
    case S_CPOS : cmd[i] = 0x36; ++i; break;	// ��ȡ���ʵʱλ��
    case S_PERR : cmd[i] = 0x37; ++i; break;	// ��ȡ���λ�����
		case S_VBAT : cmd[i] = 0x38; ++i; break;	// ��ȡ��Ȧ��������ص�ѹ��Y42��
		case S_TEMP : cmd[i] = 0x39; ++i; break;	// ��ȡ���ʵʱ�¶ȣ�X42S/Y42��
    case S_FLAG : cmd[i] = 0x3A; ++i; break;	// ��ȡ���״̬��־λ
    case S_OFLAG: cmd[i] = 0x3B; ++i; break;	// ��ȡ����״̬��־λ
		case S_OAF  : cmd[i] = 0x3C; ++i; break;	// ��ȡ���״̬��־λ + ����״̬��־λ��X42S/Y42��
		case S_PIN  : cmd[i] = 0x3D; ++i; break;	// ��ȡ����״̬��X42S/Y42��
    default: break;
  }
	
	cmd[i] = (uint8_t)(time_ms >> 8);  ++i;	// ��ʱʱ��
	cmd[i] = (uint8_t)(time_ms >> 0);  ++i;

  cmd[i] = 0x6B; ++i;                   	// У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < i; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ��ȡϵͳ���� - ���ص�����ָ����
  * @param    addr  �������ַ
  * @param    s     ��ϵͳ��������
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Read_Sys_Params(uint8_t addr, SysParams_t s)
{
  uint8_t i = 0, j = 0; uint8_t cmd[16] = {0};
  
  // װ������
  cmd[i] = addr; ++i;                   // ��ַ

  switch(s)                             // ������
  {
    case S_VBUS : cmd[i] = 0x24; ++i; break;	// ��ȡ���ߵ�ѹ
		case S_CBUS : cmd[i] = 0x26; ++i; break;	// ��ȡ���ߵ���
    case S_CPHA : cmd[i] = 0x27; ++i; break;	// ��ȡ�����
		case S_ENCO : cmd[i] = 0x29; ++i; break;	// ��ȡ������ԭʼֵ
		case S_CLKC : cmd[i] = 0x30; ++i; break;	// ��ȡʵʱ������
    case S_ENCL : cmd[i] = 0x31; ++i; break;	// ��ȡ�������Ի�У׼��ı�����ֵ
		case S_CLKI : cmd[i] = 0x32; ++i; break;	// ��ȡ����������
    case S_TPOS : cmd[i] = 0x33; ++i; break;	// ��ȡ���Ŀ��λ��
    case S_SPOS : cmd[i] = 0x34; ++i; break;	// ��ȡ���ʵʱ�趨��Ŀ��λ��
		case S_VEL  : cmd[i] = 0x35; ++i; break;	// ��ȡ���ʵʱת��
    case S_CPOS : cmd[i] = 0x36; ++i; break;	// ��ȡ���ʵʱλ��
    case S_PERR : cmd[i] = 0x37; ++i; break;	// ��ȡ���λ�����
		case S_VBAT : cmd[i] = 0x38; ++i; break;	// ��ȡ��Ȧ��������ص�ѹ��Y42��
		case S_TEMP : cmd[i] = 0x39; ++i; break;	// ��ȡ���ʵʱ�¶ȣ�X42S/Y42��
    case S_FLAG : cmd[i] = 0x3A; ++i; break;	// ��ȡ���״̬��־λ
    case S_OFLAG: cmd[i] = 0x3B; ++i; break;	// ��ȡ����״̬��־λ
		case S_OAF  : cmd[i] = 0x3C; ++i; break;	// ��ȡ���״̬��־λ + ����״̬��־λ��X42S/Y42��
		case S_PIN  : cmd[i] = 0x3D; ++i; break;	// ��ȡ����״̬��X42S/Y42��
    case S_SYS  : cmd[i] = 0x43; ++i; cmd[i] = 0x7A; ++i; break;	// ��ȡϵͳ״̬����
		default: break;
  }

  cmd[i] = 0x6B; ++i;                   // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < i; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**********************************************************
*** ��д������������
**********************************************************/
/**
  * @brief    �޸ĵ��ID��ַ - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    id			 ��Ĭ�ϵ��IDΪ1�����޸�Ϊ1-255��0Ϊ�㲥��ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_Motor_ID(uint8_t addr, bool svF, uint8_t id)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xAE;                       // ������
  cmd[2] =  0x4B;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  id;                  				// Ĭ�ϵ��IDΪ1�����޸�Ϊ1-255��0Ϊ�㲥��ַ
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 6; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸�ϸ��ֵ - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    mstep		 ��Ĭ��ϸ��Ϊ16�����޸�Ϊ1-2556��0Ϊ256ϸ��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_MicroStep(uint8_t addr, bool svF, uint8_t mstep)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x84;                       // ������
  cmd[2] =  0x8A;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  mstep;                 	 		// Ĭ��ϸ��Ϊ16�����޸�Ϊ1-2556��0Ϊ256ϸ��
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 6; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸ĵ����־ - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    pdf		 	 �������־
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_PDFlag(uint8_t addr, bool pdf)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x50;                       // ������
  cmd[2] =  pdf;                 	 			// �����־
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 4; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ��ȡѡ�����״̬��X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Read_Opt_Param_Sta(uint8_t addr)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x1A;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 3; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸ĵ�����ͣ�X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    mottype	 ��������ͣ�Ĭ��Ϊ0��0��ʾ1.8�㲽�������1��ʾ0.9�㲽�����
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_Motor_Type(uint8_t addr, bool svF, bool mottype)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0}; uint8_t MotType = 0;
  
	if(mottype) { MotType = 25; } else { MotType = 50; }
	
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xD7;                       // ������
  cmd[2] =  0x35;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  MotType;                 	 	// ������ͣ�0��ʾ0.9�㲽�������1��ʾ1.8�㲽�����
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 6; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸Ĺ̼����ͣ�X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    fwtype	 ���̼����ͣ�Ĭ��Ϊ0��0ΪX�̼���1ΪEmm�̼�
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_Firmware_Type(uint8_t addr, bool svF, bool fwtype)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xD5;                       // ������
  cmd[2] =  0x69;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  fwtype;                 	 	// ������ͣ�25��ʾ0.9�㲽�������50��ʾ1.8�㲽�����
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 6; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸Ŀ���/�ջ�����ģʽ��X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    ctrl_mode������ģʽ��Ĭ��Ϊ1,0Ϊ����ģʽ��1Ϊ�ջ�FOCģʽ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_Ctrl_Mode(uint8_t addr, bool svF, bool ctrl_mode)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x46;                       // ������
  cmd[2] =  0x69;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  ctrl_mode;                  // ����ģʽ��Ĭ��Ϊ1,0Ϊ����ģʽ��1Ϊ�ջ�FOCģʽ
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 6; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸ĵ���˶�������X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    dir			 ������˶�������Ĭ��ΪCW��0ΪCW��˳ʱ�뷽�򣩣�1ΪCCW
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_Motor_Dir(uint8_t addr, bool svF, bool dir)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xD4;                       // ������
  cmd[2] =  0x60;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  dir;                  			// ����˶�������Ĭ��ΪCW��0ΪCW��˳ʱ�뷽�򣩣�1ΪCCW
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 6; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸������������ܣ�X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    lock		 �������������ܣ�Ĭ��ΪDisable��0ΪDisable��1ΪEnable
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_Lock_Btn(uint8_t addr, bool svF, bool lock)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xD0;                       // ������
  cmd[2] =  0xB3;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  lock;                  			// �����������ܣ�Ĭ��ΪDisable��0ΪDisable����1ΪEnable
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 6; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸�����λ�ýǶ��Ƿ������С10�����루X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    s_vel		 ������λ�ýǶ��Ƿ������С10�����룬Ĭ��ΪDisable��0ΪDisable��1ΪEnable
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_S_Vel(uint8_t addr, bool svF, bool s_vel)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x4F;                       // ������
  cmd[2] =  0x71;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  s_vel;                  		// ����λ�ýǶ��Ƿ������С10�����룬Ĭ��ΪDisable��0ΪDisable��1ΪEnable
  cmd[5] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 6; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸Ŀ���ģʽ�������� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    om_ma 	 ������ģʽ������������λmA
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_OM_mA(uint8_t addr, bool svF, uint16_t om_ma)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x44;                       // ������
  cmd[2] =  0x33;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  (uint8_t)(om_ma >> 8);			// ����ģʽ������������λmA
	cmd[5] =  (uint8_t)(om_ma >> 0);
  cmd[6] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 7; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸ıջ�ģʽ������ - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    foc_mA 	 ���ջ�ģʽ����������λmA
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_FOC_mA(uint8_t addr, bool svF, uint16_t foc_mA)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x45;                       // ������
  cmd[2] =  0x66;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  (uint8_t)(foc_mA >> 8);			// �ջ�ģʽ����������λmA
	cmd[5] =  (uint8_t)(foc_mA >> 0);
  cmd[6] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 7; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ��ȡPID���� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Read_PID_Params(uint8_t addr)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x21;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 3; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸�PID���� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    pTkp 	 	 ����������λ�û�����ϵ����Ĭ��Ϊ126640
	* @param    pBkp 	 	 ��ֱͨ����λ�û�����ϵ����Ĭ��Ϊ126640
	* @param    vkp 	 	 ���ٶȻ�����ϵ����42Ĭ��Ϊ15600
	* @param    vki 	 	 ���ٶȻ�����ϵ����42Ĭ��Ϊ26
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_PID_Params(uint8_t addr, bool svF, uint32_t pTkp, uint32_t pBkp, uint32_t vkp, uint32_t vki)
{
  uint8_t j = 0; __IO static uint8_t cmd[32] = {0};
  
  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0x4A;                      // ������
  cmd[2]  =  0xC3;                      // ������
  cmd[3]  =  svF;                       // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4]  =  (uint8_t)(pTkp >> 24);			// pTkp
	cmd[5]  =  (uint8_t)(pTkp >> 16);
	cmd[6]  =  (uint8_t)(pTkp >> 8);
	cmd[7]  =  (uint8_t)(pTkp >> 0);
	cmd[8]  =  (uint8_t)(pBkp >> 24);			// pBkp
	cmd[9]  =  (uint8_t)(pBkp >> 16);
	cmd[10] =  (uint8_t)(pBkp >> 8);
	cmd[11] =  (uint8_t)(pBkp >> 0);
	cmd[12] =  (uint8_t)(vkp >> 24);			// vkp
	cmd[13] =  (uint8_t)(vkp >> 16);
	cmd[14] =  (uint8_t)(vkp >> 8);
	cmd[15] =  (uint8_t)(vkp >> 0);
	cmd[16] =  (uint8_t)(vki >> 24);			// vki
	cmd[17] =  (uint8_t)(vki >> 16);
	cmd[18] =  (uint8_t)(vki >> 8);
	cmd[19] =  (uint8_t)(vki >> 0);
  cmd[20] =  0x6B;                      // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 21; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ��ȡDMX512Э�������X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Read_DMX512_Params(uint8_t addr)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x49;                       // ������
	cmd[2] =  0x78;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 4; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ��ȡDMX512Э�������X42S/Y42�� - ���ص�����ָ����
  * @param    addr  		�������ַ
  * @param    svF   		���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    tch				����ͨ������Ĭ��Ϊ192����ֵҪ������ DMX512 ����������ͨ����һ��
	* @param    nch				��ÿ�����ռ�õ�ͨ������Ĭ��Ϊ1��1Ϊ��ͨ��ģʽ,2Ϊ˫ͨ��ģʽ
	* @param    mode			���˶�ģʽ��Ĭ��Ϊ1��0��ʾ���λ��ģʽ�˶���1��ʾ��������ʽλ���˶�
	* @param    vel				����ͨ��ģʽ���˶��ٶȣ�Ĭ��ֵΪ1000�� ��λRPM�� ��1000RPM��
	* @param    acc				�����ٶȣ�acc=������ֵ/8=125������ʱ���˵���顰5.3.12 λ��ģʽ���ƣ�Emm����
	* @param    vel_step	��˫ͨ��ģʽ�ٶȲ�����Ĭ��ֵΪ 10�� ������˶��ٶ�Ϊ(ͨ��ֵ * 10)RPM
	* @param    pos_step	��˫ͨ��ģʽ�˶�������Ĭ��ֵΪ 100�� �����ת���Ƕ�Ϊ(ͨ��ֵ * 10.0)��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_DMX512_Params(uint8_t addr, bool svF, uint16_t tch, uint8_t nch, uint8_t mode, uint16_t vel, uint16_t acc, uint16_t vel_step, uint32_t pos_step)
{
  uint8_t j = 0; __IO static uint8_t cmd[32] = {0};
  
  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xD9;                      // ������
  cmd[2]  =  0x90;                      // ������
  cmd[3]  =  svF;                       // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4]  =  (uint8_t)(tch >> 8);     	// ��ͨ����
  cmd[5]  =  (uint8_t)(tch >> 0);
	cmd[6]  =  nch;                       // ÿ�����ռ�õ�ͨ����
	cmd[7]  =  mode;                      // �˶�ģʽ
	cmd[8]  =  (uint8_t)(vel >> 8);     	// ��ͨ��ģʽ���˶��ٶ�
  cmd[9]  =  (uint8_t)(vel >> 0);
	cmd[10] =  (uint8_t)(acc >> 8);     	// ˫ͨ��ģʽ�ٶȲ���
  cmd[11] =  (uint8_t)(acc >> 0);
	cmd[12] =  (uint8_t)(vel_step >> 8);  // ˫ͨ��ģʽ�ٶȲ���
  cmd[13] =  (uint8_t)(vel_step >> 0);
  cmd[14]  = (uint8_t)(pos_step >> 24);	// ˫ͨ��ģʽ�˶�����
  cmd[15]  = (uint8_t)(pos_step >> 16);
  cmd[16] =  (uint8_t)(pos_step >> 8);
  cmd[17] =  (uint8_t)(pos_step >> 0);
  cmd[18] =  0x6B;                      // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 19; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ��ȡλ�õ��ﴰ�ڣ�X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Read_Pos_Window(uint8_t addr)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x41;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 3; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸�λ�õ��ﴰ�ڣ�X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    prw 	 	 ��λ�õ��ﴰ�ڣ�Ĭ��ֵΪ8����0.8��
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_Pos_Window(uint8_t addr, bool svF, uint16_t prw)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0xD1;                       // ������
  cmd[2] =  0x07;                       // ������
  cmd[3] =  svF;                        // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4] =  (uint8_t)(prw >> 8);				// λ�õ��ﴰ�ڣ�Ĭ��ֵΪ8����0.8��
	cmd[5] =  (uint8_t)(prw >> 0);
  cmd[6] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 7; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ��ȡ���ȹ������������ֵ��X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Read_Otocp(uint8_t addr)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x13;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 3; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸Ĺ��ȹ������������ֵ��X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    otp 	 	 �����ȱ��������ֵ��Ĭ��100��
	* @param    ocp 	 	 ���������������ֵ��Ĭ��6600mA
	* @param    time_ms  �����ȹ������ʱ�䣬Ĭ��1000ms
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_Otocp(uint8_t addr, bool svF, uint16_t otp, uint16_t ocp, uint16_t time_ms)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0xD3;                      // ������
  cmd[2]  =  0x56;                      // ������
  cmd[3]  =  svF;                       // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4]  =  (uint8_t)(otp >> 8);				// ���ȱ��������ֵ
	cmd[5]  =  (uint8_t)(otp >> 0);
	cmd[6]  =  (uint8_t)(ocp >> 8);				// �������������ֵ
	cmd[7]  =  (uint8_t)(ocp >> 0);
	cmd[8]  =  (uint8_t)(time_ms >> 8);		// ���ȹ������ʱ��
	cmd[9]  =  (uint8_t)(time_ms >> 0);
  cmd[10] =  0x6B;                      // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 11; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ��ȡ������������ʱ�䣨X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Read_Heart_Protect(uint8_t addr)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x16;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 3; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸�������������ʱ�䣨X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    hp 	 	 	 ����������ʱ�䣬��λ��ms
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_Heart_Protect(uint8_t addr, bool svF, uint32_t hp)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0x68;                      // ������
  cmd[2]  =  0x38;                      // ������
  cmd[3]  =  svF;                       // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4]  =  (uint8_t)(hp >> 24);				// ��������ʱ�䣬��λ��ms
	cmd[5]  =  (uint8_t)(hp >> 16);
	cmd[6]  =  (uint8_t)(hp >> 8);
	cmd[7]  =  (uint8_t)(hp >> 0);
  cmd[8]  =  0x6B;                      // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 9; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ��ȡ�����޷�/����ϵ����X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Read_Integral_Limit(uint8_t addr)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x23;                       // ������
  cmd[2] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 3; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    �޸Ļ����޷�/����ϵ����X42S/Y42�� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @param    svF      ���Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  * @param    il 	 	 	 ������ϵ����X �̼�Ĭ��ΪX42S/Y42/388��X57S/Y57/512
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Modify_Integral_Limit(uint8_t addr, bool svF, uint32_t il)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0]  =  addr;                      // ��ַ
  cmd[1]  =  0x4B;                      // ������
  cmd[2]  =  0x57;                      // ������
  cmd[3]  =  svF;                       // �Ƿ�洢��־��falseΪ���洢��trueΪ�洢
  cmd[4]  =  (uint8_t)(il >> 24);				// ����ϵ����X �̼�Ĭ��ΪX42S/Y42/388��X57S/Y57/512
	cmd[5]  =  (uint8_t)(il >> 16);
	cmd[6]  =  (uint8_t)(il >> 8);
	cmd[7]  =  (uint8_t)(il >> 0);
  cmd[8]  =  0x6B;                      // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 9; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**********************************************************
*** ��ȡ����������������
**********************************************************/
/**
  * @brief    ��ȡϵͳ״̬���� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Read_System_State_Params(uint8_t addr)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x43;                       // ������
	cmd[2] =  0x7A;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 4; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}

/**
  * @brief    ��ȡ�������ò��� - ���ص�����ָ����
  * @param    addr     �������ַ
  * @retval   ��ַ + ������ + ����״̬ + У���ֽ�
  */
void X_V2_MMCL_Read_Motor_Conf_Params(uint8_t addr)
{
  uint8_t j = 0; __IO static uint8_t cmd[16] = {0};
  
  // װ������
  cmd[0] =  addr;                       // ��ַ
  cmd[1] =  0x42;                       // ������
	cmd[2] =  0x6C;                       // ������
  cmd[3] =  0x6B;                       // У���ֽ�
  
  // ���ص�ǰ�������������
  for(j=0; j < 4; j++) { MMCL_cmd[MMCL_count] = cmd[j]; ++MMCL_count; }
}
