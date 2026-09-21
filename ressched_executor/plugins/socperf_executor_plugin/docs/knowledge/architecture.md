# 架构设计及约束

## 架构设计

### 设计原则

| 原则 | 描述 | 理由 |
| ---- | ---- | ---- |
| 事件驱动 | 以 functionMap_ 映射表驱动事件路由，处理逻辑与分发逻辑解耦 | 新增事件类型仅需注册映射条目 |
| XML 配置驱动 | 资源节点定义由 XML 配置文件声明，支持多文件叠加加载 | 产品差异化通过配置文件实现，无需修改代码 |
| fd 缓存复用 | 内核节点 fd 首次打开后缓存，后续写入仅 lseek+write | 避免频繁 open/close 系统调用开销 |

### 逻辑架构

```plantuml
@startuml
skinparam componentStyle rectangle

package "事件分发层" {
    [functionMap_\n(ResExeType → 处理函数)] as Dispatcher
    [resTypeWithVal_\n(精确订阅)] as Subscribe
}

package "配置管理层" {
    [SocPerfConfig\n(XML 解析, resourceNodeInfo_)] as Config
    [socperf_resource_config.xml] as XmlFile
}

package "节点写入层" {
    [SocPerfExecutorWirteNode\n(fdInfo_ 缓存)] as Writer
    [WriteNode\n(lseek + write)] as WriteAction
}

package "资源节点模型" {
    [ResourceNode (基类)] as BaseNode
    [ResNode (普通频率)] as NormalNode
    [GovResNode (Governor)] as GovNode
}

node "内核层" {
    [sysfs 节点] as Kernel
}

node "外部服务" {
    [PerfSo] as PerfSo
}

Dispatcher --> Subscribe : 事件过滤
Dispatcher --> Config : 查询资源节点
Config --> XmlFile : LoadAllConfigXmlFile\n(多文件叠加, 反转后加载)
Config --> BaseNode : 构建资源节点
BaseNode <|-- NormalNode
BaseNode <|-- GovNode
Dispatcher --> Writer : WriteNodeThreadWraps\n(resIdVec + valueVec)
Writer --> WriteAction : UpdateCurrentValue
Writer --> Kernel : WRITE_NODE 模式
Writer --> PerfSo : REPORT_TO_PERFSO 模式
@enduml
```

### 非功能设计

| 场景类别 | 方案设计 |
| ---- | ---- |
| 安全加固 | CFI + cfi_cross_dso + PAC_RET + stack-protector-strong + RELRO/NOW |
| fd 安全 | fdsan 标签保护（SCHEDULE_CGROUP_FDSAN_TAG），析构时 fdsan_close_with_tag 释放 |
| 配置叠加 | GetCfgFiles 返回多配置文件链，反转后依次加载，先加载的配置若 ID 重复则跳过（`resourceNodeInfo_.find` 去重） |
| 路径安全 | realpath 校验配置文件路径和节点路径，防止符号链接攻击和路径越界 |
