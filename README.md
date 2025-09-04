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
