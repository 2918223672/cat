#include "track_algo.h"

/******************************************************************************
   * 循迹传感器：感为智能 无MCU 灰度模块（CD4051 + STM32 ADC）
   *
   * 引脚定义：
   *   PA2  — ADC1_IN2     模拟输出 → ADC 采样
   *   PA8  — track_bit0    通道选择 A
   *   PC9  — track_bit1    通道选择 B
   *   PC8  — track_bit2    通道选择 C
*******************************************************************************
   * 循迹传感器：感为智能 带MCU 灰度模块（I2C 数字通信）
   *
   * 引脚定义：
   *   PA2  — SCL          软件 I2C 时钟线（开漏输出）
   *   PA8  — SDA          软件 I2C 数据线（开漏输出）
   *   I2C 地址：0x4C
******************************************************************************/

Track_Ctrl_t track_ctrl = {
    1.2f,   // track_Kp  (降低：减少过调振荡)
    0.5f,   // track_Kd  (降低：Kd≈2×Kp)
    5.0f,   // gyro_Kp
    2.0f,   // gyro_Kd

    20.0f,   // base_speed
    0.0f,    // turn_speed

    0        // last_error
};

Track_State_t my_track_sensor;
uint16_t env_white[8] = {94, 175, 185, 168, 120, 198, 175, 134};
uint16_t env_black[8] = {53,  84,  82,  64,  66,  75,  78,  50};

// 二值化
void binaryzation(uint16_t *adc_value,uint16_t *Gray_white,uint16_t *Gray_black, uint8_t *Digital)
{
    // ★ 先假定全白：只有确认低于黑阈值的位才清零
    //    避免迟滞导致全白区域 Digtal ≠ 0xFF，误走循迹分支
    *Digital = 0xFF;

    for (int i = 0; i < 8; i++) {
        if (adc_value[i] < Gray_black[i]) {
            *Digital &= ~(1 << i);  // 确认是黑色才清零
        }
    }
}

// 归一化
void unification(uint16_t *adc_value,double *Normal_factor,uint16_t *Calibrated_black,uint16_t *result,double bits)
{
    for (int i = 0; i < 8; i++) {
        uint16_t n ;
        // 计算归一化值（减去黑电平后缩放）
        if(adc_value[i]<Calibrated_black[i]) n=0;  // 低于黑电平归零
        else n = (adc_value[i] - Calibrated_black[i]) * Normal_factor[i];
        //   当前ADC值 - 黑色基准值                  × 4096/(白-黑)

        // 限幅处理
        if (n > bits) {
            n = bits;
        }
        result[i]=n;
    }
}

// 初始化的初始化，只需一次, 用于更新阈值
void Task_init_Frist(Track_State_t*sensor)
{
    // 清零所有校准数据和状态
    memset(sensor->Calibrated_black,0,16);
    memset(sensor->Calibrated_white,0,16);
    memset(sensor->Normal_value,0,16);
    memset(sensor->Analog_value,0,16);

    // 初始化归一化系数
    for(int i = 0; i < 8; i++)
    {
        sensor->Normal_factor[i]=0.0;
    }

    // 初始化状态变量
    sensor->Digtal=0;
    sensor->Tick=0;
    sensor->ok=0;  // 标记未完成校准
}

// 初始化，每次调用
void Task_init(Track_State_t *sensor, uint16_t *white_data, uint16_t *black_data)
{
    Task_init_Frist(sensor);

    sensor->bits = 255.0;   // I2C 8位传感器最大值

    double Normal_Diff[8];
    uint16_t temp;

    for (int i = 0; i < 8; i++)
    {
        // 确保白值 > 黑值（防呆反接）
        if(black_data[i] >= white_data[i])
        {
            temp = white_data[i];
            white_data[i] = black_data[i];
            black_data[i] = temp;
        }

        // 计算迟滞灰度阈值
        sensor->Gray_white[i] = (white_data[i] * 2 + black_data[i]) / 3;
        sensor->Gray_black[i] = (white_data[i] + black_data[i] * 2) / 3;

        // 保存基准校准数据
        sensor->Calibrated_black[i] = black_data[i];
        sensor->Calibrated_white[i] = white_data[i];

        // 处理无效校准数据（防除零死机）
        if ((white_data[i] == 0 && black_data[i] == 0) ||
            (white_data[i] == black_data[i]))
        {
            sensor->Normal_factor[i] = 0.0;  // 坏点的通道直接废弃
            continue;
        }

        // 计算归一化放大系数
        Normal_Diff[i] = (double)white_data[i] - (double)black_data[i];
        sensor->Normal_factor[i] = sensor->bits / Normal_Diff[i];
    }

    sensor->ok = 1;

}

//// 数据处理任务 无MCU 灰度模块
//void track_task(Track_State_t*sensor)
//{
//    adc_value_all(sensor->Analog_value);  // 采集数据
//    binaryzation(sensor->Analog_value, sensor->Gray_white,sensor->Gray_black, &sensor->Digtal);// 二值化处理
//    unification(sensor->Analog_value,  sensor->Normal_factor,sensor->Calibrated_black,sensor->Normal_value,sensor->bits);// 归一化处理
//}

// 数据处理任务 带MCU 灰度模块
void track_task(Track_State_t*sensor)
{
    unsigned char buf[8];

    IIC_Get_Anolog(buf, 8);                     // 读模拟值（0~255）
    for (int i = 0; i < 8; i++) sensor->Analog_value[i] = buf[i];

    sensor->Digtal = IIC_Get_Digtal();          // 硬件二值化结果

    IIC_Anolog_Normalize(0xFF);                 // 开启归一化
    HAL_Delay(5);                               // 等传感器刷新
    IIC_Get_Anolog(buf, 8);                     // 读归一化值（0~255）
    for (int i = 0; i < 8; i++) sensor->Normal_value[i] = buf[i];
    IIC_Anolog_Normalize(0x00);                 // 关闭归一化
}

// I2C 版传感器数据采集（硬件I2C，MCU端软件二值化+归一化，零阻塞）
void track_task_i2c(Track_State_t *sensor)
{
    unsigned char i2c_data[8];
    if (!IIC_Get_Anolog(i2c_data, 8)) return;  // I2C 读取失败则跳过本次

    for (int i = 0; i < 8; i++) {
        sensor->Analog_value[i] = (uint16_t)i2c_data[i];
    }

    binaryzation(sensor->Analog_value, sensor->Gray_white, sensor->Gray_black, &sensor->Digtal);
    unification(sensor->Analog_value, sensor->Normal_factor, sensor->Calibrated_black,
                sensor->Normal_value, sensor->bits);
}

// 获取传感器数据，用于矫正姿态
uint16_t Get_track_Digtal(Track_State_t*sensor)
{
    return sensor->Digtal;  // 返回8位数字状态（每位对应一个传感器）
}

// 获取归一化数据，用于路径规划和控制算法
uint16_t Get_track_Normal_value(Track_State_t*sensor,uint16_t* result)
{
    if(!sensor->ok)return 0;
    else
    {
        memcpy(result,sensor->Normal_value, 8 * sizeof(uint16_t));  // 拷贝归一化数据
        return 1;
    }
}

// 获取原始ADC数据，用于调试和校准
uint16_t Get_track_Ok(Track_State_t*sensor,uint16_t *result)
{
    memcpy(result,sensor->Analog_value, 8 * sizeof(uint16_t));
    if(!sensor->ok)return 0;
    else return 1;
}

// 获取偏转角度
int32_t track_steering(uint16_t Normal[8],uint8_t field)  // 1-白底 ，0-黑底
{
    // 定义权值数组，从左到右对应-7,-5,-3,-1,1,3,5,7
    const short weights[8] = {-7, -5, -3, -1, 1, 3, 5, 7};

    int32_t weighted_sum = 0;            // 加权和
    int32_t original_sum = 0;            // 原始数据和
    static int last_value = 0;           // 上一次的值

    // 计算加权和和原始数据和
    for (int i = 0; i < 8; i++) {
//			weighted_sum += 1024*(field?(4096-Normal[i]) * weights[i]:Normal[i] * weights[i]);
//			original_sum += field?(4096-Normal[i]):Normal[i];     // 无mcu
            weighted_sum += 1024*(field?(255-Normal[i]) * weights[i]:Normal[i] * weights[i]);
			original_sum += field?(255-Normal[i]):Normal[i];  // 有mcu
    }

    if (original_sum != 0) {
        last_value = weighted_sum / original_sum;
        return last_value;
    } else {
        return last_value;
    }
}
