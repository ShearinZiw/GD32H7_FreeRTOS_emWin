# GDemWin 详解 —— 结合源码理解 emWin 移植

## 目录

1. [GDemWin 是什么](#1-gdemwin-是什么)
2. [整体架构：从应用到硬件](#2-整体架构从应用到硬件)
3. [逐文件详解](#3-逐文件详解)
4. [启动流程：从开机到第一帧画面](#4-启动流程从开机到第一帧画面)
5. [绘制流程：GUI_DispString 到底做了什么](#5-绘制流程gui_dispstring-到底做了什么)
6. [触摸流程：手指到 UI 响应](#6-触摸流程手指到-ui-响应)
7. [内存布局详解](#7-内存布局详解)
8. [TLI 硬件详解](#8-tli-硬件详解)
9. [关键设计决策 FAQ](#9-关键设计决策-faq)

---

## 1. GDemWin 是什么

### 1.1 名字的含义

```
GDemWin = GD (GigaDevice) + emWin (SEGGER)
```

- **emWin** 是德国 SEGGER 公司开发的嵌入式图形库（GUI Library），专为资源受限的单片机设计
- **GDemWin** 是 GD（兆易创新/GigaDevice）从 SEGGER 购买的授权版本，专门给 GD32 系列 MCU 使用
- 功能和 API 与标准 emWin 完全相同，只是库文件用 GD32 的工具链编译

类比：就像 STM32 有 **STemWin**，NXP 有 **NXPemWin**，GD32 的就是 **GDemWin**。内核都一样。

### 1.2 版本信息

```
版本:    emWin V6.52
授权号:  GUI-00918
平台:    Cortex-M (ARM Cortex-M7)
库文件:  GDemWin_CM7_OS_DP.lib
         CM7 = Cortex-M7 指令集
         OS  = 支持操作系统 (FreeRTOS)
         DP  = Double Precision (双精度浮点)
```

### 1.3 库文件选型

`GD32H7xx_GDemWin652/Lib/` 目录下有多个库文件：

| 库文件名 | 编译器 | 是否含OS | 用途 |
|----------|--------|----------|------|
| `GDemWin_CM7_DP.lib` | Keil (ARMCC) | 无OS | 裸机 / 单任务 |
| `GDemWin_CM7_OS_DP.lib` | Keil (ARMCC) | 有OS | **FreeRTOS 多任务（我们用这个）** |
| `GDemWin_CM7_DP_GNU.a` | GCC | 无OS | 裸机 |
| `GDemWin_CM7_OS_DP_GNU.a` | GCC | 有OS | FreeRTOS |
| `GDemWin_CM7_DP_IAR832.a` | IAR | 无OS | 裸机 |
| `GDemWin_CM7_OS_DP_IAR832.a` | IAR | 有OS | FreeRTOS |

我们用的是 **`GDemWin_CM7_OS_DP.lib`**：
- 用 Keil (ARM Compiler 6) 编译 → `.lib` 格式
- 包含 RTOS 支持，内部会调用 `GUI_X_Lock/Unlock` 做多任务互斥
- 双精度浮点 → 与 GD32H759IM 的 DFPU 匹配

### 1.4 emWin 是什么级别的库？

emWin 不是一个简单的"画点画线库"，它是完整的 **GUI 框架**：

```
层级                     emWin 提供的功能
─────────────────────────────────────────────────
Widget 层                按钮、滑块、编辑框、列表框、窗口……
Window Manager 层        窗口管理、焦点、消息传递
2D 绘图层                画线、矩形、圆、多边形、位图、字体
Renderer 层              GUIDRV_Lin (线性帧缓冲驱动)
Color Conversion 层      GUICC_M565 (RGB565 颜色转换)
OS 抽象层                GUI_X_FreeRTOS (与操作系统对接)
─────────────────────────────────────────────────
```

---

## 2. 整体架构：从应用到硬件

```
┌──────────────────────────────────────────────────────┐
│                    main.c                            │
│  GUI_Task() {                                        │
│    GUI_Init();     ← 初始化整个 GUI 系统              │
│    GUI_DispStringAt("Hello", 100, 200);  ← 画字符串  │
│    GUI_Delay(50);  ← 处理事件 + 延时                 │
│  }                                                   │
│                                                      │
│  Touch_Task() {                                      │
│    bsp_gt1151qm_read_touch(&x, &y);  ← 读触摸坐标    │
│    GUI_PID_StoreState(&state);       ← 喂给 emWin    │
│  }                                                   │
└──────────┬───────────────────────────────────────────┘
           │ 调用 emWin API
           ▼
┌──────────────────────────────────────────────────────┐
│     GDemWin_CM7_OS_DP.lib  (SEGGER 预编译库)         │
│                                                      │
│  提供: GUI_DispString, GUI_FillRect, GUI_DrawBitmap  │
│        BUTTON_Create, WM_Exec, GUI_Init ...          │
│                                                      │
│  内部调用 ↓                                          │
│  - LCD_X_Config()        (由 LCDConf.c 提供)         │
│  - LCD_X_DisplayDriver() (由 LCDConf.c 提供)         │
│  - GUI_X_Delay/GetTime() (由 GUI_X_FreeRTOS.c 提供)  │
│                                                      │
│  读取 GUIConf.h 决定:                                │
│  - 是否多任务  (GUI_OS)                              │
│  - 是否触摸    (GUI_SUPPORT_TOUCH)                   │
│  - 内存池大小  (GUI_ALLOC_SIZE)                      │
└──────────┬──────────────────────────────────────────┘
           │ 通过 LCD_X_DisplayDriver 下发硬件命令
           ▼
┌──────────────────────────────────────────────────────┐
│              LCDConf.c  (硬件适配层)                  │
│                                                      │
│  LCD_X_Config() {                                    │
│    GUI_DEVICE_CreateAndLink(                         │
│      GUIDRV_LIN_16,  ← 驱动类型: 16位线性帧缓冲      │
│      GUICC_M565,     ← 颜色转换: RGB565              │
│      0, 0);                                          │
│    LCD_SetVRAMAddrEx(0, framebuffer_addr);           │
│  }                                                   │
│                                                      │
│  LCD_X_DisplayDriver(cmd) {                          │
│    case LCD_X_INITCONTROLLER: → 硬件初始化           │
│    case LCD_X_ON:             → tli_enable()         │
│    case LCD_X_SHOWBUFFER:     → 切换显示缓冲         │
│  }                                                   │
└──────────┬──────────────────────────────────────────┘
           │ 调用 GDemWin 内置的 GUIDRV_Lin_16 驱动
           ▼
┌──────────────────────────────────────────────────────┐
│      GUIDRV_Lin_16 (emWin 内置驱动, 在 .lib 中)      │
│                                                      │
│  知道如何把 "在 (100,200) 画一个'A'" 这个操作        │
│  转换成 "写帧缓冲偏移量 y*800*2 + x*2 处写入像素值"  │
│                                                      │
│  最终写的是:  s_frame_buffer[y*800 + x] = color;     │
└──────────┬──────────────────────────────────────────┘
           │ 帧缓冲数据在 SRAM 中
           ▼
┌──────────────────────────────────────────────────────┐
│             bsp_lcd.c  (硬件驱动层)                   │
│                                                      │
│  bsp_lcd_init() {                                    │
│    1. _TLI_Clock_Config();  ← 配置 PLL2 产生像素时钟 │
│    2. _TLI_GPIO_Config();   ← 配置 RGB 引脚为 AF14   │
│    3. _TLI_Init();          ← 配置 TLI 外设时序      │
│    4. memset(fb, 0, size);  ← 清屏为黑色             │
│    5. bsp_lcd_backlight_on(); ← 开背光               │
│  }                                                   │
└──────────┬──────────────────────────────────────────┘
           │ TLI 外设自动工作
           ▼
┌──────────────────────────────────────────────────────┐
│              GD32H7xx TLI 硬件外设                    │
│                                                      │
│  TLI = TFT-LCD Interface (相当于 STM32 的 LTDC)      │
│                                                      │
│  自动完成:                                           │
│  1. 从 s_frame_buffer 读像素数据 (通过 DMA)          │
│  2. 按 RGB565 格式拆解为 R/G/B 分量                  │
│  3. 按时序产生 HSYNC / VSYNC / DE / PCLK 信号        │
│  4. 在 R[4:0]/G[5:0]/B[4:0] 引脚上输出并行 RGB 数据 │
│                                                      │
│  TLI 是纯硬件，配置好后自动运行，不需 CPU 干预       │
└──────────┬──────────────────────────────────────────┘
           │ 16根数据线 + 4根控制线
           ▼
┌──────────────────────────────────────────────────────┐
│            5寸 LCD 面板 (IL6122/IL5960)               │
│                                                      │
│  接收 RGB565 数据 + HSYNC/VSYNC/DE/PCLK              │
│  内部 Gate Driver + Source Driver 驱动液晶像素       │
│  800 × 480 = 384,000 个像素点                        │
└──────────────────────────────────────────────────────┘
```

---

## 3. 逐文件详解

### 3.1 `GUIConf.h` — emWin 核心配置 (src/GUIConf.h)

这是 emWin 的"总开关"文件。emWin 内部代码通过 `#ifdef` 读取这些宏来决定编译哪些功能。

```c
#define GUI_NUM_LAYERS            1   // 显示层数 = 1（单层，不用多层叠加）
#define GUI_OS                    1   // 启用多任务保护（配合 FreeRTOS）
#define GUI_SUPPORT_TOUCH         1   // 启用触摸输入
#define GUI_WINSUPPORT            1   // 启用窗口管理器
#define GUI_DEFAULT_FONT          &GUI_Font6x8  // 默认字体
#define GUI_ALLOC_SIZE            65536  // emWin 动态内存池 64KB
#define GUI_USE_ARGB              0   // 颜色格式: 0=ABGR(标准), 1=ARGB
#define GUI_MEMDEV_SUPPORT_CUSTOMDRAW  0  // 暂不用 DMA2D 硬件加速
```

**每个宏的含义：**

| 宏 | 设为0的影响 | 设为1的影响 |
|----|------------|-----------|
| `GUI_OS` | 单任务模式，不加锁 | 多任务模式，emWin 内部调用 `GUI_X_Lock/Unlock` 保护临界区 |
| `GUI_SUPPORT_TOUCH` | 触摸相关代码不编译 | `GUI_PID_StoreState()` 可用 |
| `GUI_WINSUPPORT` | 只能用基本画图函数 | 可以使用窗口、按钮、编辑框等控件 |
| `GUI_NUM_LAYERS` | (最少1) | 多层硬件叠加，用于视频播放+UI 叠加等场景 |
| `GUI_ALLOC_SIZE` | — | emWin 内部 `malloc` 用的总池子，控件越多需要越大 |
| `GUI_USE_ARGB` | ABGR格式(0xAABBGGRR) | ARGB格式(0xAARRGGBB) |

**注意：** `GUIConf.h` 被 `GUI.h` 内部 include，所以不需要你在应用代码中手动 include。你只需要把它放在 include path 能搜到的位置。

### 3.2 `LCDConf.c` — 显示器适配层 (src/LCDConf.c)

这是移植中最关键的适配文件。它提供了两个 emWin 必须调用的函数。

#### 3.2.1 `LCD_X_Config()` — 创建显示驱动

```c
void LCD_X_Config(void)
{
    // 第1步: 创建显示驱动设备
    GUI_DEVICE_CreateAndLink(DISPLAY_DRIVER, COLOR_CONVERSION, 0, 0);
    //                       ↑                ↑                ↑  ↑
    //                       GUIDRV_LIN_16    GUICC_M565        层 驱动号
    //                       (16位线性帧缓冲)  (RGB565颜色转换)

    // 第2步: 告诉 emWin 屏幕物理尺寸
    LCD_SetSizeEx(0, 800, 480);     // 物理分辨率
    LCD_SetVSizeEx(0, 800, 480);    // 虚拟分辨率（可大于物理分辨率做滚动）

    // 第3步: 告诉 emWin 帧缓冲在哪
    LCD_SetVRAMAddrEx(0, (void *)0x24000000);
}
```

**`GUI_DEVICE_CreateAndLink` 做了什么？**

emWin 内部维护一个"设备链表"。每个设备包含：
- 一个 **Driver**（GUIDRV_Lin_16）—— 知道怎么往帧缓冲写像素
- 一个 **Color Conversion**（GUICC_M565）—— 知道怎么把 GUI_COLOR 转成 RGB565
- 一个 **Display Driver callback**（LCD_X_DisplayDriver）—— 硬件操作回调

调用后，emWin 就知道"我有一个 800x480 的显示器，帧缓冲在 0x24000000，用 RGB565 格式"。

#### 3.2.2 `LCD_X_DisplayDriver()` — 硬件操作回调

emWin 在需要操作硬件时，通过这个函数下发命令：

```c
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData)
{
    switch (Cmd) {

    case LCD_X_INITCONTROLLER:   // emWin 说: "初始化显示控制器"
        _InitController();       // 硬件已在 bsp_lcd_init() 中初始化好了
        break;

    case LCD_X_SETVRAMADDR:      // emWin 说: "帧缓冲地址换了"
        TLI_LXFBADDR(LAYER0) = new_addr;  // 直接写 TLI 寄存器更新地址
        tli_reload_config(TLI_FRAME_BLANK_RELOAD_EN);
        break;

    case LCD_X_SHOWBUFFER:       // emWin 说: "新画面画好了，显示它"
        // 多缓冲时切换显示缓冲，单缓冲时不需要做什么
        break;

    case LCD_X_ON:               // emWin 说: "开显示"
        tli_enable();
        break;

    case LCD_X_OFF:              // emWin 说: "关显示"
        tli_disable();
        break;
    }
}
```

**关键点：** `LCD_X_INITCONTROLLER` 是 emWin 的 `GUI_Init()` 内部调用的。我们的 TLI 硬件初始化在它之前（在 `AppTaskCreate` 中调用了 `bsp_lcd_init()`），所以这里不需要再做硬件初始化。

**为什么 `TLI_LXFBADDR(LAYER0) = addr` 可以直接写寄存器？**

`TLI_LXFBADDR` 是一个宏，定义在 `gd32h7xx_tli.h`：
```c
#define TLI_LXFBADDR(layerx)  REG32((layerx) + 0x00000028U)
//                                     ↑               ↑
//                              TLI_BASE+0x84      帧缓冲地址寄存器偏移
//                              = 0x40001084
```

展开后就是直接写 TLI 外设的帧缓冲地址寄存器，让硬件知道从哪里读像素数据。

### 3.3 `GUI_X_FreeRTOS.c` — OS 抽象层 (src/GUI_X_FreeRTOS.c)

emWin 本身不知道你在用什么操作系统。这个文件告诉它：

```c
// emWin 问: "现在几点（毫秒）？"
int GUI_X_GetTime(void) {
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

// emWin 说: "我要睡 50 毫秒"
void GUI_X_Delay(int Period) {
    vTaskDelay(pdMS_TO_TICKS(Period));
}

// emWin 说: "我要进入临界区，别人别碰 GUI"
void GUI_X_Lock(void) {
    xSemaphoreTake(_Semaphore, 0);   // 拿走信号量（不走等待）
}

// emWin 说: "我退出临界区了"
void GUI_X_Unlock(void) {
    xSemaphoreGive(_Semaphore);      // 归还信号量
}
```

**锁机制详解：**

当 `GUI_OS = 1` 时，emWin 内部每次操作前自动：
```
GUI_X_Lock()   → 执行 GUI 操作 → GUI_X_Unlock()
```

信号量初始值 = 0（不可用）。第一次 `GUI_X_Lock()` 会拿不到？不，因为：
- `GUI_X_InitOS()` 创建信号量时 `xSemaphoreCreateCountingStatic(1, 0, ...)`
  - 第1个参数 `1` = 最大计数为1（二值信号量）
  - 第2个参数 `0` = 初始计数为0（锁住状态）

这看起来初始是锁住的，但 emWin 的初始化流程是先 `Unlock` 再 `Lock`。SEGGER 的代码就是这样设计的，这是经过验证的模式。

**注意：** 只有当多个任务同时调用 emWin API 时才需要锁。本项目中只有 `GUI_Task` 调用 emWin，所以锁实际上不会产生竞争。但配置 `GUI_OS = 1` 是最佳实践——将来添加其他 GUI 任务时不会出问题。

### 3.4 `bsp_lcd.c` — LCD 硬件驱动 (User/lcd/bsp_lcd.c)

这是最底层的硬件驱动，负责三个核心操作：

#### 3.4.1 时钟配置 `_TLI_Clock_Config()`

```
目标: 产生约 33MHz 的像素时钟给 TLI

时钟树:
  HXTAL (25MHz)
    ↓ ÷ PLL2PSC(5) = 5MHz
    ↓ × PLL2N(100)  = 500MHz (VCO)
    ↓ ÷ PLL2R(4)    = 125MHz
    ↓ ÷ RCU_PLL2R_DIV4 = 31.25MHz → TLI 像素时钟

为什么需要 33MHz？
  800 × 480 @ 60Hz 需要:
  H-total ≈ 1056 (800 + 空白)
  V-total ≈ 525  (480 + 空白)
  PCLK = 1056 × 525 × 60 = 33.26 MHz
```

#### 3.4.2 GPIO 配置 `_TLI_GPIO_Config()`

TLI 外设的所有信号都通过 GPIO 的 Alternate Function (AF) 模式输出：

```
每根线需要三步配置:
  gpio_mode_set()         → 设为 AF 模式
  gpio_output_options_set() → 设为推挽输出, 最高速
  gpio_af_set()           → 选择 AF14 (TLI 功能)

RGB565 共需 20 根线:
  R[4:0]  × 5 根  (红)
  G[5:0]  × 6 根  (绿)
  B[4:0]  × 5 根  (蓝)
  HSYNC       1 根  (水平同步)
  VSYNC       1 根  (垂直同步)
  DE          1 根  (数据使能)
  PCLK        1 根  (像素时钟)
  ─────────────────
  共 20 根
```

#### 3.4.3 TLI 时序配置 `_TLI_Init()`

TLI 需要知道 LCD 面板的时序参数：

```
LCD 一帧的数据结构（从左到右，从上到下）:

  ←─────────── H-total ────────────→
  ┌──┬─────────┬──────────┬────────┐
  │HS│   HBP   │   Active │  HFP   │  ← 水平前/后廊 + 有效区 + 同步
  └──┴─────────┴──────────┴────────┘
     ↑          ↑
     HSYNC      Back Porch    有效像素区   Front Porch
     (同步脉冲)  (后廊)       (800px)     (前廊)

  垂直方向同理: VSYNC → VBP → 480行有效 → VFP

TLI 寄存器填写方式 (以水平方向为例):
  synpsz_hpsz   = HSYNC           = 10
  backpsz_hbpsz = HSYNC + HBP     = 10 + 146 = 156
  activesz_hasz = HSYNC + HBP + Active = 156 + 800 = 956
  totalsz_htsz  = HSYNC + HBP + Active + HFP = 956 + 100 = 1056
```

**层配置：**
```c
layer_init_struct.layer_ppf = LAYER_PPF_RGB565;   // 像素格式: RGB565
layer_init_struct.layer_frame_bufaddr = 0x24000000; // 帧缓冲地址
layer_init_struct.layer_frame_line_length = 800 * 2; // 每行字节数 (RGB565 = 2B/px)
layer_init_struct.layer_frame_total_line_number = 480; // 总行数
```

### 3.5 `main.c` — 应用层 (src/main.c)

#### GUI_Task — emWin 主循环

```c
static void GUI_Task(void* parameter)
{
    GUI_Init();    // 初始化 emWin（内部会调用 LCD_X_Config 和 LCD_X_DisplayDriver）

    // 画一些文字作为演示
    GUI_SetBkColor(GUI_BLACK);     // 背景色 = 黑色
    GUI_Clear();                    // 清屏
    GUI_SetColor(GUI_WHITE);       // 前景色 = 白色
    GUI_SetFont(&GUI_Font24_ASCII); // 选字体
    GUI_DispStringAt("emWin + GD32H7 Ready!", 50, 200);  // 在(50,200)处画字

    while (1) {
        GUI_Delay(50);  // GUI_Exec() 处理绘图/输入 + 延时 50ms
    }
}
```

**`GUI_Delay(50)` 不等于 `vTaskDelay(50)`！**

`GUI_Delay(ms)` 内部做了两件事：
1. 调用 `GUI_Exec()` — 处理所有待处理的绘制操作和输入事件
2. 调用 `GUI_X_Delay(ms)` — 延时

这是 emWin 的标准主循环模式。如果你只调 `vTaskDelay`，控件不会刷新，动画不会动。

#### Touch_Task — 触摸轮询

```c
static void Touch_Task(void* parameter)
{
    bsp_gt1151qm_init();   // 初始化 GT1151QM

    while (1) {
        if (bsp_gt1151qm_read_touch(&x, &y, &pressed)) {
            GUI_PID_StoreState(&state);  // 把触摸坐标喂给 emWin
        }
        vTaskDelay(20ms);  // 50Hz 轮询
    }
}
```

**为什么要独立一个任务？**

触摸数据通过 `GUI_PID_StoreState()` 存入 emWin 的 PID (Pointer Input Device) 缓冲区。emWin 在 `GUI_Exec()` (即 `GUI_Delay` 内部) 时读取这个缓冲区，分发给当前焦点窗口。两个任务的数据流是：

```
Touch_Task (优先级2)              GUI_Task (优先级4)
    │                                  │
    ├─ read_touch() → (300,150)        │
    ├─ GUI_PID_StoreState() ───────────┤→ 存入 PID 缓冲区
    │                                  ├─ GUI_Delay(50) → GUI_Exec()
    │                                  ├─ 读取 PID 缓冲区
    │                                  ├─ 发现坐标(300,150)
    │                                  ├─ 分发给当前窗口
    │                                  ├─ 窗口回调处理点击
    │                                  └─ 刷新显示
```

### 3.6 `bsp_gt1151qm.c` — 触摸驱动 (User/touch/bsp_gt1151qm.c)

GT1151QM 是 Goodix 的电容触摸控制器，通过 I2C 总线通信。

#### I2C 通信模式

GT1151QM 使用 16 位寄存器地址（大端序），读/写分两阶段：

```
写操作: START | SLAVE_ADDR(W) | REG_H | REG_L | DATA... | STOP

读操作: START | SLAVE_ADDR(W) | REG_H | REG_L | STOP
         然后 START | SLAVE_ADDR(R) | DATA... | STOP
```

#### 触摸数据格式

GT1151QM 的触摸数据起始于寄存器 `0x814E`：

```
偏移    内容
0x814E  Status (bit7=数据就绪, bit3-0=触摸点数)
0x814F  X[11:8] (高4位) + 事件标志
0x8150  X[7:0]  (低8位)
0x8151  Y[11:8] (高4位) + 触摸ID
0x8152  Y[7:0]  (低8位)
```

**注意：** GT1151QM 输出的坐标是**屏幕物理坐标**。如果你的触摸屏和 LCD 有偏移，需要通过 emWin 的触摸校准功能矫正。当前代码用 `GUI_PID_StoreState` 直接存原始坐标。

### 3.7 `board.c` — MPU 配置 (src/board.c)

这是最容易忽略但非常关键的部分。

#### 为什么需要配置 MPU？

Cortex-M7 有 **L1 Cache**（I-Cache + D-Cache）。启用 Cache 后：

```
CPU 写帧缓冲 → 先写入 D-Cache (CPU 认为写完了)
              → 过一段时间才真正写入 SRAM

TLI 读帧缓冲 → 直接从 SRAM 读 (看不到 Cache 里的数据)
              → 屏幕上显示的不是最新的画面！
```

这就是 **Cache 一致性问题**。

#### 解决方案：Write-Through

我们在 MPU 中把帧缓冲区域设为 **Write-Through Cacheable**：

```c
mpu_init_struct.region_base_address = 0x24000000;      // 帧缓冲起点
mpu_init_struct.region_size = MPU_REGION_SIZE_1MB;      // 大小 1MB
mpu_init_struct.access_cacheable = MPU_ACCESS_CACHEABLE; // 允许缓存
mpu_init_struct.tex_type = MPU_TEX_TYPE1;               // Write-Through!
```

**Write-Through 的含义：**
- CPU **读**帧缓冲：从 Cache 读（快）
- CPU **写**帧缓冲：同时写入 Cache 和 SRAM（稍慢，但数据对 TLI 可见）

这是性能和正确性的最佳平衡。如果不设 Write-Through，就需要在每次绘制后手动调用 `SCB_CleanDCache()` 刷新缓存，否则显示的是旧数据。

---

## 4. 启动流程：从开机到第一帧画面

下面是完整的上电时序：

```
┌─ main() ──────────────────────────────────────────────────────────┐
│                                                                   │
│  1. BSP_Init()                                                    │
│     ├─ SCB_EnableICache()      // 开 I-Cache                      │
│     ├─ SCB_EnableDCache()      // 开 D-Cache                      │
│     ├─ mpu_config()            // 配置 MPU (含帧缓冲 Write-Thru)  │
│     ├─ USART_Config()          // 初始化串口                      │
│     ├─ KEY_GPIO_Config()       // 初始化按键                      │
│     └─ LED_GPIO_Config()       // 初始化 LED                      │
│                                                                   │
│  2. xTaskCreate(AppTaskCreate, ...)                               │
│                                                                   │
│  3. vTaskStartScheduler() → 开始任务调度                          │
│                                                                   │
└───────────────────────────────────────────────────────────────────┘
         │
         ▼
┌─ AppTaskCreate (优先级 1) ────────────────────────────────────────┐
│                                                                   │
│  1. TCPIP_Init()              // 初始化 LwIP 协议栈               │
│                                                                   │
│  2. bsp_lcd_init()            // ★ 初始化 LCD 硬件 ★             │
│     ├─ _TLI_Clock_Config()    //   配置 PLL2 → 31.25MHz 像素时钟  │
│     ├─ _TLI_GPIO_Config()     //   配置 20 根 TLI GPIO 引脚       │
│     ├─ _TLI_Init()            //   配置 TLI 时序 + 层参数         │
│     ├─ memset(fb, 0, ...)     //   帧缓冲清零 (黑屏)              │
│     └─ bsp_lcd_backlight_on() //   点亮背光                       │
│                                                                   │
│  3. xTaskCreate(GUI_Task, ...)     // 创建 GUI 任务 (优先级 4)    │
│  4. xTaskCreate(Touch_Task, ...)   // 创建触摸任务 (优先级 2)     │
│  5. xTaskCreate(Test1_Task, ...)   // 创建 LED 任务               │
│  6. xTaskCreate(Test2_Task, ...)   // 创建 LED 任务               │
│  7. xTaskCreate(Test3_Task, ...)   // 创建按键+TCP任务             │
│                                                                   │
│  8. vTaskDelete(self)         // AppTaskCreate 自杀               │
│                                                                   │
└───────────────────────────────────────────────────────────────────┘
         │
         ├──────────────────────┐
         ▼                      ▼
┌─ GUI_Task (优先级 4) ───┐  ┌─ Touch_Task (优先级 2) ──┐
│                         │  │                          │
│ GUI_Init()              │  │ bsp_gt1151qm_init()     │
│  ├─ LCD_X_Config()      │  │                          │
│  │   └─ 创建驱动设备    │  │ while(1) {              │
│  ├─ LCD_X_DisplayDriver │  │   read_touch(&x, &y)    │
│  │   └─ INITCONTROLLER  │  │   GUI_PID_StoreState()  │
│  └─ 初始化完成          │  │   vTaskDelay(20ms)      │
│                         │  │ }                        │
│ GUI_Clear()             │  └──────────────────────────┘
│ GUI_DispStringAt(...)   │
│                         │
│ while(1) {              │
│   GUI_Delay(50)         │
│ }                       │
└─────────────────────────┘
```

**此时屏幕显示：** 黑底白字 "emWin + GD32H7 Ready!"

---

## 5. 绘制流程：GUI_DispString 到底做了什么

以 `GUI_DispStringAt("Hello", 100, 200)` 为例：

```
GUI_DispStringAt("Hello", 100, 200)
  │
  ├─ 1. 计算起始像素位置: offset = (200 * 800 + 100) * 2
  │      = 帧缓冲中第 160,100 个字节
  │
  ├─ 2. 查字体表 GUI_Font24_ASCII
  │     'H' 的位图数据: 24x24 像素的 1-bit 位图
  │
  ├─ 3. 对位图中每个像素:
  │     if (位图像素 == 1) {
  │       s_frame_buffer[offset] = GUI_WHITE;  // 画前景色
  │     } else {
  │       s_frame_buffer[offset] = GUI_BLACK;  // 画背景色
  │     }
  │
  ├─ 4. offset 移到下一个字符位置
  │
  └─ 5. 循环直到所有字符画完

画完后，帧缓冲中的数据变了。
TLI 硬件在下一个刷新周期自动读取新数据 → 屏幕显示新内容。

整个过程 CPU 只读写 SRAM（帧缓冲），TLI 硬件自己读取。
CPU 不需要管 HSYNC/VSYNC/DE 这些信号，TLI 硬件全自动。
```

**关键理解：** emWin 的所有绘制操作（画线、画圆、画字、画位图）最终都是**往帧缓冲写像素值**。显示则是 TLI 硬件从帧缓冲**读**像素值然后输出到屏幕。软件只负责写，硬件只负责读，互不干扰。

---

## 6. 触摸流程：手指到 UI 响应

```
手指触摸屏幕
    │
    ▼
GT1151QM 电容感应 → 检测到触摸 → 更新内部寄存器
    │
    ▼ (20ms 后)
Touch_Task 轮询:
    bsp_gt1151qm_read_touch(&x=300, &y=150, &pressed=1)
    │
    ├─ I2C 读寄存器 0x814E
    │   Status = 0x81 (数据就绪 + 1个触摸点)
    │
    ├─ I2C 读坐标数据
    │   X = (0x0F & 0x814F) << 8 | 0x8150 = 300
    │   Y = (0x0F & 0x8151) << 8 | 0x8152 = 150
    │
    ├─ I2C 写 0x814E = 0x00 (清除状态，准备下次读)
    │
    └─ GUI_PID_StoreState(&state)
         │
         └─ state = { x=300, y=150, Pressed=1 }
            存入 emWin 内部的 PID 缓冲区
    │
    ▼ (最多 50ms 后)
GUI_Task:
    GUI_Delay(50) 内部调用 GUI_Exec()
    │
    └─ 从 PID 缓冲区取出 state
         │
         ├─ 找到坐标 (300,150) 所在的窗口
         │   (如果没有窗口，就是 desktop)
         │
         ├─ 发送 WM_TOUCH 消息给该窗口的回调函数
         │
         └─ 窗口回调处理:
              if (消息 == WM_TOUCH) {
                  更新按钮状态 (按下/释放)
                  重绘按钮 (不同颜色)
              }
```

---

## 7. 内存布局详解

```
GD32H759IM 内部 SRAM 总览:

0x24000000 ┌─────────────────────────┐  ← 帧缓冲起点
           │                         │
           │  s_frame_buffer[800][480]│  768 KB (800×480×2)
           │  RGB565 像素数据         │
           │                         │
0x240BB800 ├─────────────────────────┤  ← 帧缓冲结束 (0x24000000 + 0xBB800)
           │                         │
           │  其他 .data / .bss      │  ≈128 KB
           │  (FreeRTOS 堆栈,        │
           │   LwIP 数据,             │
           │   emWin 动态池(64KB),    │
           │   应用变量)              │
           │                         │
0x240E0000 └─────────────────────────┘  总 SRAM 区域结束

0x20000000 ┌─────────────────────────┐  AXI SRAM
           │  64KB                    │  FreeRTOS 系统堆 (heap_4.c)
0x20010000 └─────────────────────────┘

0x30000000 ┌─────────────────────────┐
           │  16KB DMA 描述符         │  Non-cacheable (MPU Region 0)
0x30004000 ├─────────────────────────┤
           │  16KB LwIP 堆            │  Non-cacheable (MPU Region 1)
0x30008000 └─────────────────────────┘

0x00000000 ┌─────────────────────────┐  ITCM (64KB)
           │  关键代码/中断向量       │
0x00010000 └─────────────────────────┘
```

**emWin 的 64KB 动态内存池用来存什么？**

```
GUI_ALLOC_SIZE = 65536 (64KB)
├─ 窗口管理器数据结构  ~2KB
├─ 窗口/控件结构体     (每个按钮 ~200B, 每个窗口 ~500B)
├─ 字符串缓冲区
├─ 字体缓存
├─ Clip Rectangle 数据
└─ 其他 emWin 内部分配
```

如果你创建了很多控件（按钮、滑块、编辑框等），可能需要增大 `GUI_ALLOC_SIZE`。但如果 64KB 不够用，剩下的内部 SRAM 只有 ~128KB，需要谨慎分配。

---

## 8. TLI 硬件详解

### 8.1 TLI 是什么

TLI (TFT-LCD Interface) 是 GD32H7 系列的一个硬件外设，专门用于驱动并行 RGB 接口的 LCD 屏幕。

```
TLI 硬件模块
┌─────────────────────────────────────────────────────┐
│                                                     │
│  AHB Master (DMA)                                   │
│  ┌──────────┐    读帧缓冲                           │
│  │  DMA 引擎 │ ←──────────→ SRAM (s_frame_buffer)    │
│  └────┬─────┘                                       │
│       │ 像素数据流                                   │
│       ▼                                             │
│  ┌──────────┐                                       │
│  │ 像素格式  │  拆解 RGB565 → R[4:0], G[5:0], B[4:0]│
│  │ 转换器   │                                       │
│  └────┬─────┘                                       │
│       │                                             │
│       ▼                                             │
│  ┌──────────┐                                       │
│  │ 时序生成器│  产生 HSYNC, VSYNC, DE, PCLK          │
│  └────┬─────┘                                       │
│       │                                             │
│       ▼                                             │
│  ┌──────────┐                                       │
│  │  Layer 0 │  窗口裁剪、Alpha混合、颜色键           │
│  │  Layer 1 │  (我们用了 Layer 0 单层)              │
│  └────┬─────┘                                       │
│       │                                             │
│       ▼ 20根输出引脚                                 │
│    R[4:0] G[5:0] B[4:0] HSYNC VSYNC DE PCLK        │
│                                                     │
└─────────────────────────────────────────────────────┘
```

### 8.2 TLI vs STM32 LTDC

GD32H7 的 TLI 和 STM32H7 的 LTDC 功能几乎一样，寄存器也很相似：

| GD32H7 TLI | STM32H7 LTDC | 功能 |
|------------|-------------|------|
| `TLI_SPSZ` | `LTDC_SSCR` | 同步脉冲大小 |
| `TLI_BPSZ` | `LTDC_BPCR` | 后廊大小 |
| `TLI_ASZ` | `LTDC_AWCR` | 有效区大小 |
| `TLI_TSZ` | `LTDC_TWCR` | 总大小 |
| `TLI_CTL` | `LTDC_GCR` | 控制寄存器 |
| `TLI_LXFBADDR` | `LTDC_LxCFBAR` | 层帧缓冲地址 |

**这就是为什么 emWin 的 STM32H7 `LCDConf.c` 示例可以直接参考**——把 `HAL_LTDC_xxx` 替换为 GD32 的 `tli_xxx` 即可。

### 8.3 RGB565 数据线接法

对于 5 寸 800x480 RGB565 面板，只需要 16 根数据线：

```
TLI 输出 (24-bit RGB888)       面板接收 (16-bit RGB565)
  TLI_R7 ────────────────►  R4 (MSB of Red,   5 bits)
  TLI_R6 ────────────────►  R3
  TLI_R5 ────────────────►  R2
  TLI_R4 ────────────────►  R1
  TLI_R3 ────────────────►  R0 (LSB of Red)

  TLI_G7 ────────────────►  G5 (MSB of Green, 6 bits)
  TLI_G6 ────────────────►  G4
  TLI_G5 ────────────────►  G3
  TLI_G4 ────────────────►  G2
  TLI_G3 ────────────────►  G1
  TLI_G2 ────────────────►  G0 (LSB of Green)

  TLI_B7 ────────────────►  B4 (MSB of Blue,  5 bits)
  TLI_B6 ────────────────►  B3
  TLI_B5 ────────────────►  B2
  TLI_B4 ────────────────►  B1
  TLI_B3 ────────────────►  B0 (LSB of Blue)

未用的低位 (TLI_R2-R0, TLI_G1-G0, TLI_B2-B0): 浮空即可
```

TLI 配置为 `LAYER_PPF_RGB565` 后，硬件会自动把 16 位 RGB565 像素值拆分到对应的高位数据线上。

---

## 9. 关键设计决策 FAQ

### Q: 为什么用单缓冲而不是双缓冲？

A: 双缓冲需要 2 × 768KB = 1536KB，超过内部 SRAM 总量 (896KB)。如果板子上有外部 SDRAM 并配置好了 EXMC，可以改成双缓冲：

```c
// 双缓冲配置 (需要外部 SDRAM)
#define NUM_BUFFERS 2
// 帧缓冲地址改为 SDRAM 地址
#define LCD_FRAME_BUFFER_0  0xC0000000
#define LCD_FRAME_BUFFER_1  0xC00BC000
```

双缓冲的好处：在一个缓冲区绘制时，TLI 显示另一个，画面不撕裂。

### Q: 为什么用 GUIDRV_LIN_16 而不是 GUIDRV_FlexColor？

A:
- `GUIDRV_LIN_xx` — 用于**线性帧缓冲**（整个屏幕的像素存在连续内存中），适合 TLI/LTDC 这种带 DMA 的 RGB 接口
- `GUIDRV_FlexColor` — 用于**8080 并口屏**或 SPI 屏，需要通过命令/数据总线逐像素发送

我们用的是 RGB 并行接口，帧缓冲是连续的 → 用 `GUIDRV_LIN_16`。

### Q: GUI_OS 到底要不要设为 1？

A: 设为 1 是最安全的做法：
- 如果只有一个任务调用 emWin API → 锁操作很快，几乎无开销
- 如果将来添加第二个 GUI 任务 → 自动有互斥保护，不会出现数据竞争

### Q: MPU 不配 Write-Through 会怎样？

A: 屏幕显示会出问题：
- 部分区域不刷新（看起来"卡住"了）
- 动画有撕裂
- 文字有残影

这些都是因为 CPU 的 D-Cache 中的数据还没写到 SRAM，TLI 就读了旧数据。

如果不用 Write-Through，也可以用软件刷新：
```c
// 每次绘制后手动 Clean Cache
SCB_CleanDCache_by_Addr(s_frame_buffer, sizeof(s_frame_buffer));
```
但性能比 Write-Through 差，因为每次要刷整个 768KB。

### Q: 触摸坐标不准怎么办？

A: emWin 提供触摸校准功能。在 `GUI_Init()` 之后调用：

```c
GUI_TOUCH_Calibrate(GUI_COORD_X, 0, 799, 触摸x_min, 触摸x_max);
GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, 479, 触摸y_min, 触摸y_max);
```

或者用 emWin 内置的校准画面：
```c
GUI_TOUCH_Exec();  // 交互式四点校准
```

前提是改用 `GUI_TOUCH_StoreStateEx()` 而不是 `GUI_PID_StoreState()`。

---

## 附录：文件结构速查

```
工程根目录/
│
├── src/                          ← 应用源码
│   ├── main.c                    ← 入口 + GUI_Task + Touch_Task
│   ├── board.c                   ← BSP_Init + MPU 配置
│   ├── GUIConf.h                 ← emWin 核心开关
│   ├── LCDConf.c                 ← LCD 硬件适配 (LCD_X_Config + DisplayDriver)
│   ├── GUI_X_FreeRTOS.c          ← OS 抽象层 (时间 + 锁 + 延时)
│   └── ...
│
├── User/                         ← 板级 BSP 驱动
│   ├── lcd/
│   │   ├── bsp_lcd.h             ← LCD 参数宏
│   │   └── bsp_lcd.c             ← TLI 时钟/GPIO/时序/背光
│   └── touch/
│       ├── bsp_gt1151qm.h        ← 触摸参数宏
│       └── bsp_gt1151qm.c        ← I2C + GT1151QM 协议
│
├── GD32H7xx_GDemWin652/          ← emWin 库
│   ├── Include/                  ← GUI.h, LCD.h, BUTTON.h ...
│   ├── Lib/
│   │   └── GDemWin_CM7_OS_DP.lib ← 我们用的预编译库
│   └── Sample/                   ← SEGGER 官方示例 (参考用)
│
├── FreeRTOS/                     ← FreeRTOS 内核
├── lwip-2.1.2/                   ← LwIP 协议栈
├── Firmware/                     ← GD32 HAL 库
└── MDK-ARM/                      ← Keil 工程
    ├── Project.uvprojx           ← 工程文件 (已添加 GDemWin 组)
    └── output/
        └── Project.sct           ← 散列文件 (已保留帧缓冲区域)
```
