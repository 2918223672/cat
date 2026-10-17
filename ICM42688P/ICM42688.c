/**
 * @file    ICM42688.c
 * @brief   ICM42688 驱动实现 (基于 TI 官方 MSPM0 SDK 例程)
 * @note    I2C 模式，MSPM0G3507，32MHz CPUCLK，所有 I2C 操作带超时
 */

#include "ICM42688.h"
#include "I2C_communication.h"
#include "ti_msp_dl_config.h"

static float accSensitivity  = 0.244f;   // mg/LSB (默认 ±2G)
static float gyroSensitivity = 32.8f;    // (dps/100)/LSB (默认 ±1000dps)

/* ---------- 延时 ---------- */
void delay_us(uint32_t __us)  { delay_cycles((CPUCLK_FREQ / 1000000) * __us); }
void delay_ms(uint32_t __ms)  { delay_cycles((CPUCLK_FREQ / 1000)    * __ms); }
#define ICM42688DelayMs(_nms)  delay_ms(_nms)

/* ---------- 单寄存器读 ---------- */
static int8_t icm42688_read_reg(uint8_t reg, uint8_t *pval)
{
    return I2C_ReadReg(ICM42688_ADDRESS, reg, pval, 1);
}

/* ---------- 多寄存器连续读 ---------- */
static int8_t icm42688_read_regs(uint8_t reg, uint8_t* buf, uint16_t len)
{
    return I2C_ReadReg(ICM42688_ADDRESS, reg, buf, len);
}

/* ---------- 单寄存器写 ---------- */
static int8_t icm42688_write_reg(uint8_t reg, uint8_t value)
{
    return I2C_WriteReg(ICM42688_ADDRESS, reg, &value, 1);
}

/* ---------- 加速度量程 → 灵敏度 ---------- */
float bsp_Icm42688GetAres(uint8_t Ascale)
{
    switch (Ascale) {
        case AFS_2G:  accSensitivity = 2000  / 32768.0f; break;
        case AFS_4G:  accSensitivity = 4000  / 32768.0f; break;
        case AFS_8G:  accSensitivity = 8000  / 32768.0f; break;
        case AFS_16G: accSensitivity = 16000 / 32768.0f; break;
    }
    return accSensitivity;
}

/* ---------- 陀螺仪量程 → 灵敏度 ---------- */
float bsp_Icm42688GetGres(uint8_t Gscale)
{
    switch (Gscale) {
        case GFS_15_125DPS: gyroSensitivity = 15.125f / 32768.0f; break;
        case GFS_31_25DPS:  gyroSensitivity = 31.25f  / 32768.0f; break;
        case GFS_62_5DPS:   gyroSensitivity = 62.5f   / 32768.0f; break;
        case GFS_125DPS:    gyroSensitivity = 125.0f  / 32768.0f; break;
        case GFS_250DPS:    gyroSensitivity = 250.0f  / 32768.0f; break;
        case GFS_500DPS:    gyroSensitivity = 500.0f  / 32768.0f; break;
        case GFS_1000DPS:   gyroSensitivity = 1000.0f / 32768.0f; break;
        case GFS_2000DPS:   gyroSensitivity = 2000.0f / 32768.0f; break;
    }
    return gyroSensitivity;
}

/* ---------- 寄存器初始化配置 ---------- */
int8_t bsp_Icm42688RegCfg(void)
{
    uint8_t reg_val = 0;

    /* 读取 WHO_AM_I */
    if (icm42688_read_reg(ICM42688_WHO_AM_I, &reg_val) != 0) return -1;
    if (reg_val != ICM42688_ID) return -1;

    /* 软复位 */
    icm42688_write_reg(ICM42688_REG_BANK_SEL, 0);
    icm42688_write_reg(ICM42688_REG_BANK_SEL, 0x01);
    ICM42688DelayMs(100);

    /* 加速度计: ±4G, 100Hz */
    bsp_Icm42688GetAres(AFS_4G);
    if (icm42688_write_reg(ICM42688_REG_BANK_SEL, 0x00) != 0) return -1;
    reg_val = (AFS_4G << 5) | AODR_100Hz;
    if (icm42688_write_reg(ICM42688_ACCEL_CONFIG0, reg_val) != 0) return -1;

    /* 陀螺仪: ±1000dps, 100Hz */
    bsp_Icm42688GetGres(GFS_1000DPS);
    if (icm42688_write_reg(ICM42688_REG_BANK_SEL, 0x00) != 0) return -1;
    reg_val = (GFS_1000DPS << 5) | GODR_100Hz;
    if (icm42688_write_reg(ICM42688_GYRO_CONFIG0, reg_val) != 0) return -1;

    /* 电源: 温度使能, 陀螺仪+加速度计→低噪声模式 */
    if (icm42688_write_reg(ICM42688_REG_BANK_SEL, 0x00) != 0) return -1;
    if (icm42688_read_reg(ICM42688_PWR_MGMT0, &reg_val) != 0) return -1;
    reg_val &= ~(1 << 5);          // TEMP_EN = 1
    reg_val |= (3 << 2);           // GYRO_MODE = 3 (低噪声)
    reg_val |= 3;                  // ACCEL_MODE = 3 (低噪声)
    if (icm42688_write_reg(ICM42688_PWR_MGMT0, reg_val) != 0) return -1;
    ICM42688DelayMs(1);

    return 0;
}

/* ---------- 初始化 ---------- */
int8_t bsp_Icm42688Init(void)
{
    return bsp_Icm42688RegCfg();
}

/* ---------- 读温度 ---------- */
int8_t bsp_IcmGetTemperature(int16_t* pTemp)
{
    uint8_t buffer[2] = {0};
    if (icm42688_read_regs(ICM42688_TEMP_DATA1, buffer, 2) != 0) return -1;
    *pTemp = (int16_t)(((int16_t)((buffer[0] << 8) | buffer[1])) / 132.48 + 25);
    return 0;
}

/* ---------- 读加速度 (raw → mg) ---------- */
int8_t bsp_IcmGetAccelerometer(icm42688RawData_t* accData)
{
    uint8_t buffer[6] = {0};
    if (icm42688_read_regs(ICM42688_ACCEL_DATA_X1, buffer, 6) != 0) return -1;

    accData->x = ((uint16_t)buffer[0] << 8) | buffer[1];
    accData->y = ((uint16_t)buffer[2] << 8) | buffer[3];
    accData->z = ((uint16_t)buffer[4] << 8) | buffer[5];

    accData->x = (int16_t)(accData->x * accSensitivity);
    accData->y = (int16_t)(accData->y * accSensitivity);
    accData->z = (int16_t)(accData->z * accSensitivity);
    return 0;
}

/* ---------- 读陀螺仪 (raw → 0.1dps) ---------- */
int8_t bsp_IcmGetGyroscope(icm42688RawData_t* GyroData)
{
    uint8_t buffer[6] = {0};
    if (icm42688_read_regs(ICM42688_GYRO_DATA_X1, buffer, 6) != 0) return -1;

    GyroData->x = ((uint16_t)buffer[0] << 8) | buffer[1];
    GyroData->y = ((uint16_t)buffer[2] << 8) | buffer[3];
    GyroData->z = ((uint16_t)buffer[4] << 8) | buffer[5];

    GyroData->x = (int16_t)(GyroData->x * gyroSensitivity);
    GyroData->y = (int16_t)(GyroData->y * gyroSensitivity);
    GyroData->z = (int16_t)(GyroData->z * gyroSensitivity);
    return 0;
}

/* ---------- 一次读12字节获取6轴物理量 ---------- */
int8_t bsp_IcmGetRawData(icm42688RealData_t* accData, icm42688RealData_t* GyroData)
{
    uint8_t       buffer[12] = {0};
    icm42688RawData_t accRaw;
    icm42688RawData_t gyroRaw;

    if (icm42688_read_regs(ICM42688_ACCEL_DATA_X1, buffer, 12) != 0) return -1;

    accRaw.x  = ((uint16_t)buffer[0]  << 8) | buffer[1];
    accRaw.y  = ((uint16_t)buffer[2]  << 8) | buffer[3];
    accRaw.z  = ((uint16_t)buffer[4]  << 8) | buffer[5];
    gyroRaw.x = ((uint16_t)buffer[6]  << 8) | buffer[7];
    gyroRaw.y = ((uint16_t)buffer[8]  << 8) | buffer[9];
    gyroRaw.z = ((uint16_t)buffer[10] << 8) | buffer[11];

    accData->x = (float)(accRaw.x * accSensitivity);
    accData->y = (float)(accRaw.y * accSensitivity);
    accData->z = (float)(accRaw.z * accSensitivity);

    GyroData->x = (float)(gyroRaw.x * gyroSensitivity);
    GyroData->y = (float)(gyroRaw.y * gyroSensitivity);
    GyroData->z = (float)(gyroRaw.z * gyroSensitivity);

    return 0;
}
