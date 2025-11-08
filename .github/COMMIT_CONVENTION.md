# Commit Message 规范

本项目遵循 [Conventional Commits](https://www.conventionalcommits.org/) 规范，用于自动化版本管理和生成 Changelog。

## 格式

```
<type>(<scope>): <subject>

[optional body]

[optional footer(s)]
```

### 示例

```
feat(led): 添加 RGB LED 呼吸灯效果
fix(uart): 修复高波特率下数据丢失问题
perf(dma): 优化 DMA 传输效率，减少 CPU 占用
refactor(app): 重构主循环代码结构
docs(readme): 更新开发环境配置说明
```

## Type 类型

| Type | 说明 | 版本影响 |
|------|------|---------|
| `feat` | 新功能 | MINOR +1 |
| `fix` | Bug修复 | PATCH +1 |
| `perf` | 性能优化 | PATCH +1 |
| `refactor` | 代码重构（不影响功能） | PATCH +1 |
| `docs` | 文档更新 | PATCH +1 |
| `style` | 代码格式调整 | PATCH +1 |
| `test` | 测试相关 | PATCH +1 |
| `build` | 构建系统或依赖更新 | PATCH +1 |
| `ci` | CI/CD 配置修改 | PATCH +1 |
| `chore` | 其他杂项修改 | PATCH +1 |

## Scope 范围（可选）

建议使用的 scope：

| Scope | 说明 |
|-------|------|
| `led` | LED 控制相关 |
| `uart` | 串口通信 |
| `spi` | SPI 总线 |
| `i2c` | I2C 总线 |
| `dma` | DMA 传输 |
| `timer` | 定时器 |
| `rtos` | FreeRTOS 相关 |
| `app` | 应用层代码 |
| `hal` | HAL 层适配 |
| `build` | 构建系统 |
| `ci` | CI/CD |
| `docs` | 文档 |

## Subject 主题

- 使用祈使句，现在时态："添加"而非"已添加"或"添加了"
- 首字母小写
- 结尾不加句号
- 简明扼要（建议不超过 50 字符）

### 好的示例

```
feat(uart): 添加 DMA 收发支持
fix(timer): 修复定时器溢出计算错误
perf(spi): 优化批量传输性能
```

### 不好的示例

```
feat(uart): 添加了 DMA 收发支持。         # 使用了"添加了"过去式，且有句号
fix: 修复Bug                              # 太模糊，没说明具体修复了什么
Update code                               # 没有 type，太笼统
```

## Breaking Changes（破坏性变更）

如果引入了不兼容的 API 修改，需要标注 `BREAKING CHANGE`：

### 方法 1: 在 type 后添加 `!`

```
feat(api)!: 修改串口初始化接口参数

BREAKING CHANGE: uart_init() 现在需要传入配置结构体指针
```

### 方法 2: 在 footer 中说明

```
feat(api): 重构 GPIO 接口

BREAKING CHANGE:
- gpio_set() 改为 gpio_write()
- gpio_get() 改为 gpio_read()
```

**注意**: 检测到 `BREAKING CHANGE` 后，系统会提示建议手动升级 MAJOR 版本号。

## 自动化版本规则

### 版本号格式

```
vMAJOR.MINOR.PATCH
```

### 自动递增规则

| Commit 类型 | 版本变化 | 示例 |
|------------|---------|------|
| `feat:` | MINOR +1, PATCH 归 0 | v1.0.5 → v1.1.0 |
| `fix:` 或其他 | PATCH +1 | v1.1.0 → v1.1.1 |
| BREAKING CHANGE | 提示升级 MAJOR | 需手动修改 workflow |

### MAJOR 版本升级

MAJOR 版本需要手动在 [auto-release.yml](../workflows/auto-release.yml#L40) 中修改：

```yaml
# 配置区：MAJOR 版本号（手动维护）
MAJOR=1  # ← 修改这里
```

修改后，下一次发布会自动重置为 `vMAJOR.0.0`。

## 工作流程示例

### 场景 1: 添加新功能

```bash
# 当前版本: v1.0.5

# 开发并提交
git commit -m "feat(led): 添加 WS2812 RGB 灯带驱动"
git push origin main

# 自动触发发布: v1.1.0
```

### 场景 2: 修复 Bug

```bash
# 当前版本: v1.1.0

# 修复并提交
git commit -m "fix(uart): 修复接收缓冲区溢出问题"
git push origin main

# 自动触发发布: v1.1.1
```

### 场景 3: 重大更新（需升级 MAJOR）

```bash
# 当前版本: v1.5.2

# 1. 修改 .github/workflows/auto-release.yml
#    MAJOR=1 改为 MAJOR=2

# 2. 提交破坏性变更
git commit -m "feat(api)!: 重构核心 API 接口

BREAKING CHANGE:
- 修改所有外设初始化接口
- 不兼容 v1.x 版本"

git push origin main

# 自动触发发布: v2.0.0
```

### 场景 4: 文档更新（不触发新版本？）

```bash
# 文档更新也会触发 PATCH 版本递增
git commit -m "docs(readme): 更新硬件连接示意图"
git push origin main

# 触发发布: v1.1.2
```

如果**不想**文档更新触发发布，可以使用 `[skip ci]`：

```bash
git commit -m "docs: 更新注释 [skip ci]"
```

## 最佳实践

### 1. 每次提交只做一件事

不好：
```
feat: 添加LED驱动，修复UART bug，更新文档
```

好：
```
feat(led): 添加 WS2812 驱动支持
fix(uart): 修复接收超时处理逻辑
docs(led): 添加 WS2812 使用示例
```

### 2. 提供清晰的上下文

不好：
```
fix: 修复错误
```

好：
```
fix(dma): 修复 SPI DMA 传输时内存对齐问题

- 添加缓冲区对齐检查
- 对非对齐数据使用临时缓冲
```

### 3. 使用 Body 提供详细信息

```
feat(rtos): 添加任务监控统计功能

实现了以下功能：
- 实时 CPU 使用率统计
- 任务运行时间记录
- 栈使用情况监控

相关 Issue: #123
```

### 4. 引用 Issue

```
fix(uart): 修复波特率计算精度问题

Fixes #42
```

## 工具支持

### 本地 Commitlint 检查（推荐）

安装依赖：

```bash
npm install --save-dev @commitlint/cli @commitlint/config-conventional
```

创建配置文件 `commitlint.config.js`：

```javascript
module.exports = {
  extends: ['@commitlint/config-conventional']
}
```

配置 Git Hook（需要 husky）：

```bash
npm install --save-dev husky
npx husky install
npx husky add .husky/commit-msg 'npx --no -- commitlint --edit "$1"'
```

### VSCode 插件

推荐安装：
- [Conventional Commits](https://marketplace.visualstudio.com/items?itemName=vivaxy.vscode-conventional-commits)

## 参考资源

- [Conventional Commits 官方文档](https://www.conventionalcommits.org/)
- [Angular Commit Guidelines](https://github.com/angular/angular/blob/main/CONTRIBUTING.md#commit)
- [Semantic Versioning](https://semver.org/)

---

**有问题？** 查看项目的 [自动发布工作流配置](../workflows/auto-release.yml)
