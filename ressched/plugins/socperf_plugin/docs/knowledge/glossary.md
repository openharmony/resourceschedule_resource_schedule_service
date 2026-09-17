# 术语表

## 弱交互（WeakAction）

无主动调频请求时延迟切换到低频策略的机制。当前台应用命中关键应用（`keyAppName_` 需签名校验通过，或 `keyAppType_` 匹配关键应用类型），或自定义游戏状态激活时，弱交互被关闭以维持高频。

## 游戏加速（GameBoost）

针对游戏场景的多级调频加速。`socperfGameBoostSwitch_` 为功能开关，前台应用全为游戏类型（`APP_TYPE_GAME = 2`）时激活，抑制常规点击/滑动/按键事件，按 LEVEL0~LEVEL3 下发不同级别调频命令。另有第三方游戏框架通过 `RES_TYPE_REPORT_GAME_STATE_CHANGE` 上报的自定义游戏状态（`custGameState`），激活时同样抑制弱交互和点击/滑动事件，但由外部驱动而非插件内部判断。

## 显示模式（DisplayMode）

屏幕显示状态的调频策略分类：全屏（`displayFull`）、主屏（`displayMain`）、全局全屏（`displayGlobalFull`）。全局全屏下按设备方向（横屏/竖屏）下发不同调频命令，仅在亮屏时生效。

## 电池限频（BatteryLimit）

按电量阈值和充电状态动态控制频率限制。`socperfBatteryConfig_` 存储各电量阈值的限频配置，低电量放电时通过 `PowerLimitBoost` + `LimitRequest` 下发限频，充电或电量恢复时解除。
