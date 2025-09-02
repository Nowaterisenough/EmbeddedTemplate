# CHANGELOG

## Unreleased (2024-11-07)

### Features
- 更新航线规划界面 by @lidi in 52d34c1 8f10d73 b00c38e
- 更新PID调参的布局 by @lidi in 076e280
- 横向带状仪表 by @jiaofupeng in fa4e53a
- 仪表盘组件 by @jiaofupeng in a9b70c2
- 电池组件 by @jiaofupeng in c4d0ba9
- 故障灯组件 by @jiaofupeng in 68747e4
- 电池管理界面1.0 by @jiaofupeng in 1668715
- 电机监控模块/电池监控模块 1.0 提交 by @jiaofupeng in 2955ccd
- 电池块数据初始化 by @jiaofupeng in 8326601
- 惯导添加空白页 by @jiaofupeng in 3e787ea

### Bug Fixes
- 修复字体表现不一致的问题 by @lidi in 8ce359a
- 修复插入新航点闪烁的问题 by @lidi in be3258f
- PID问题修复 by @lidi in 1bbd52a
- 航线规划下拉框问题修复 by @lidi in 6563c84
- 修复连接问题 by @lidi in 26d19fe
- 舵机界面更换飞机图片 by @jiaofupeng in 48263b6

### Documentation
- 更新版本号 by @lidi in 65845c4

## 0.1.1008.0834 (2024-10-08)

### Features
- 更新三方组件 by @lidi in 90a9e8f

## 0.1.0913.1724 (2024-09-13)

### Features
- 舵机界面样式修改 by @jiaofupeng in 5bb6083
- 调用电压转电量并显示 by @jiaofupeng in fd5b256
- 任务规划单航点信息列表显示 by @jiaofupeng in 1687efe
- 航迹功能模块提交 by @jiaofupeng in 2207471
- 舵机PID添加副翼、升降、方向，调参界面参数设置，添加赋值按钮 by @jiaofupeng in b7ef816
- pid结构体修改 by @jiaofupeng in cd73403

### Bug Fixes
- 消除 event 告警 by @jiaofupeng in 70f0d25
- 修复CHANGELOG生成错误 by @lidi in 65b6290 c0b0df4
- 处理任务规划界面航点拖动bug by @jiaofupeng in 79bb807

### Documentation
- 更新版本 by @lidi in fa83d02

### Refactor
- 重构PID调参界面 by @lidi in 4c13182

## 0.1.0828.1806 (2024-08-28)

### Bug Fixes
- 修复CHANGELOG生成错误 by @lidi in 5d4b64e 794ea59 3ebe656 419c155 36a954c 609da31 9e1a1b1 0304ad7 38a6ecb 7b7105e 9956c92 1238b30 14a13ad b8da677 5f05774 02841cb 1f37953 90a662b b367ef0

### Others
- 更新版本号 by @lidi in bbee0c5 0bfca99

## 0.1.0828.1655 (2024-08-28)

### Features
- PID界面 Apply 按钮设置定时器 by @jiaofupeng in 6c1f40f
- 更新hooks脚本 by @lidi in a36772c
- StepProgressBar 步骤进度条（飞前检查进度条）组件 by @jiaofupeng in a656fd9
- SwitchButton 开关按钮组件 by @jiaofupeng in 3e72dbb
- UploadSlider 任务上传滑动条组件 by @jiaofupeng in 01884c9
- qml 去掉CustomControls 1.0引入 by @jiaofupeng in 8b7f951
- 引入并测试 UploadSlider、SwitchButton、stepProgressBar组件 by @jiaofupeng in 76d9e7e
- 新增轨迹点绘制 by @lidi in 8194b0f
- 轨迹点新增序号 by @lidi in 268116c
- 新增拖拽地图和缩放地图的功能 by @lidi in 321189a
- 新增电池电量算法 by @lidi in 5401a2a
- 新增轨迹点信息弹窗 by @lidi in ba71cc1
- 更新H L起飞点降落点标识 by @lidi in 3f2887a
- 重构地图弹窗，优化轨迹点管理 by @lidi in f165fab
- 更新断开H L点逻辑 by @lidi in bf9efc8
- 双边滑动条组件 by @jiaofupeng in f204617
- PID界面参数设置记忆功能 by @jiaofupeng in 3ea2d38
- PID参数设置记忆功能优化，设置项与回传数据对照apply功能 by @jiaofupeng in d19cef7
- 新增多任务轨迹规划 by @lidi in 65c04f4
- 更新航线规划模块 by @lidi in 0e2ed4f a79487f 44dab6d 11bc99b
- MFD数据调用 by @jiaofupeng in 67c7113
- control模块组件提交 by @jiaofupeng in 8b03b73
- 组件调用 by @jiaofupeng in b12e321

### Bug Fixes
- PID曲线图例点击线条变化 by @jiaofupeng in aaedf41
- SwitchButton、StepProgressBar、UploadSlider模块变量命名规范 by @jiaofupeng in e7907fa
- 修复拖动轨迹点时，midmarker位置不正确的问题 by @lidi in 359641e
- 修复编辑按钮第二次checked后不能触发midmarker的问题 by @lidi in 515046b
- 修复报错问题 by @lidi in a0d2802
- 优化StepProgressBar组件 by @jiaofupeng in 8cfc102
- 去除无用代码 by @lidi in 6f97aae
- 修复merge不符合规范的约束 by @lidi in fc4d42c
- 双向进度条、任务上传组件优化 by @jiaofupeng in 2589684
- PID bug修复 by @jiaofupeng in 09eedda

### Performance Improvements
- 调整TCP和串口的接收缓存区大小，优化性能，避免丢帧 by @lidi in a1519cf

### Refactor
- 模块化 by @lidi in 5edee2f
- 重构地图界面 by @lidi in 1a9076b

### Others
- 修复文件大小写 by @lidi in 600ac27 ed53cbb
- 删除注释掉的无用代码 by @lidi in 5fa5bcb

## 0.1.0813.1144 (2024-08-12)

### Features
- 初始化代码仓 by @lidi in 6277cb4
- 舵机界面重构 by @jiaofupeng in ee5d1d9
- 新增打包脚本 by @lidi in 188c9bb
- 数传数据处理，HUD引入 by @jiaofupeng in 886e817
- 新增信号状态 by @lidi in 1840ced
- 状态栏新增带宽显示 by @lidi in 1282736
- 新增标签管理脚本与版本号自动更新功能 by @lidi in aa4a07e
- 数据记录仪PID数据写入.csv文件 by @jiaofupeng in 3dde51f
- 添加数据记录仪解析界面 by @jiaofupeng in 4c463e7
- 主界面 '更多' 按钮菜单添加图标，IconPushButton组件类接口更改 by @jiaofupeng in a318ea1
- 新增航点计划功能，实现航点添加、删除、编辑等功能 by @lidi in 2d567b6
- 更新AppGlobal.hpp枚举常量名称，提高代码可读性 by @lidi in 207ef97
- 修复串口错误处理，优化数据接收逻辑 by @lidi in 7ce7e35

### Bug Fixes
- 告警修复 by @jiaofupeng in c47489c
- 数据链数据处理，状态卡片初始化处理 by @jiaofupeng in 0ffef9d
- 修复通信卡顿问题 by @lidi in 1fe887e
- 添加头文件 by @jiaofupeng in f8db298
- 无线测距接收值处理，数据记录仪功能模块化 by @jiaofupeng in a430cf4
- 修复协议问题 by @lidi in ddc276e
- 修复点击不展开的问题 by @lidi in fd50aa9

### Performance Improvements
- 优化协议性能 by @lidi in b83a5ec

### Refactor
- 重构地图模块，以鼠标位置为中心缩放 by @lidi in 44172a7

### Others
- Initial commit by @lidi in 6f68d03
- 调整目录结构 by @lidi in 6c1a55d
- 添加MFD模块到构建配置文件 by @lidi in 03ebf2a
- 更新clang format by @lidi in cd4ce39
- 更新格式 by @lidi in ce49ec6

