# EmbeddedTemplate

**中文** | [English](README_EN.md)

一个现代化的STM32嵌入式开发模板，支持多种工具链和开发板。

## 特性

- 多工具链支持：ARM GCC 和 ARM Clang (Keil MDK)
- 多开发板支持：STM32F407ZG、STM32H743ZI 
- 模块化架构：清晰的目录结构和CMake组织
- VS Code集成：完整的开发环境配置
- 调试支持：集成RTT (Real-Time Transfer) 调试输出
- 现代构建系统：基于CMake和Ninja的快速构建

## 支持的硬件

| 开发板 | MCU | 架构 | 工具链 |
|--------|-----|------|--------|
| STM32F407ZG | STM32F407ZGT6 | Cortex-M4 | GCC, Clang |
| STM32H743ZI | STM32H743ZIT6 | Cortex-M7 | GCC, Clang |

## 环境要求

### 必需工具
- VS Code 1.60+
- CMake 3.20+
- Ninja Build
- CMake Tools 扩展

### 工具链
**ARM GCC** (推荐)
- 版本：GNU Arm Embedded Toolchain 10.3+
- 下载：[ARM Developer](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)

**ARM Clang** (可选)
- 版本：ARM Compiler 6.16+
- 来源：Keil MDK 或 ARM Development Studio

## 快速开始

### 1. 克隆项目
```bash
git clone --recursive https://github.com/Nowaterisenough/EmbeddedTemplate.git
```

### 2. 配置环境变量

**方法一：系统环境变量（推荐）**

在系统环境变量中添加：
```
ARM_CLANG_PATH=C:\path\to\ArmCompiler6.xx\bin
ARM_GCC_PATH=C:\path\to\arm-gnu-toolchain\bin
```

**方法二：命令行临时设置**

Windows PowerShell:
```powershell
$env:ARM_CLANG_PATH="C:\path\to\ArmCompiler6.xx\bin"
$env:ARM_GCC_PATH="C:\path\to\arm-gnu-toolchain\bin"
```

Windows CMD:
```cmd
set ARM_CLANG_PATH=C:\path\to\ArmCompiler6.xx\bin
set ARM_GCC_PATH=C:\path\to\arm-gnu-toolchain\bin
```

Linux/macOS:
```bash
export ARM_CLANG_PATH="/path/to/ArmCompiler6.xx/bin"
export ARM_GCC_PATH="/path/to/arm-gnu-toolchain/bin"
```

**方法三：VS Code工作区配置**

在 `.vscode/settings.json` 中配置（仅VS Code环境）：
```json
{
  "cmake.configureEnvironment": {
    "ARM_CLANG_PATH": "C:/path/to/ArmCompiler6.xx/bin",
    "ARM_GCC_PATH": "C:/path/to/arm-gnu-toolchain/bin"
  },
  "cortex-debug.gdbPath": "C:/path/to/arm-gnu-toolchain/bin/arm-none-eabi-gdb.exe",
  "cortex-debug.objdumpPath": "C:/path/to/arm-gnu-toolchain/bin/arm-none-eabi-objdump.exe",
  "cortex-debug.JLinkGDBServerPath": "C:/Program Files/SEGGER/JLink_Vxxx/JLinkGDBServerCL.exe"
}
```

### 3. 选择构建配置
在VS Code中：
1. 打开命令面板 (Ctrl+Shift+P)
2. 选择 `CMake: Select Configure Preset`
3. 选择目标配置：
   - `h743-gcc` - H743开发板 + ARM GCC
   - `h743-clang` - H743开发板 + ARM Clang
   - `f407-gcc` - F407开发板 + ARM GCC
   - `f407-clang` - F407开发板 + ARM Clang

### 4. 构建项目
按 F7 或运行：
```bash
cmake --build build
```

### 5. 输出文件
构建完成后在 `build/bin/<board>/` 目录下生成：
- `*.elf` - 可执行文件
- `*.bin` - 二进制镜像
- `*.hex` - Intel HEX格式
- `*.map` - 内存映射文件

## 项目结构

```
EmbeddedTemplate/
├── 3rd/                    # 第三方库
│   └── RTT/                # SEGGER RTT调试库
├── app/                    # 应用程序代码
├── boards/                 # 开发板支持包
│   ├── stm32f407zg/       # STM32F407ZG开发板
│   └── stm32h743zi/       # STM32H743ZI开发板
├── cmake/                  # CMake工具链文件
│   └── toolchains/
│       ├── arm-none-eabi-gcc.cmake
│       └── arm-none-eabi-clang.cmake
├── drivers/                # 硬件抽象层
│   ├── STM32F4/           # STM32F4 HAL
│   └── STM32H7/           # STM32H7 HAL
├── include/               # 公共头文件
├── middleware/            # 中间件
├── modules/               # 功能模块
└── tests/                 # 测试代码
```

## 开发指南

### Board API 使用

项目提供统一的板级 API，所有开发板共享相同接口：

#### 1. 初始化

```c
#include "board.h"

int main(void) {
    board_init();      // 初始化系统时钟、HAL等
    board_led_init();  // 初始化LED（如果板子支持）

    // 你的应用代码
}
```

#### 2. LED 控制

```c
// LED 枚举：BOARD_LED_1, BOARD_LED_2, BOARD_LED_3, BOARD_LED_4
// 状态枚举：LED_ON, LED_OFF

// 设置 LED 状态
board_led_set(BOARD_LED_1, LED_ON);   // 点亮 LED1
board_led_set(BOARD_LED_2, LED_OFF);  // 熄灭 LED2

// 翻转 LED 状态
board_led_toggle(BOARD_LED_1);        // LED1 状态取反
```

#### 3. 延时与时间

```c
// 毫秒级延时
board_delay_ms(1000);  // 延时 1 秒

// 获取系统运行时间（毫秒）
uint32_t uptime = board_millis();
```

#### 4. 错误处理

```c
// 断言检查（仅在 DEBUG 模式生效）
BOARD_ASSERT(value != 0);

// 致命错误处理
board_error_handler(__FILE__, __LINE__, __func__);  // 记录错误并挂起
board_fatal_halt();  // 直接挂起系统
```

### 如何添加新板子

假设要添加 `STM32L476RG` 开发板：

#### 1. 创建板子目录结构

```
boards/stm32l476rg/
├── CMakeLists.txt          # 板级 CMake 配置
├── board.c                 # 板级实现
├── board_config.h          # 硬件配置
├── config/
│   ├── stm32l4xx_hal_conf.h
│   ├── system_stm32l4xx.c
│   └── stm32l4xx_it.c
├── startup/
│   └── startup_stm32l476xx.s
└── linker/
    └── stm32l476rg_flash.ld
```

#### 2. 定义 LED 映射（board_config.h）

```c
#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "stm32l4xx.h"

#define BOARD_HAS_LED 1

/* LED 硬件映射表 */
#define BOARD_LED_MAP { \
    {GPIOA, GPIO_PIN_5, __HAL_RCC_GPIOA_CLK_ENABLE}, /* BOARD_LED_1 */ \
    {NULL,  0,          NULL},                       /* 未使用 */ \
    {NULL,  0,          NULL},                       /* 未使用 */ \
    {NULL,  0,          NULL}                        /* 未使用 */ \
}

/* LED 极性 */
#define BOARD_LED_ACTIVE_HIGH 1

#endif
```

#### 3. 实现板级代码（board.c）

参考 `boards/stm32h743zi/board.c`，实现：
- `board_init()` - 初始化时钟和HAL
- `board_led_init()` - 使用映射表初始化LED（已提供通用实现）
- `board_led_set()` - 控制LED（已提供通用实现）
- `board_led_toggle()` - 翻转LED（已提供通用实现）

**关键代码片段：**

```c
/* LED 硬件映射表（通用实现） */
typedef struct {
    GPIO_TypeDef *port;
    uint16_t      pin;
    void        (*clk_enable)(void);
} led_map_t;

static const led_map_t led_map[] = BOARD_LED_MAP;
static uint8_t led_state[BOARD_LED_MAX] = {0};

void board_led_init(void) {
    GPIO_InitTypeDef gpio_init = {
        .Mode  = GPIO_MODE_OUTPUT_PP,
        .Pull  = GPIO_NOPULL,
        .Speed = GPIO_SPEED_FREQ_LOW
    };

    for (size_t i = 0; i < BOARD_LED_MAX; i++) {
        if (led_map[i].port == NULL) continue;
        if (led_map[i].clk_enable) led_map[i].clk_enable();
        gpio_init.Pin = led_map[i].pin;
        HAL_GPIO_Init(led_map[i].port, &gpio_init);
        board_led_set((board_led_id_t)i, LED_OFF);
    }
}

void board_led_set(board_led_id_t led, led_state_t state) {
    if (led >= BOARD_LED_MAX || led_map[led].port == NULL) return;
    led_state[led] = state;

    GPIO_PinState pin_state;
    #if BOARD_LED_ACTIVE_HIGH
        pin_state = state ? GPIO_PIN_SET : GPIO_PIN_RESET;
    #else
        pin_state = state ? GPIO_PIN_RESET : GPIO_PIN_SET;
    #endif

    HAL_GPIO_WritePin(led_map[led].port, led_map[led].pin, pin_state);
}

void board_led_toggle(board_led_id_t led) {
    if (led >= BOARD_LED_MAX) return;
    board_led_set(led, led_state[led] ? LED_OFF : LED_ON);
}
```

#### 4. 更新 CMakePresets.json

添加新板子的构建配置：

```json
{
  "name": "l476-gcc",
  "displayName": "STM32L476RG - ARM GCC",
  "cacheVariables": {
    "BOARD": "stm32l476rg"
  }
}
```

#### 5. 构建测试

```bash
cmake --preset l476-gcc
cmake --build build
```

### 数据驱动设计原则

本项目采用**数据结构优先**的设计理念（参考 Linus Torvalds "好品味"原则）：

**❌ 避免：硬编码的条件分支**
```c
// 不推荐：每个 LED 都有 switch-case
switch (led) {
    case LED1: HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, state); break;
    case LED2: HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, state); break;
    // ...
}
```

**✅ 推荐：映射表驱动**
```c
// 硬件关系在编译时定义为数据
static const led_map_t led_map[] = BOARD_LED_MAP;

// 运行时直接索引，无分支
HAL_GPIO_WritePin(led_map[led].port, led_map[led].pin, state);
```

**优势：**
- 添加新板子只需修改一行宏定义
- 所有板子的 `board.c` 实现完全一致
- 零特殊情况，零条件分支

## 调试

项目集成了SEGGER RTT，支持实时调试输出：

```c
#include "SEGGER_RTT.h"

// 使用RTT输出
SEGGER_RTT_printf(0, "Hello RTT!\n");

// 或使用重定向的printf
printf("Debug: value = %d\n", value);
```

### VS Code Cortex调试配置（可选）

如需在VS Code中使用J-Link进行调试，在 `.vscode/launch.json` 中添加以下配置：

**STM32H743ZI调试配置：**
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug STM32H743 (ARM GCC)",
            "cwd": "${workspaceFolder}",
            "executable": "${workspaceFolder}/build/bin/stm32h743zi/EmbeddedTemplate.elf",
            "request": "launch",
            "type": "cortex-debug",
            "runToEntryPoint": "main",
            "servertype": "jlink",
            "device": "STM32H743ZI",
            "interface": "swd",
            "serialNumber": "",
            "showDevDebugOutput": "parsed",
            "swoConfig": {
                "enabled": true,
                "cpuFrequency": 400000000,
                "swoFrequency": 4000000,
                "source": "probe"
            },
            "gdbPath": "${config:cortex-debug.gdbPath}",
            "toolchainPrefix": "arm-none-eabi",
            "objdumpPath": "${config:cortex-debug.objdumpPath}",
            "serverpath": "${config:cortex-debug.JLinkGDBServerPath}",
            "serverArgs": [
                "-if", "swd", "-device", "STM32H743ZI", "-endian", "little",
                "-speed", "4000", "-nogui", "-singlerun"
            ],
            "postLaunchCommands": [
                "monitor reset",
                "monitor halt"
            ]
        }
    ]
}
```

**STM32F407ZG调试配置：**
```json
{
    "name": "Debug STM32F407 (ARM GCC)",
    "cwd": "${workspaceFolder}",
    "executable": "${workspaceFolder}/build/bin/stm32f407zg/EmbeddedTemplate.elf",
    "request": "launch",
    "type": "cortex-debug",
    "runToEntryPoint": "main",
    "servertype": "jlink",
    "device": "STM32F407ZG",
    "interface": "swd",
    "serialNumber": "",
    "showDevDebugOutput": "parsed",
    "swoConfig": {
        "enabled": true,
        "cpuFrequency": 168000000,
        "swoFrequency": 4000000,
        "source": "probe"
    },
    "gdbPath": "${config:cortex-debug.gdbPath}",
    "toolchainPrefix": "arm-none-eabi",
    "objdumpPath": "${config:cortex-debug.objdumpPath}",
    "serverpath": "${config:cortex-debug.JLinkGDBServerPath}",
    "serverArgs": [
        "-if", "swd", "-device", "STM32F407ZG", "-endian", "little",
        "-speed", "4000", "-nogui", "-singlerun"
    ],
    "postLaunchCommands": [
        "monitor reset",
        "monitor halt"
    ]
}
```

**调试前提条件：**
- 安装Cortex-Debug扩展
- 配置工具链路径（见上文环境变量配置）
- 连接J-Link调试器到目标板

## 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件。
