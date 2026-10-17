#include "key_app.h"

uint8_t key_val = 0;  // 当前按键状态
uint8_t key_old = 0;  // 前一按键状态
uint8_t key_down = 0; // 按下的按键
uint8_t key_up = 0;   // 释放的按键

//uint8_t sys_mode = 0; // 0-wu，1-black，2-white，3-topic1，4-topic2，5-topic3，6-topic4, 7-topic5
//uint8_t run_mode = 0;
uint8_t sys_mode = 0; // 0-wu，1-topic1，2-topic2，3-topic3，4-topic4, 5-topic5
uint8_t run_mode = 0;

uint8_t key_read(void)
{
  uint8_t temp = 0;

  if (HAL_GPIO_ReadPin(key_nev_white_GPIO_Port, key_nev_white_Pin) == GPIO_PIN_RESET)
    temp |= 1;  // 用于功能切换
  if (HAL_GPIO_ReadPin(key_nev_black_GPIO_Port, key_nev_black_Pin) == GPIO_PIN_RESET)
    temp |= 2;  // 用于确认功能

  return temp;
}

void key_proc(void)
{
  key_val = key_read();
  key_down = key_val & (key_old ^ key_val);
  key_up = ~key_val & (key_old ^ key_val);
  key_old = key_val;

  if(key_down & 0x01)  // 用于功能切换
  {
    if (run_mode == 0)
      {
        sys_mode = (sys_mode + 1) % 6;
        race_cnt = 0;  // 切题时清零时间
      }
  }

  if(key_down & 0x02)  // 用于确认功能
  {
    if (run_mode == 0)
      {
        HAL_Delay(500);
        run_mode = sys_mode;
        race_cnt = 0;  // 启动时清零时间
      }
      else
      {
        // 运行中 → 退出回空闲
        run_mode = 0;
        sys_mode = 0;
        race_cnt = 0;  // 退出时清零时间
      }
    }
}
