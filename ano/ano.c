#include "ano.h"

/* 匿名V7上位机协议	*/
uint8_t Data_Buff[ANO_BUFFER_SIZE] ={ANO_FRAME_HEAD,ANO_FRAME_ADDRESS,ANO_ID_DATA};
uint8_t Data_Sent[ANO_BUFFER_SIZE] ={ANO_FRAME_HEAD,ANO_FRAME_ADDRESS,ANO_ID_WRITE_PARAMETER};
uint8_t Data_Check[12]={ANO_FRAME_HEAD,ANO_FRAME_ADDRESS,ANO_ID_CHECK};

uint8_t Data_Get[ANO_BUFFER_SIZE];

/**************************************************************************
函数功能：发送上位机绘制波形函数
入口参数：发送的参数（32位，32位，32位，32位）
返回  值：无
**************************************************************************/
void Data_send(int32_t _a,int32_t _b,int32_t _c,int32_t _d)
{
	uint8_t i,cnt=4;	/* 计数 */
	uint8_t sc=0,ac=0;	/* 校验 */
	
	Data_Buff[cnt++] = BYTE0(_a);
	Data_Buff[cnt++] = BYTE1(_a);
    Data_Buff[cnt++] = BYTE2(_a);
	Data_Buff[cnt++] = BYTE3(_a);
	
	Data_Buff[cnt++] = BYTE0(_b);
	Data_Buff[cnt++] = BYTE1(_b);
	Data_Buff[cnt++] = BYTE2(_b);
	Data_Buff[cnt++] = BYTE3(_b);
	
	Data_Buff[cnt++] = BYTE0(_c);
	Data_Buff[cnt++] = BYTE1(_c);
    Data_Buff[cnt++] = BYTE2(_c);
	Data_Buff[cnt++] = BYTE3(_c);
	
	Data_Buff[cnt++] = BYTE0(_d);
	Data_Buff[cnt++] = BYTE1(_d);
    Data_Buff[cnt++] = BYTE2(_d);
	Data_Buff[cnt++] = BYTE3(_d);


	Data_Buff[3]=cnt-4;
	
	for(i=0;i<cnt;i++)	/* 校验位计算 */
	{
		sc+=Data_Buff[i];
		ac+=sc;
	}
	
	Data_Buff[cnt++] = sc;
	Data_Buff[cnt++] = ac;
    
    HAL_UART_Transmit(&ANO_UART_HANDLE,Data_Buff,cnt,ANO_UART_TIMEOUT_MS);
}

/**************************************************************************
函数功能：发送数据，内部调用
入口参数：id，数据
返回  值：无
**************************************************************************/
void Ano_SentPar(uint16_t id,int32_t data)
{
    uint8_t cnt=4,i=0,sc=0,ac=0;
    
    Data_Sent[cnt++] = BYTE0(id);
	Data_Sent[cnt++] = BYTE1(id);

	
	Data_Sent[cnt++] = BYTE0(data);
	Data_Sent[cnt++] = BYTE1(data);
	Data_Sent[cnt++] = BYTE2(data);
	Data_Sent[cnt++] = BYTE3(data);
    
    Data_Sent[3]=cnt-4;
    
    for(i=0;i<cnt;i++)		/* 校验位计算 */
	{
		sc+=Data_Sent[i];
		ac+=sc;
	}
	
	Data_Sent[cnt++] = sc;
	Data_Sent[cnt++] = ac;
    
    for( i = 0 ; i < cnt; i++) 
	{
        while((ANO_UART_INSTANCE->SR&0X40)==0);
        ANO_UART_INSTANCE->DR = Data_Sent[i];
	}   
}

/**************************************************************************
函数功能：返回给上位机的校验数据，接受到上位机数据写入时通信使用，内部调用
入口参数：id，和校验，附加校验
返回  值：无
**************************************************************************/
void Ano_SentCheck(uint8_t id,uint8_t _sc,uint8_t _ac)  /* 校验帧 */
{
    uint8_t cnt=4,i=0,sc=0,ac=0;
    
    Data_Check[cnt++] = id;
	Data_Check[cnt++] = _sc;
    Data_Check[cnt++] = _ac;
    
    Data_Check[3]=cnt-4;
    
    for(i=0;i<cnt;i++)		/* 校验位计算 */
	{
		sc+=Data_Check[i];
		ac+=sc;
	}
	
	Data_Check[cnt++] = sc;
	Data_Check[cnt++] = ac;
    
    for( i = 0 ; i < cnt; i++) 
	{
        while((ANO_UART_INSTANCE->SR&0X40)==0);
        ANO_UART_INSTANCE->DR = Data_Check[i];
	}    
}

/**************************************************************************
函数功能：匿名协议数据解析函数
入口参数：数据包
id   号 ：1    2   3   4
参    数：KP   KI  KD  num
返 回 值：无
**************************************************************************/
void Ano_Anl(uint8_t *Data_Pack)          /* 数据解析 */
{
    uint8_t sc = 0,ac = 0;
    
    for(uint8_t i=0;i<Data_Pack[3] + 4;i++)
    {
        sc += Data_Pack[i];
        ac += sc;
    }
    if(sc != Data_Pack[Data_Pack[3]+4] || ac!= Data_Pack[Data_Pack[3]+5])
    {
        return;
    } 
    
    if(Data_Pack[2] == ANO_ID_READ_PARAMETER) {
      uint16_t id = Data_Pack[4] + ((uint16_t)Data_Pack[5]<<8);
      switch(id) {
          case 1:  Ano_SentPar(id, *(int32_t *)&left_pid.kp);     break;  // 读左 Kp
          case 2:  Ano_SentPar(id, *(int32_t *)&left_pid.ki);     break;  // 读左 Ki
          case 3:  Ano_SentPar(id, *(int32_t *)&left_pid.kd);     break;  // 读左 Kd
          case 4:  Ano_SentPar(id, *(int32_t *)&left_pid.target); break;  // 读左目标
          case 5:  Ano_SentPar(id, *(int32_t *)&right_pid.kp);    break;  // 读右 Kp
          case 6:  Ano_SentPar(id, *(int32_t *)&right_pid.ki);    break;  // 读右 Ki
          case 7:  Ano_SentPar(id, *(int32_t *)&right_pid.kd);    break;  // 读右 Kd
          case 8:  Ano_SentPar(id, *(int32_t *)&right_pid.target);break;  // 读右目标
          default: Ano_SentPar(id, 0); break;
      }
    }
    
    if(Data_Pack[2] == ANO_ID_WRITE_PARAMETER) {
      uint16_t id = Data_Pack[4] + ((uint16_t)Data_Pack[5]<<8);
      switch(id) {
          case 1:  left_pid.kp     = *(float *)(&Data_Pack[6]);  break;
          case 2:  left_pid.ki     = *(float *)(&Data_Pack[6]);  break;
          case 3:  left_pid.kd     = *(float *)(&Data_Pack[6]);  break;
          case 4:  left_pid.target = *(float *)(&Data_Pack[6]);  break;
          case 5:  right_pid.kp    = *(float *)(&Data_Pack[6]);  break;
          case 6:  right_pid.ki    = *(float *)(&Data_Pack[6]);  break;
          case 7:  right_pid.kd    = *(float *)(&Data_Pack[6]);  break;
          case 8:  right_pid.target = *(float *)(&Data_Pack[6]);break;
      }
    }
    Ano_SentCheck(Data_Pack[2],Data_Pack[Data_Pack[3]+4],Data_Pack[Data_Pack[3]+5]); /* 返回校验 */
}

/**************************************************************************
函数功能：匿名协议数据校验
入口参数：收到的一帧数据
返回  值：无
**************************************************************************/
void Ano_GetByte(uint8_t data)
{
    static uint8_t sta=0,datalen=0,datacnt=0;      /* 帧长度，接收到长度 */ 
    
    if(sta==0)
    {
        datalen=0;
        datacnt=0;
        
        Data_Get[0]=data;
        if(data==ANO_FRAME_HEAD)       /* 帧头 */
            sta=1;
    }
    else if(sta==1)          /* 地址 */
    {
        Data_Get[1]=data;
        sta=2;
    }
    else if(sta==2)          /* ID */
    {
        Data_Get[2]=data;
        sta=3;
    }
    else if(sta==3)          /* 数据长度 */
    {
        if(data > (ANO_BUFFER_SIZE - 6U))
        {
            sta=0;
            return;
        }
        sta=4;
        Data_Get[3]=data;
        datalen=data;
    }
    else if(sta==4)         /* 数据缓存 */
    {
        Data_Get[4+datacnt++]=data;
        if(datacnt >= datalen)
        {
            sta=5;
        }
    }
    else if(sta==5)         /* 和校验 */
    {
        Data_Get[4+datacnt++]=data;
        sta=6;
    }
    else if(sta==6)         /* 附加校验 */
    {
        sta=0;
        Data_Get[4+datacnt++]=data;
        
        Ano_Anl(Data_Get);  /* 数据解析 */
    }     
}

