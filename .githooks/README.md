# Git Hooks

本目录包含项目的 Git hooks 脚本。

## 安装

在克隆仓库后，运行以下命令安装 hooks：

```bash
bash scripts/setup-hooks.sh
```

或者手动配置 Git 使用此目录：

```bash
git config core.hooksPath .githooks
```

## 可用的 Hooks

### pre-commit

在提交前自动运行 clang-format 格式化所有待提交的 C/C++ 代码文件。

**要求：**
- 系统中安装了 `clang-format` 命令

**行为：**
- 自动格式化所有待提交的 `.c`、`.h`、`.cpp`、`.hpp` 文件
- 排除 STM32Cube 和第三方库文件
- 如果未找到 clang-format，会显示警告但仍允许提交

## 注意事项

- Hooks 不会自动生效，需要手动安装
- 如果修改了 `.githooks` 中的脚本，需要重新运行安装脚本
