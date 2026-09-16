# 架构设计及约束

## 架构设计

### 设计原则

| 原则 | 描述 | 理由 |
| ---- | ---- | ---- |
| 事件驱动 | 以 functionMap 映射表驱动事件路由，处理逻辑与分发逻辑解耦 | 新增事件仅需注册映射条目，分发逻辑无需修改 |
| 配置驱动 | 调频策略参数由 PluginMgr 配置定义，支持云端热更新 | 策略调整无需重新编译，产品差异化快速响应 |

### 逻辑架构

```plantuml
@startuml
skinparam componentStyle rectangle

package "事件分发层" {
    [functionMap\n(ResType → 处理函数)] as Dispatcher
    [cmdidPolicy_\n((resType,value) → (cmdId,onOff))] as Policy
    [resTypes / resTypeWithVal_\n(全量/精确订阅)] as Subscribe
}

package "配置管理层" {
    [PluginMgr 配置加载] as ConfigLoad
    [HandleRssCloudConfigUpdate\n(云端热更新)] as CloudUpdate
}

package "状态管理层" {
    [焦点应用 / 游戏状态] as AppState
    [屏幕状态 / 设备模式] as ScreenState
    [电池限频 / 性能模式] as PowerState
}

package "调频输出层" {
    [SocPerfClient\n(SA 1906)] as Output
}

package "工具层" {
    [Utils\n(应用名/类型解析、限频)] as Utils
}

Dispatcher --> Policy : 优先匹配
Dispatcher --> Subscribe : 事件过滤
Dispatcher --> AppState : 读写状态
Dispatcher --> ScreenState : 读写状态
Dispatcher --> Output : PerfRequest / PerfRequestEx / LimitRequest
ConfigLoad --> Policy : 加载策略表
ConfigLoad --> AppState : 初始化状态
CloudUpdate --> ConfigLoad : 增量更新
Dispatcher --> Utils : 限频应用
@enduml
```

### 非功能设计

| 场景类别 | 方案设计 |
| ---- | ---- |
| 连续拖拽节流 | 两次拖拽调频命令最小间隔 4500ms |
| 编译裁剪 | 通过特性宏控制功能编译路径，未开启特性的代码不编译 |
