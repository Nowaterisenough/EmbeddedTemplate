# EmbeddedTemplate

**中文** | [English](README_EN.md)

一个现代化、模块化的 STM32 嵌入式开发模板，支持多工具链、多开发板，基于 CMake 构建系统。

## 特性

- **多工具链支持**：ARM GCC / ARM Clang (Keil)
- **多开发板支持**：STM32F407ZG、STM32H743ZI
- **数据驱动设计**：统一的板级 API，零条件分支
- **自动版本管理**：基于 Conventional Commits 的语义化版本
- **现代构建系统**：CMake + Ninja，支持 VS Code 集成
- **调试支持**：SEGGER RTT 实时日志输出

## 支持的硬件

| 开发板 | MCU | 架构 | 工具链 |
|--------|-----|------|--------|
| STM32F407ZG | STM32F407ZGT6 | Cortex-M4 | GCC, Clang |
| STM32H743ZI | STM32H743ZIT6 | Cortex-M7 | GCC, Clang |

## 快速开始

### 1. 克隆项目
```bash
git clone --recursive https://github.com/Nowaterisenough/EmbeddedTemplate.git
cd EmbeddedTemplate
```

### 2. 配置工具链

设置环境变量（选择一种方式）：

```powershell
# Windows PowerShell
$env:ARM_GCC_PATH="C:\path\to\arm-gnu-toolchain\bin"
$env:ARM_CLANG_PATH="C:\path\to\ArmCompiler6.xx\bin"  # 可选
```

```bash
# Linux/macOS
export ARM_GCC_PATH="/path/to/arm-gnu-toolchain/bin"
export ARM_CLANG_PATH="/path/to/ArmCompiler6.xx/bin"  # 可选
```

### 3. 构建

```bash
# 选择构建配置
cmake --preset h743-gcc          # H743 + GCC
cmake --preset f407-gcc          # F407 + GCC

# 构建
cmake --build build

# 输出文件在: build/bin/<board>/
```

### 4. 固件版本信息

项目支持自动版本管理，版本号嵌入固件中：

```bash
# 查看固件版本（需要 pyelftools）
python tools/extract_version.py build/bin/stm32h743zi/EmbeddedTemplate.elf
```

## 项目结构

```
EmbeddedTemplate/
├── 3rd/                    # 第三方库 (SEGGER RTT)
├── app/                    # 应用程序代码
├── boards/                 # 开发板支持包
│   ├── stm32f407zg/
│   └── stm32h743zi/
├── cmake/                  # CMake 工具链
├── drivers/                # STM32 HAL 驱动
├── modules/                # 功能模块 (scheduler, hylink)
└── tools/                  # 构建工具
```

## 开发指南

### 统一的 Board API

所有开发板共享相同接口：

```c
#include "board.h"

int main(void) {
    board_init();               // 初始化系统
    board_led_init();           // 初始化 LED

    while (1) {
        board_led_toggle(BOARD_LED_1);
        board_delay_ms(1000);
    }
}
```

**API 列表**：
- `board_init()` - 初始化时钟和外设
- `board_led_set(led, state)` - 设置 LED 状态
- `board_led_toggle(led)` - 翻转 LED
- `board_delay_ms(ms)` - 毫秒延时
- `board_millis()` - 获取系统运行时间

### 添加新开发板

1. 复制 `boards/stm32h743zi/` 目录到 `boards/<your-board>/`
2. 修改 `board_config.h` 中的 LED 映射表：

```c
#define BOARD_LED_MAP { \
    {GPIOH, GPIO_PIN_10, __HAL_RCC_GPIOH_CLK_ENABLE}, /* LED1 */ \
    {GPIOH, GPIO_PIN_11, __HAL_RCC_GPIOH_CLK_ENABLE}, /* LED2 */ \
    {GPIOH, GPIO_PIN_12, __HAL_RCC_GPIOH_CLK_ENABLE}, /* LED3 */ \
}
```

3. 在 `CMakePresets.json` 添加构建配置
4. 构建测试：`cmake --preset <your-board>-gcc`

### 数据驱动设计

项目采用 **数据结构优先** 的设计理念，避免硬编码的条件分支：

```c
// ❌ 避免：硬编码的 switch-case
switch (led) {
    case LED1: HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, state); break;
    case LED2: HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, state); break;
}

// ✅ 推荐：映射表驱动，零分支
static const led_map_t led_map[] = BOARD_LED_MAP;
HAL_GPIO_WritePin(led_map[led].port, led_map[led].pin, state);
```

### 调试输出

使用 SEGGER RTT 进行实时调试：

```c
#include "SEGGER_RTT.h"

SEGGER_RTT_printf(0, "Debug: value = %d\n", value);
```

## 版本管理

项目遵循 [Conventional Commits](https://www.conventionalcommits.org/) 规范：

```bash
# 提交格式
<type>(<scope>): <subject>

# 示例
feat(led): 添加 RGB LED 驱动
fix(uart): 修复接收缓冲区溢出
```

**版本规则**：
- `feat:` → MINOR 版本 +1 (v1.0.0 → v1.1.0)
- `fix:` → PATCH 版本 +1 (v1.1.0 → v1.1.1)
- MAJOR 版本需手动修改 [.github/workflows/auto-release.yml](.github/workflows/auto-release.yml#L40)

详见 [提交规范文档](.github/COMMIT_CONVENTION.md)。

## 手动发布

```bash
# 交互式发布
./scripts/release.sh              # Linux/macOS
.\scripts\release.ps1             # Windows

# 指定版本发布
./scripts/release.sh v1.2.3
```

## VS Code 配置

### 必需扩展
- **CMake Tools** - 构建系统集成
- **Cortex-Debug** (可选) - 硬件调试支持

### 基础配置

在 `.vscode/settings.json` 中配置工具链路径：

```json
{
  "cmake.configureEnvironment": {
    "ARM_GCC_PATH": "C:/path/to/arm-gnu-toolchain/bin",
    "ARM_CLANG_PATH": "C:/path/to/ArmCompiler6.xx/bin"
  },
  "cortex-debug.gdbPath": "C:/path/to/arm-gnu-toolchain/bin/arm-none-eabi-gdb.exe",
  "cortex-debug.objdumpPath": "C:/path/to/arm-gnu-toolchain/bin/arm-none-eabi-objdump.exe",
  "cortex-debug.JLinkGDBServerPath": "C:/Program Files/SEGGER/JLink_Vxxx/JLinkGDBServerCL.exe"
}
```

### 硬件调试配置

在 `.vscode/launch.json` 中添加调试配置（需要 J-Link 调试器）：

**STM32H743ZI 调试配置：**
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

**STM32F407ZG 调试配置：**
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
- 安装 Cortex-Debug 扩展
- 配置工具链路径（见上文基础配置）
- 连接 J-Link 调试器到目标板

## 许可证

MIT License - 详见 [LICENSE](LICENSE)

## 相关资源

- [CMake 构建系统](https://cmake.org/documentation/)
- [Conventional Commits 规范](https://www.conventionalcommits.org/)
- [SEGGER RTT 文档](https://www.segger.com/products/debug-probes/j-link/technology/about-real-time-transfer/)
