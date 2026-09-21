# socperf_executor_plugin 模块 - AI 编码代理指南

## 基本信息

socperf_executor_plugin 是资源调度服务执行器侧的调频插件，运行在 `ressched_executor` 服务（SA 1918，root 权限）中。它接收 ressched 决策侧（SA 1901）下发的调频请求，通过解析 XML 配置将资源 ID 映射到内核 sysfs 节点路径，直接写内核节点控制 CPU/GPU/DDR 频率。插件由 `ressched_executor` 通过 `dlopen` 加载，导出 `OnPluginInit`、`OnPluginDisable`、`OnDispatchResource` 三个 C 入口。

## 架构及依赖

### 核心架构

采用**事件驱动 + XML 配置驱动**架构，继承 `Plugin` 基类实现 `Init`/`Disable`/`DispatchResource`：

- **事件分发**：`functionMap_`（`unordered_map<uint32_t, function<void(ResData)>>`）将 `ResExeType` 映射到处理函数，当前仅订阅 `EWS_TYPE_SOCPERF_EXECUTOR_ASYNC_EVENT` 的 `SOCPERF_EVENT_WIRTE_NODE` 值
- **精确订阅**：仅使用 `resTypeWithVal_` 按 `(resType, value)` 精确订阅，无全量订阅
- **配置管理**：`SocPerfConfig` 单例从 `socperf_resource_config.xml` 加载资源节点定义，支持多配置文件叠加加载（`GetCfgFiles` 按 CfgFiles 链反转后依次加载）
- **节点写入**：`SocPerfExecutorWirteNode` 单例管理 fd 缓存（`fdInfo_`），通过 `lseek + write` 写内核节点

## 目录结构

```
socperf_executor_plugin/
├── framework/
│   ├── include/
│   │   ├── socperf_executor_plugin.h      # SocPerfExecutorPlugin 类
│   │   ├── socperf_executor_config.h      # SocPerfConfig 类（XML 配置解析）
│   │   ├── socperf_executor_wirte_node.h  # SocPerfExecutorWirteNode 类（内核节点写入）
│   │   └── socperf_common.h              # ResourceNode/ResNode/GovResNode 类型、常量、工具函数
│   ├── src/
│   │   ├── socperf_executor_plugin.cpp    # 插件主逻辑
│   │   ├── socperf_executor_config.cpp    # XML 配置解析实现
│   │   └── socperf_executor_wirte_node.cpp # 内核节点写入实现
│   └── test/unittest/                     # GTest 单元测试
├── BUILD.gn                               # socperf_executor_plugin（动态库）+ socperf_executor_plugin_static
└── libsocperf_executor_plugin.versionscript # 仅导出 OnPluginInit/OnPluginDisable/OnDispatchResource
```

## 知识索引

稳定背景知识放在 docs/knowledge/，改动前按场景读取对应文件：

| 场景 | 先读 |
|------|------|
| 理解业务术语 | docs/knowledge/glossary.md |
| 了解业务背景 | docs/knowledge/business-context.md |
| 架构设计 | docs/knowledge/architecture.md |
| 核心技术机制 | docs/knowledge/socperf-plugin-Technology.md |
| 核心业务实体 | docs/knowledge/socperf-plugin-Entity.md |
| 编码前必读 | docs/knowledge/coding-standards.md |

## 新增资源节点

1. `etc/soc_perf/socperf_resource_config.xml` 添加 `<Resource>` 或 `<GovResource>` 节点
2. 资源 ID 需在有效范围内（1000-5999，REPORT_TO_PERFSO 模式支持 10000-10999）
3. 若有 `pair` 属性，需确保 pairResId 已定义
4. 若有 `available` 列表，`default` 值必须在 available 中
5. 添加单元测试覆盖配置解析和节点写入
