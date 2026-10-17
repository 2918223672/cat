#ifndef __X_V2_H
#define __X_V2_H

#include "Myconfig.h"
#include "usart.h"

/**********************************************************
***	X_V2步进闭环控制例程
***	编写作者：ZHANGDATOU
***	技术支持：张大头闭环伺服
***	淘宝店铺：https://zhangdatou.taobao.com
***	CSDN博客：http s://blog.csdn.net/zhangdatou666
***	qq交流群：262438510
**********************************************************/

#define					ABS(x)							((x) > 0 ? (x) : -(x)) 

typedef enum {
	S_VBUS  = 5,	// 读取总线电压
	S_CBUS  = 6,	// 读取总线电流
	S_CPHA  = 7,	// 读取相电流
	S_ENCO  = 8,	// 读取编码器原始值
	S_CLKC  = 9,	// 读取实时脉冲数
	S_ENCL  = 10,	// 读取经过线性化校准后的编码器值
	S_CLKI  = 11,	// 读取输入脉冲数
	S_TPOS  = 12,	// 读取电机目标位置
	S_SPOS  = 13,	// 读取电机实时设定的目标位置
	S_VEL   = 14,	// 读取电机实时转速
	S_CPOS  = 15,	// 读取电机实时位置
	S_PERR  = 16,	// 读取电机位置误差
	S_VBAT  = 17,	// 读取多圈编码器电池电压（Y42）
	S_TEMP  = 18,	// 读取电机实时温度（X42S/Y42）
	S_FLAG  = 19,	// 读取电机状态标志位
	S_OFLAG = 20, // 读取回零状态标志位
	S_OAF   = 21,	// 读取电机状态标志位 + 回零状态标志位（X42S/Y42）
	S_PIN   = 22,	// 读取引脚状态（X42S/Y42）
	S_SYS   = 23,	// 读取系统状态信息
}SysParams_t;

#define		MMCL_LEN		512
extern __IO uint16_t MMCL_count, MMCL_cmd[MMCL_LEN];

/**
***********************************************************
***********************************************************
*** 
***
*** @brief	后缀带有（X42S/Y42）为X42S/Y42新增命令，X42不能用，其他通用
***
*** 
***********************************************************
***********************************************************
***/
/**********************************************************
*** 触发动作命令
**********************************************************/
// 触发编码器校准
void X_V2_Trig_Encoder_Cal(uint8_t addr);
// 重启电机（X42S/Y42）
void X_V2_Reset_Motor(uint8_t addr);
// 将当前位置清零
void X_V2_Reset_CurPos_To_Zero(uint8_t addr);
// 解除堵转保护
void X_V2_Reset_Clog_Pro(uint8_t addr);
// 恢复出厂设置
void X_V2_Restore_Motor(uint8_t addr);
/**********************************************************
*** 运动控制命令
**********************************************************/
// 多电机命令（X42S/Y42）
void X_V2_Multi_Motor_Cmd(uint8_t addr); // 参考下面的多电机命令说明使用，或多电机命令例程使用，或说明书的多电机命令章节说明，原理是一次性发送多条电机的命令出去
// 电机使能控制
void X_V2_En_Control(uint8_t addr, bool state, bool snF);
// 力矩模式
void X_V2_Torque_Control(uint8_t addr, uint8_t sign, uint16_t t_ramp, uint16_t torque, bool snF);
// 力矩模式限速控制（X42S/Y42）
void X_V2_Torque_LV_Control(uint8_t addr, uint8_t sign, uint16_t t_ramp, uint16_t torque, bool snF, float maxVel);
// 速度模式控制
void X_V2_Vel_Control(uint8_t addr, uint8_t dir, uint16_t acc, float vel, bool snF);
// 速度模式限电流控制（X42S/Y42）
void X_V2_Vel_LC_Control(uint8_t addr, uint8_t dir, uint16_t acc, float vel, bool snF, uint16_t maxCur);
// 直通限速位置模式控制
void X_V2_Bypass_Pos_LV_Control(uint8_t addr, uint8_t dir, float vel, float pos, uint8_t raf, bool snF);
// 直通限速位置模式限电流控制（X42S/Y42）
void X_V2_Bypass_Pos_LV_LC_Control(uint8_t addr, uint8_t dir, float vel, float pos, uint8_t raf, bool snF, uint16_t maxCur);
// 梯形曲线加减速位置模式控制
void X_V2_Traj_Pos_Control(uint8_t addr, uint8_t dir, uint16_t acc, uint16_t dec, float vel, float pos, uint8_t raf, bool snF);
// 梯形曲线加减速位置模式限电流控制（X42S/Y42）
void X_V2_Traj_Pos_LC_Control(uint8_t addr, uint8_t dir, uint16_t acc, uint16_t dec, float vel, float pos, uint8_t raf, bool snF, uint16_t maxCur);
// 设置快速梯形曲线位置模式的运动参数（X42S/Y42）
void X_V2_Set_QTrajPos_Params(uint8_t addr, uint16_t acc, uint16_t dec, float vel, uint8_t raf, bool snF, uint16_t maxCur);
// 快速梯形曲线位置模式控制（X42S/Y42）
void X_V2_QTrajPos_LC_Control(uint8_t addr, float pos);
// 让电机立即停止运动
void X_V2_Stop_Now(uint8_t addr, bool snF);
// 触发多机同步开始运动
void X_V2_Synchronous_motion(uint8_t addr);
/**********************************************************
*** 原点回零命令
**********************************************************/
// 设置单圈回零的零点位置
void X_V2_Origin_Set_O(uint8_t addr, bool svF);
// 触发回零
void X_V2_Origin_Trigger_Return(uint8_t addr, uint8_t o_mode, bool snF);
// 强制中断并退出回零
void X_V2_Origin_Interrupt(uint8_t addr);
// 读取回零参数
void X_V2_Origin_Read_Params(uint8_t addr);
// 修改回零参数
void X_V2_Origin_Modify_Params(uint8_t addr, bool svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, bool potF);
// 读取碰撞回零返回角度（X42S/Y42）
void X_V2_Origin_Read_SL_RP(uint8_t addr);
// 修改碰撞回零返回角度（X42S/Y42）
void X_V2_Origin_Modify_SL_RP(uint8_t addr, bool svF, uint16_t sl_rp);
/**********************************************************
*** 读取系统参数命令
**********************************************************/
// 定时返回信息命令（X42S/Y42）
void X_V2_Auto_Return_Sys_Params_Timed(uint8_t addr, SysParams_t s, uint16_t time_ms);
// 读取系统参数
void X_V2_Read_Sys_Params(uint8_t addr, SysParams_t s);
/**********************************************************
*** 读写驱动参数命令
**********************************************************/
// 修改电机ID地址
void X_V2_Modify_Motor_ID(uint8_t addr, bool svF, uint8_t id);
// 修改细分值
void X_V2_Modify_MicroStep(uint8_t addr, bool svF, uint8_t mstep);
// 修改掉电标志
void X_V2_Modify_PDFlag(uint8_t addr, bool pdf);
// 读取选项参数状态（X42S/Y42）
void X_V2_Read_Opt_Param_Sta(uint8_t addr);
// 修改电机类型（X42S/Y42）
void X_V2_Modify_Motor_Type(uint8_t addr, bool svF, bool mottype);
// 修改固件类型（X42S/Y42）
void X_V2_Modify_Firmware_Type(uint8_t addr, bool svF, bool fwtype);
// 修改开环/闭环控制模式（X42S/Y42）
void X_V2_Modify_Ctrl_Mode(uint8_t addr, bool svF, bool ctrl_mode);
// 修改电机运动正方向（X42S/Y42）
void X_V2_Modify_Motor_Dir(uint8_t addr, bool svF, bool dir);
// 修改锁定按键功能（X42S/Y42）
void X_V2_Modify_Lock_Btn(uint8_t addr, bool svF, bool lockbtn);
// 修改命令位置角度是否继续缩小10倍输入（X42S/Y42）
void X_V2_Modify_S_Vel(uint8_t addr, bool svF, bool s_vel);
// 修改开环模式工作电流
void X_V2_Modify_OM_ma(uint8_t addr, bool svF, uint16_t om_ma);
// 修改闭环模式最大电流
void X_V2_Modify_FOC_mA(uint8_t addr, bool svF, uint16_t foc_mA);
// 读取PID参数
void X_V2_Read_PID_Params(uint8_t addr);
// 修改PID参数
void X_V2_Modify_PID_Params(uint8_t addr, bool svF, uint32_t pTkp, uint32_t pBkp, uint32_t vkp, uint32_t vki);
// 读取DMX512协议参数（X42S/Y42）
void X_V2_Read_DMX512_Params(uint8_t addr);
// 修改DMX512协议参数（X42S/Y42）
void X_V2_Modify_DMX512_Params(uint8_t addr, bool svF, uint16_t tch, uint8_t nch, uint8_t mode, uint16_t vel, uint16_t acc, uint16_t vel_step, uint32_t pos_step);
// 读取位置到达窗口（X42S/Y42）
void X_V2_Read_Pos_Window(uint8_t addr);
// 修改位置到达窗口（X42S/Y42）
void X_V2_Modify_Pos_Window(uint8_t addr, bool svF, uint16_t prw);
// 读取过热过流保护检测阈值（X42S/Y42）
void X_V2_Read_Otocp(uint8_t addr);
// 修改过热过流保护检测阈值（X42S/Y42）
void X_V2_Modify_Otocp(uint8_t addr, bool svF, uint16_t otp, uint16_t ocp, uint16_t time_ms);
// 读取心跳保护功能时间（X42S/Y42）
void X_V2_Read_Heart_Protect(uint8_t addr);
// 修改心跳保护功能时间（X42S/Y42）
void X_V2_Modify_Heart_Protect(uint8_t addr, bool svF, uint32_t hp);
// 读取积分限幅/刚性系数（X42S/Y42）
void X_V2_Read_Integral_Limit(uint8_t addr);
// 修改积分限幅/刚性系数（X42S/Y42）
void X_V2_Modify_Integral_Limit(uint8_t addr, bool svF, uint32_t il);
/**********************************************************
*** 读取所有驱动参数命令
**********************************************************/
// 读取系统状态参数
void X_V2_Read_System_State_Params(uint8_t addr);
// 读取驱动配置参数
void X_V2_Read_Motor_Conf_Params(uint8_t addr);

/**
***********************************************************
***********************************************************
*** @brief	多电机命令说明
***
*** @brief	以下是把相应命令加载到X42S/Y42多电机命令上的函数（X42S/Y42）
*** @brief	命令跟上面一样，就是先发送出去，先把命令存到数组MMCL_cmd[]上
*** @brief	调用函数：void X_V2_Multi_Motor_Cmd(uint8_t addr); 后把MMCL_cmd[]数组中的所有命令发送出去
***
*** 
***********************************************************
***********************************************************
***/
/**********************************************************
*** 触发动作命令
**********************************************************/
// 触发编码器校准 - 加载到多电机指令上
void X_V2_MMCL_Trig_Encoder_Cal(uint8_t addr);
// 重启电机 - 加载到多电机指令上
void X_V2_MMCL_Reset_Motor(uint8_t addr);
// 将当前位置清零 - 加载到多电机指令上
void X_V2_MMCL_Reset_CurPos_To_Zero(uint8_t addr);
// 解除堵转保护 - 加载到多电机指令上
void X_V2_MMCL_Reset_Clog_Pro(uint8_t addr);
// 恢复出厂设置 - 加载到多电机指令上
void X_V2_MMCL_Restore_Motor(uint8_t addr);
/**********************************************************
*** 运动控制命令
**********************************************************/
// 电机使能控制 - 加载到多电机指令上
void X_V2_MMCL_En_Control(uint8_t addr, bool state, bool snF);
// 力矩模式控制 - 加载到多电机指令上
void X_V2_MMCL_Torque_Control(uint8_t addr, uint8_t sign, uint16_t t_ramp, uint16_t torque, bool snF);
// 力矩模式限速控制（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Torque_LV_Control(uint8_t addr, uint8_t sign, uint16_t t_ramp, uint16_t torque, bool snF, float maxVel);
// 速度模式控制 - 加载到多电机指令上
void X_V2_MMCL_Vel_Control(uint8_t addr, uint8_t dir, uint16_t acc, float vel, bool snF);
// 直通限速位置模式 - 加载到多电机指令上
void X_V2_MMCL_Bypass_Pos_LV_Control(uint8_t addr, uint8_t dir, float vel, float pos, uint8_t raf, bool snF);
// 直通限速位置模式限电流控制 - 加载到多电机指令上
void X_V2_MMCL_Bypass_Pos_LV_LC_Control(uint8_t addr, uint8_t dir, float vel, float pos, uint8_t raf, bool snF, uint16_t maxCur);
// 梯形曲线加减速位置模式控制 - 加载到多电机指令上
void X_V2_MMCL_Traj_Pos_Control(uint8_t addr, uint8_t dir, uint16_t acc, uint16_t dec, float vel, float pos, uint8_t raf, bool snF);
// 梯形曲线加减速位置模式限电流控制（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Traj_Pos_LC_Control(uint8_t addr, uint8_t dir, uint16_t acc, uint16_t dec, float vel, float pos, uint8_t raf, bool snF, uint16_t maxCur);
// 设置快速梯形曲线位置模式的运动参数（X42S/Y42）
void X_V2_MMCL_Set_QTrajPos_Params(uint8_t addr, uint16_t acc, uint16_t dec, float vel, uint8_t raf, bool snF, uint16_t maxCur);
// 快速梯形曲线位置模式控制（X42S/Y42）
void X_V2_MMCL_QTrajPos_LC_Control(uint8_t addr, float pos);
// 让电机立即停止运动 - 加载到多电机指令上
void X_V2_MMCL_Stop_Now(uint8_t addr, bool snF);
// 触发多机同步开始运动 - 加载到多电机指令上
void X_V2_MMCL_Synchronous_motion(uint8_t addr);
/**********************************************************
*** 原点回零命令
**********************************************************/
// 设置单圈回零的零点位置 - 加载到多电机指令上
void X_V2_MMCL_Origin_Set_O(uint8_t addr, bool svF);
// 触发回零 - 加载到多电机指令上
void X_V2_MMCL_Origin_Trigger_Return(uint8_t addr, uint8_t o_mode, bool snF);
// 强制中断并退出回零 - 加载到多电机指令上
void X_V2_MMCL_Origin_Interrupt(uint8_t addr);
// 修改回零参数 - 加载到多电机指令上
void X_V2_MMCL_Origin_Modify_Params(uint8_t addr, bool svF, uint8_t o_mode, uint8_t o_dir, uint16_t o_vel, uint32_t o_tm, uint16_t sl_vel, uint16_t sl_ma, uint16_t sl_ms, bool potF);
// 读取碰撞回零返回角度（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Origin_Read_SL_RP(uint8_t addr);
// 修改碰撞回零返回角度（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Origin_Modify_SL_RP(uint8_t addr, bool svF, uint16_t sl_rp);
/**********************************************************
*** 读取系统参数命令
**********************************************************/
// 定时返回信息命令（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Auto_Return_Sys_Params_Timed(uint8_t addr, SysParams_t s, uint16_t time_ms);
// 读取系统参数 - 加载到多电机指令上
void X_V2_MMCL_Read_Sys_Params(uint8_t addr, SysParams_t s);
/**********************************************************
*** 读写驱动参数命令
**********************************************************/
// 修改电机ID地址 - 加载到多电机指令上
void X_V2_MMCL_Modify_Motor_ID(uint8_t addr, bool svF, uint8_t id);
// 修改细分值 - 加载到多电机指令上
void X_V2_MMCL_Modify_MicroStep(uint8_t addr, bool svF, uint8_t mstep);
// 修改掉电标志 - 加载到多电机指令上
void X_V2_MMCL_Modify_PDFlag(uint8_t addr, bool pdf);
// 读取选项参数状态（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Read_Opt_Param_Sta(uint8_t addr);
// 修改电机类型（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Modify_Motor_Type(uint8_t addr, bool svF, bool mottype);
// 修改固件类型（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Modify_Firmware_Type(uint8_t addr, bool svF, bool fwtype);
// 修改开环/闭环控制模式（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Modify_Ctrl_Mode(uint8_t addr, bool svF, bool ctrl_mode);
// 修改电机运动正方向（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Modify_Motor_Dir(uint8_t addr, bool svF, bool dir);
// 修改锁定按键功能（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Modify_Lock_Btn(uint8_t addr, bool svF, bool lock);
// 修改命令位置角度是否继续缩小10倍输入（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Modify_S_Vel(uint8_t addr, bool svF, bool s_vel);
// 修改开环模式工作电流 - 加载到多电机指令上
void X_V2_MMCL_Modify_OM_mA(uint8_t addr, bool svF, uint16_t om_ma);
// 修改闭环模式最大电流 - 加载到多电机指令上
void X_V2_MMCL_Modify_FOC_mA(uint8_t addr, bool svF, uint16_t foc_mA);
// 读取PID参数 - 加载到多电机指令上
void X_V2_MMCL_Read_PID_Params(uint8_t addr);
// 修改PID参数 - 加载到多电机指令上
void X_V2_MMCL_Modify_PID_Params(uint8_t addr, bool svF, uint32_t pTkp, uint32_t pBkp, uint32_t vkp, uint32_t vki);
// 读取DMX512协议参数（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Read_DMX512_Params(uint8_t addr);
// 读取DMX512协议参数（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Modify_DMX512_Params(uint8_t addr, bool svF, uint16_t tch, uint8_t nch, uint8_t mode, uint16_t vel, uint16_t acc, uint16_t vel_step, uint32_t pos_step);
// 读取位置到达窗口（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Read_Pos_Window(uint8_t addr);
// 修改位置到达窗口（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Modify_Pos_Window(uint8_t addr, bool svF, uint16_t prw);
// 读取过热过流保护检测阈值（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Read_Otocp(uint8_t addr);
// 修改过热过流保护检测阈值（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Modify_Otocp(uint8_t addr, bool svF, uint16_t otp, uint16_t ocp, uint16_t time_ms);
// 读取心跳保护功能时间（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Read_Heart_Protect(uint8_t addr);
// 修改心跳保护功能时间（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Modify_Heart_Protect(uint8_t addr, bool svF, uint32_t hp);
// 读取积分限幅/刚性系数（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Read_Integral_Limit(uint8_t addr);
// 修改积分限幅/刚性系数（X42S/Y42） - 加载到多电机指令上
void X_V2_MMCL_Modify_Integral_Limit(uint8_t addr, bool svF, uint32_t il);
/**********************************************************
*** 读取所有驱动参数命令
**********************************************************/
// 读取系统状态参数 - 加载到多电机指令上
void X_V2_MMCL_Read_System_State_Params(uint8_t addr);
// 读取驱动配置参数 - 加载到多电机指令上
void X_V2_MMCL_Read_Motor_Conf_Params(uint8_t addr);

#endif
