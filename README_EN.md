# EmbeddedTemplate

[中文](README.md) | **English**

A modern STM32 embedded development template supporting multiple toolchains and development boards.

## Features

- Multi-toolchain support: ARM GCC and ARM Clang (Keil MDK)
- Multi-board support: STM32F407ZG, STM32H743ZI
- Modular architecture: Clear directory structure and CMake organization
- VS Code integration: Complete development environment configuration
- Debug support: Integrated RTT (Real-Time Transfer) debug output
- Modern build system: Fast building based on CMake and Ninja

## Supported Hardware

| Board | MCU | Architecture | Toolchain |
|-------|-----|--------------|-----------|
| STM32F407ZG | STM32F407ZGT6 | Cortex-M4 | GCC, Clang |
| STM32H743ZI | STM32H743ZIT6 | Cortex-M7 | GCC, Clang |

## Requirements

### Required Tools
- VS Code 1.60+
- CMake 3.20+
- Ninja Build
- CMake Tools Extension

### Toolchains
**ARM GCC** (Recommended)
- Version: GNU Arm Embedded Toolchain 10.3+
- Download: [ARM Developer](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)

**ARM Clang** (Optional)
- Version: ARM Compiler 6.16+
- Source: Keil MDK or ARM Development Studio

## Quick Start

### 1. Clone Project
```bash
git clone --recursive https://github.com/Nowaterisenough/EmbeddedTemplate.git
```

### 2. Configure Environment Variables

**Method 1: System Environment Variables (Recommended)**

Add to system environment variables:
```
ARM_CLANG_PATH=C:\path\to\ArmCompiler6.xx\bin
ARM_GCC_PATH=C:\path\to\arm-gnu-toolchain\bin
```

**Method 2: Command Line Temporary Setup**

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

**Method 3: VS Code Workspace Configuration**

Configure in `.vscode/settings.json` (VS Code only):
```json
{
  "cmake.configureEnvironment": {
    "ARM_CLANG_PATH": "C:/path/to/ArmCompiler6.xx/bin",
    "ARM_GCC_PATH": "C:/path/to/arm-gnu-toolchain/bin"
  }
}
```

**Debug Tools Configuration (Optional)**

For debugging in VS Code, add to `.vscode/settings.json`:
```json
{
  "cortex-debug.gdbPath": "C:/path/to/arm-gnu-toolchain/bin/arm-none-eabi-gdb.exe",
  "cortex-debug.objdumpPath": "C:/path/to/arm-gnu-toolchain/bin/arm-none-eabi-objdump.exe",
  "cortex-debug.JLinkGDBServerPath": "C:/Program Files/SEGGER/JLink_Vxxx/JLinkGDBServerCL.exe"
}
```

### 3. Select Build Configuration
In VS Code:
1. Open Command Palette (Ctrl+Shift+P)
2. Select `CMake: Select Configure Preset`
3. Choose target configuration:
   - `h743-gcc` - H743 Board + ARM GCC
   - `h743-clang` - H743 Board + ARM Clang
   - `f407-gcc` - F407 Board + ARM GCC
   - `f407-clang` - F407 Board + ARM Clang

### 4. Build Project
Press F7 or run:
```bash
cmake --build build
```

### 5. Output Files
After building, find in `build/bin/<board>/` directory:
- `*.elf` - Executable file
- `*.bin` - Binary image
- `*.hex` - Intel HEX format
- `*.map` - Memory map file

## Project Structure

```
EmbeddedTemplate/
├── 3rd/                    # Third-party libraries
│   └── RTT/                # SEGGER RTT debug library
├── app/                    # Application code
├── boards/                 # Board support packages
│   ├── stm32f407zg/       # STM32F407ZG board
│   └── stm32h743zi/       # STM32H743ZI board
├── cmake/                  # CMake toolchain files
│   └── toolchains/
│       ├── arm-none-eabi-gcc.cmake
│       └── arm-none-eabi-clang.cmake
├── drivers/                # Hardware abstraction layer
│   ├── STM32F4/           # STM32F4 HAL
│   └── STM32H7/           # STM32H7 HAL
├── include/               # Public header files
├── middleware/            # Middleware
├── modules/               # Function modules
└── tests/                 # Test code
```

## Debugging

The project integrates SEGGER RTT for real-time debug output:

```c
#include "SEGGER_RTT.h"

// Use RTT output
SEGGER_RTT_printf(0, "Hello RTT!\n");

// Or use redirected printf
printf("Debug: value = %d\n", value);
```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
