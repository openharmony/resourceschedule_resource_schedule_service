# socperf_plugin 模块 - AI 编码代理指南

## 基本信息

socperf_plugin 是资源调度服务的调频插件，将系统事件（应用启动、窗口焦点、亮灭屏、滑动、点击、Web 手势、游戏加速、电池状态、设备模式切换等约 40 种）转换为 SoC 调频命令，通过 `SocPerfClient`（SA 1906）控制 CPU/GPU/DDR 频率。插件由 `ressched` 服务（SA 1901）通过 `dlopen` 加载，导出 `OnPluginInit`、`OnPluginDisable`、`OnDispatchResource` 三个 C 入口。

## 架构及依赖

### 核心架构

采用**事件驱动 + 配置驱动**架构，继承 `Plugin` 基类实现 `Init`/`Disable`/`DispatchResource`：

- **事件分发**：`functionMap`（`unordered_map<uint32_t, function<void(ResData)>>`）将 `ResType` 映射到处理函数 Lambda，未匹配的事件被静默忽略
- **双订阅模型**：`resTypes` 全量订阅、`resTypeWithVal_` 按 `(resType, value)` 精确订阅
- **配置管理**：策略参数从 `PluginMgr` 配置加载，支持运行时云端热更新（`HandleRssCloudConfigUpdate`）
- **调频接口**：`PerfRequest`（一次性）、`PerfRequestEx`（开始/停止对）、`RequestDeviceMode`（模式切换）、`LimitRequest`（限频）、`PowerLimitBoost`（功耗限频加速）

## 目录结构

```
socperf_plugin/
├── include/
│   ├── socperf_plugin.h              # SocPerfPlugin 类、AppKeyMessage/Frequencies/BatteryChargeState/PerformanceMode 类型
│   ├── common_define.h               # 字符串常量、PERF_REQUEST_CMD_ID_* 命令 ID 常量
│   └── utils.h                       # Utils 静态工具类
├── src/
│   ├── socperf_plugin.cpp            # 全部插件逻辑
│   └── utils.cpp                     # 应用名/类型解析、策略模式提取、限频应用
├── test/
│   ├── unittest/soc_perf_plugin_test.cpp   # GTest 单元测试
│   └── fuzztest/socperfplugin_fuzzer/      # LibFuzzer 模糊测试
├── BUILD.gn                          # socperf_plugin（动态库）+ socperf_plugin_static（静态库）
└── libsocperf_plugin.versionscript   # 仅导出 OnPluginInit/OnPluginDisable/OnDispatchResource
```

## 知识索引

| 场景 | 先读 |
|------|------|
| 理解业务术语 | docs/knowledge/glossary.md |
| 了解业务背景 | docs/knowledge/business-context.md |
| 架构设计 | docs/knowledge/architecture.md |
| 核心技术机制 | docs/knowledge/socperf-plugin-Technology.md |
| 核心业务实体 | docs/knowledge/socperf-plugin-Entity.md |
| 编码前必读 | docs/knowledge/coding-standards.md |

## 新增事件处理函数

1. `include/common_define.h` 添加 `PERF_REQUEST_CMD_ID_*` 常量
2. `include/socperf_plugin.h` private 区域声明处理函数
3. `src/socperf_plugin.cpp` 实现处理函数（校验 data → 匹配 value → 调用 SocPerfClient）
4. `InitFunctionMap`/`AddEventToFunctionMap`/`AddOtherEventToFunctionMap` 注册处理函数
5. `InitResTypes()`（全量）或 `InitResTypeWithVal()`（精确）添加 ResType
6. 添加单元测试和 Fuzz 测试
