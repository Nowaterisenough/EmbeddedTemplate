# EmbeddedTemplate

[中文](README.md) | **English**

A modern, modular STM32 embedded development template with multi-toolchain and multi-board support, based on CMake build system.

## Features

- **Multi-toolchain Support**: ARM GCC / ARM Clang (Keil)
- **Multi-board Support**: STM32F407ZG, STM32H743ZI
- **Data-driven Design**: Unified board API with zero conditional branches
- **Automatic Versioning**: Semantic versioning based on Conventional Commits
- **Modern Build System**: CMake + Ninja with VS Code integration
- **Debug Support**: SEGGER RTT real-time logging

## Supported Hardware

| Board | MCU | Architecture | Toolchain |
|-------|-----|--------------|-----------|
| STM32F407ZG | STM32F407ZGT6 | Cortex-M4 | GCC, Clang |
| STM32H743ZI | STM32H743ZIT6 | Cortex-M7 | GCC, Clang |

## Quick Start

### 1. Clone Repository
```bash
git clone --recursive https://github.com/Nowaterisenough/EmbeddedTemplate.git
cd EmbeddedTemplate
```

### 2. Configure Toolchain

Set environment variables (choose one method):

```powershell
# Windows PowerShell
$env:ARM_GCC_PATH="C:\path\to\arm-gnu-toolchain\bin"
$env:ARM_CLANG_PATH="C:\path\to\ArmCompiler6.xx\bin"  # Optional
```

```bash
# Linux/macOS
export ARM_GCC_PATH="/path/to/arm-gnu-toolchain/bin"
export ARM_CLANG_PATH="/path/to/ArmCompiler6.xx/bin"  # Optional
```

### 3. Build

```bash
# Select build configuration
cmake --preset h743-gcc          # H743 + GCC
cmake --preset f407-gcc          # F407 + GCC

# Build
cmake --build build

# Output files in: build/bin/<board>/
```

### 4. Firmware Version Info

The project supports automatic version management with version numbers embedded in firmware:

```bash
# View firmware version (requires pyelftools)
python tools/extract_version.py build/bin/stm32h743zi/EmbeddedTemplate.elf
```

## Project Structure

```
EmbeddedTemplate/
├── 3rd/                    # Third-party libraries (SEGGER RTT)
├── app/                    # Application code
├── boards/                 # Board support packages
│   ├── stm32f407zg/
│   └── stm32h743zi/
├── cmake/                  # CMake toolchain files
├── drivers/                # STM32 HAL drivers
├── modules/                # Functional modules (scheduler, hylink)
└── tools/                  # Build tools
```

## Development Guide

### Unified Board API

All boards share the same interface:

```c
#include "board.h"

int main(void) {
    board_init();               // Initialize system
    board_led_init();           // Initialize LEDs

    while (1) {
        board_led_toggle(BOARD_LED_1);
        board_delay_ms(1000);
    }
}
```

**API List**:
- `board_init()` - Initialize clock and peripherals
- `board_led_set(led, state)` - Set LED state
- `board_led_toggle(led)` - Toggle LED
- `board_delay_ms(ms)` - Millisecond delay
- `board_millis()` - Get system uptime

### Adding New Board

1. Copy `boards/stm32h743zi/` directory to `boards/<your-board>/`
2. Modify LED mapping table in `board_config.h`:

```c
#define BOARD_LED_MAP { \
    {GPIOH, GPIO_PIN_10, __HAL_RCC_GPIOH_CLK_ENABLE}, /* LED1 */ \
    {GPIOH, GPIO_PIN_11, __HAL_RCC_GPIOH_CLK_ENABLE}, /* LED2 */ \
    {GPIOH, GPIO_PIN_12, __HAL_RCC_GPIOH_CLK_ENABLE}, /* LED3 */ \
}
```

3. Add build configuration in `CMakePresets.json`
4. Build and test: `cmake --preset <your-board>-gcc`

### Data-driven Design

The project follows a **data structure first** design philosophy, avoiding hardcoded conditional branches:

```c
// ❌ Avoid: Hardcoded switch-case
switch (led) {
    case LED1: HAL_GPIO_WritePin(GPIOH, GPIO_PIN_10, state); break;
    case LED2: HAL_GPIO_WritePin(GPIOH, GPIO_PIN_11, state); break;
}

// ✅ Recommended: Mapping table driven, zero branches
static const led_map_t led_map[] = BOARD_LED_MAP;
HAL_GPIO_WritePin(led_map[led].port, led_map[led].pin, state);
```

### Debug Output

Use SEGGER RTT for real-time debugging:

```c
#include "SEGGER_RTT.h"

SEGGER_RTT_printf(0, "Debug: value = %d\n", value);
```

## Version Management

The project follows [Conventional Commits](https://www.conventionalcommits.org/) specification:

```bash
# Commit format
<type>(<scope>): <subject>

# Examples
feat(led): add RGB LED driver
fix(uart): fix receive buffer overflow
```

**Version Rules**:
- `feat:` → MINOR version +1 (v1.0.0 → v1.1.0)
- `fix:` → PATCH version +1 (v1.1.0 → v1.1.1)
- MAJOR version requires manual update in [.github/workflows/auto-release.yml](.github/workflows/auto-release.yml#L40)

See [commit convention documentation](.github/COMMIT_CONVENTION.md) for details.

## Manual Release

```bash
# Interactive release
./scripts/release.sh              # Linux/macOS
.\scripts\release.ps1             # Windows

# Release with specific version
./scripts/release.sh v1.2.3
```

## VS Code Configuration

### Required Extensions
- **CMake Tools** - Build system integration
- **Cortex-Debug** (Optional) - Hardware debugging support

### Basic Configuration

Configure toolchain paths in `.vscode/settings.json`:

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

### Hardware Debug Configuration

Add debug configurations in `.vscode/launch.json` (requires J-Link debugger):

**STM32H743ZI Debug Configuration:**
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

**STM32F407ZG Debug Configuration:**
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

**Debug Prerequisites:**
- Install Cortex-Debug extension
- Configure toolchain paths (see basic configuration above)
- Connect J-Link debugger to target board

## License

MIT License - See [LICENSE](LICENSE) for details

## Resources

- [CMake Build System](https://cmake.org/documentation/)
- [Conventional Commits Specification](https://www.conventionalcommits.org/)
- [SEGGER RTT Documentation](https://www.segger.com/products/debug-probes/j-link/technology/about-real-time-transfer/)
