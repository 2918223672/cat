//#include "adc_app.h"

//// 微秒级粗略延时，用于等待模拟开关切换后的电压稳定
//static void delay_us(uint32_t us) {
//    uint32_t delay = (SystemCoreClock / 4000000) * us;
//    while (delay--) {
//        __NOP();
//    }
//}

//// 单个通道ADC值
//uint16_t adc_value_one(uint8_t times) 
//{
//    int32_t adc_sum = 0;

//    for(uint8_t i = 0; i < times; i++)
//    {
//        HAL_ADC_Start(&hadc1);                        
//        HAL_ADC_PollForConversion(&hadc1, 5);        // 等待转换完成，超时设为5ms
//        adc_sum += HAL_ADC_GetValue(&hadc1); 
//        HAL_ADC_Stop(&hadc1);        
//    }

//    return (uint16_t)(adc_sum / times);
//}

//// 8个通道的ADC值
//void adc_value_all(uint16_t *raw_data) 
//{
//    for (uint8_t i = 0; i < 8; i++) 
//    {

//        HAL_GPIO_WritePin(track_bit0_GPIO_Port, track_bit0_Pin, !(i & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(track_bit1_GPIO_Port, track_bit1_Pin, !(i & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//        HAL_GPIO_WritePin(track_bit2_GPIO_Port, track_bit2_Pin, !(i & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
//        
//        delay_us(10); 
//        
//        raw_data[i] = adc_value_one(40); 
//    }
//}

