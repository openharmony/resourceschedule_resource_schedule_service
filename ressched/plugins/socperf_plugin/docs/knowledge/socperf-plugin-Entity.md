# 业务实体知识

socperf_plugin 模块核心业务实体的完整知识定义，涵盖静态概念、动态运行、规则约束与代码映射。

## 弱交互状态（WeakAction）

### 实体概念

| 子维度 | 内容 |
| --- | --- |
| 实体名称 | 弱交互状态 / WeakAction |
| 实体定义 | 无主动调频请求时延迟切换到低频策略的机制，通过设备模式 `actionmode:weakaction` 控制 SocPerfClient 的频率策略 |
| 核心特征 | `weakActionEnable_` 为总开关，`weakActionStatus_` 为当前生效状态；前台应用命中关键应用或自定义游戏状态激活时关闭弱交互以维持高频 |

### 上下文与场景

| 子维度 | 内容 |
| --- | --- |
| 使用场景 | 非关键应用前台时启用弱交互降频节省功耗；关键应用或游戏前台时关闭弱交互维持高性能 |
| 状态流转 | 焦点应用变更 → 遍历关键应用名（需签名校验通过）和关键应用类型 → 命中则弱交互=false → 全部未命中且自定义游戏状态为 false 则弱交互=weakActionEnable_ → 状态变化时下发 RequestDeviceMode |

### 规格与约束

| 子维度 | 内容 |
| --- | --- |
| 业务规则 | 关键应用名需签名校验通过才生效；自定义游戏状态为 true 时强制弱交互=false；弱交互状态变化时才下发设备模式命令，避免冗余下发 |

### 知识关联

| 子维度 | 内容 |
| --- | --- |
| 关联实体 | 关键应用名集合（keyAppName_）、关键应用类型集合（keyAppType_）、自定义游戏状态（custGameState_）、焦点应用集合（focusAppUids_，遍历判断关键应用） |

### 代码与符号

| 子维度 | 内容 |
| --- |--- |
| 核心成员 | `weakActionEnable_`、`weakActionStatus_`、`keyAppName_`、`keyAppType_`（`include/socperf_plugin.h`） |
| 核心方法 | `UpdateWeakActionStatus()`、`SetWeakActionEnable()`（`src/socperf_plugin.cpp`） |

---

## 游戏加速（GameBoost）

### 实体概念

| 子维度 | 内容 |
| --- | --- |
| 实体名称 | 游戏加速 / GameBoost |
| 实体定义 | 针对游戏场景的多级调频加速机制，激活后抑制常规点击/滑动/按键事件分发，按等级下发不同调频命令 |
| 核心特征 | `socperfGameBoostSwitch_` 为功能开关；`isFocusAppsGameType_` 标记前台应用是否全为游戏类型（APP_TYPE_GAME=2）；支持 LEVEL0~LEVEL3 四级加速 |
| 概念对比 | 游戏加速 vs 自定义游戏状态：前者由插件内部判断前台应用类型驱动，后者由外部游戏框架通过事件上报驱动 |

### 上下文与场景

| 子维度 | 内容 |
| --- | --- |
| 使用场景 | 游戏前台运行时抑制通用交互调频，避免与游戏自身的调频通路冲突 |
| 状态流转 | 焦点应用变更 → 查询应用类型 → 全部为游戏则 isFocusAppsGameType_=true → 点击/滑动/按键事件被抑制 → 收到游戏加速事件 → 按 LEVEL0~3 下发对应调频命令 |

### 规格与约束

| 子维度 | 内容 |
| --- | --- |
| 业务规则 | 游戏加速开关关闭或事件数据为空时跳过；自定义游戏状态为 true 时也抑制交互事件；事件来源进程为游戏（通过 PID 查应用类型）时同样抑制 |

### 知识关联

| 子维度 | 内容 |
| --- | --- |
| 关联实体 | 焦点应用集合、UID 到应用类型映射、PID 到应用类型映射、自定义游戏状态（custGameState_，两者共同决定是否抑制交互事件） |

### 代码与符号

| 子维度 | 内容 |
| --- | --- |
| 核心成员 | `socperfGameBoostSwitch_`、`isFocusAppsGameType_`、`custGameState_`、`focusCustGameUids_`（`include/socperf_plugin.h`） |
| 核心方法 | `HandleGameBoost()`、`UpdatesFocusAppsType()`、`IsFocusAppsAllGame()`、`UpdateCustGameState()`（`src/socperf_plugin.cpp`） |

---

## 显示模式（DisplayMode）

### 实体概念

| 子维度 | 内容 |
| --- | --- |
| 实体名称 | 显示模式 / DisplayMode |
| 实体定义 | 屏幕显示状态的调频策略分类，包括全屏（displayFull）、主屏（displayMain）、全局全屏（displayGlobalFull） |
| 核心特征 | 全局全屏模式下按设备方向（横屏 displayLandscape / 竖屏 displayPortrait）下发不同调频命令；仅在亮屏状态下生效；模式切换时需先关闭旧模式再开启新模式 |

### 上下文与场景

| 子维度 | 内容 |
| --- | --- |
| 交互流程 | 设备模式状态事件 → 校验负载模式类型与值 → 下发 RequestDeviceMode → 若为 display 类型且亮屏则执行显示模式调频（停止旧模式 + 启动新模式） |

### 规格与约束

| 子维度 | 内容 |
| --- |--- |
| 业务规则 | 设备模式类型必须为 display 且屏幕状态为亮屏才执行显示模式调频；全局全屏需结合设备方向判断横屏或竖屏；模式值为 full/globalFull/main 时额外下发屏幕切换调频 |

### 知识关联

| 子维度 | 内容 |
| --- | --- |
| 关联实体 | 屏幕状态（screenStatus_）、设备方向（deviceOrientation_）、设备模式值（deviceMode_，记录当前显示模式） |

### 代码与符号

| 子维度 | 内容 |
| --- | --- |
| 核心成员 | `deviceMode_`、`deviceOrientation_`、`screenStatus_`（`include/socperf_plugin.h`） |
| 核心方法 | `HandleDeviceModeStatusChange()`、`HandleSceenModeBoost()`、`HandleDeviceOrientationStatusChange()`（`src/socperf_plugin.cpp`） |

---

## 电池限频（BatteryLimit）

### 实体概念

| 子维度 | 内容 |
| --- | --- |
| 实体名称 | 电池限频 / BatteryLimit |
| 实体定义 | 按电池电量阈值和充电状态动态控制频率限制的机制，低电量放电时限频，充电或电量恢复时解除 |
| 核心特征 | `socperfBatteryConfig_` 按电量百分比存储限频配置（tags + configs）；通过 PowerLimitBoost + LimitRequest 双 API 联动；`lastBatteryLimitCap_` 记录上次限频电量避免重复触发 |

### 上下文与场景

| 子维度 | 内容 |
| --- | --- |
| 交互流程 | 电池状态变更事件 → 校验充电状态 → 充电中/充电完成则解除限频 → 放电状态按电量匹配最接近阈值 → 下发 PowerLimitBoost + LimitRequest |
| 状态流转 | 未限频（lastBatteryLimitCap_=-1）→ 低电量放电 → 限频（记录阈值）→ 充电/电量恢复 → 解除限频（清除记录） |

### 规格与约束

| 子维度 | 内容 |
| --- | --- |
| 业务规则 | 充电中或充电完成解除限频；放电时电量超过最大阈值则解除；限频阈值与上次相同时不重复下发；从未限频时不执行解除 |

### 知识关联

| 子维度 | 内容 |
| --- | --- |
| 关联实体 | socperfBatteryConfig_（电量阈值 → 限频配置映射）、PowerLimitBoost（功耗限频开关）、LimitRequest（限频请求） |

### 代码与符号

| 子维度 | 内容 |
| --- | --- |
| 核心成员 | `socperfBatteryConfig_`、`maxBatteryLimitCapacity_`、`lastBatteryLimitCap_`（`include/socperf_plugin.h`） |
| 核心方法 | `InitBatteryConfig()`、`HandleBatteryStatusChange()`（`src/socperf_plugin.cpp`） |
| 核心类型 | `Frequencies`（tags + configs）、`BatteryChargeState`（`include/socperf_plugin.h`） |
