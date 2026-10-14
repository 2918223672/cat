# Schedular_app 裸机调度器

本目录保存原工程的裸机轮询调度器。文件名沿用原项目拼写 `Schedular_app`，函数名使用 `Scheduler_*`。

## 文件

- `Schedular_app.c`：系统级初始化、任务表和周期调度逻辑。
- `Schedular_app.h`：调度周期、任务名称映射、超时参数和公开接口。

## 现有任务

| 任务宏 | 原函数 | 默认周期 | 功能 |
|---|---|---:|---|
| `SCHED_KEY_TASK` | `key_proc` | 10 ms | 按键扫描和模式切换 |
| `SCHED_RUNNING_TASK` | `running_proc` | 10 ms | 灰度循迹、速度 PID 和电机输出 |
| `SCHED_DISPLAY_TASK` | `seyes_proc` | 50 ms | LCD 菜单和状态刷新 |

周期和函数名称都集中定义在 `Schedular_app.h`：

```c
#define SCHED_KEY_PERIOD_MS             10U
#define SCHED_RUNNING_PERIOD_MS         10U
#define SCHED_DISPLAY_PERIOD_MS         50U

#define SCHED_KEY_TASK                  key_proc
#define SCHED_RUNNING_TASK              running_proc
#define SCHED_DISPLAY_TASK              seyes_proc
```

如果应用层函数改名，只需要修改这些映射宏。

## 系统初始化内容

`System_Init()` 当前依次执行：

1. `Encoder_Init()`：双编码器启动。
2. `motor_init()`：双路 PWM 电机启动并停止输出。
3. `PID_System_Init()`：左右轮 PID 初始化。
4. `LCD_Init()` 和开机红屏显示。
5. `seyes_proc()`：绘制初始界面。
6. `Ping()`：检测灰度传感器，默认最多等待 3 秒。
7. `Task_init()`：加载灰度传感器黑白校准参数。

## 外部依赖

该模块是当前小车项目的应用调度层，不是完全独立的通用调度器。编译时需要工程提供：

- `Encoder_Init()`、`motor_init()`、`PID_System_Init()`。
- `LCD_Init()`、`LCD_Fill()`、`LCD_W`、`LCD_H`、颜色宏。
- `Ping()`、`Task_init()`、`my_track_sensor`、`env_white`、`env_black`。
- `key_proc()`、`running_proc()`、`seyes_proc()`。
- HAL 的 `HAL_GetTick()` 和 `HAL_Delay()`。

这些声明在原工程中由 `Myconfig.h` 聚合提供。

## 使用方法

在所有 CubeMX 外设初始化完成后调用一次：

```c
Scheduler_Init();
```

随后在 `while (1)` 中持续调用：

```c
while (1)
{
    Scheduler_Run();
}
```

## 调度策略

- `last_run += rate_ms` 使用理想时间锚定，减少任务执行耗时造成的长期漂移。
- 当任务落后超过 `SCHED_MAX_LAG_PERIODS` 个周期时，会把时间基准重置为当前时间，防止连续追赶任务。
- 所有任务都在主循环中串行执行，任务函数不能长时间阻塞。
- 新增任务时，在 `scheduler_task[]` 中添加 `{函数, 周期, 0}`，并把周期宏放入 `Schedular_app.h`。
