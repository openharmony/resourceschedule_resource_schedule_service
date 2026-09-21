# 业务实体知识

socperf_executor_plugin 模块核心业务实体的完整知识定义，涵盖静态概念、动态运行、规则约束与代码映射。

## ResNode（普通频率资源）

### 实体概念

| 子维度 | 内容 |
| --- | --- |
| 实体名称 | ResNode / 普通频率资源 |
| 实体定义 | 单路径单值的频率资源节点，写入时将数值转为字符串写入单个内核 sysfs 节点 |
| 核心特征 | `path`（内核节点路径）、`pair`（配对资源 ID）、`mode`（模式）、`persistMode`（持久模式）、`def`（默认值）、`available`（合法值集合） |

### 上下文与场景

| 子维度 | 内容 |
| --- | --- |
| 使用场景 | CPU 最小/最大频率、GPU 频率、DDR 频率等单节点控制的资源 |
| 状态流转 | 收到 resId + value → value=0 时回退到 def → 查 ResNode → WriteNode(path, value) → lseek + write |

### 规格与约束

| 子维度 | 内容 |
| --- | --- |
| 业务规则 | persistMode=REPORT_TO_PERFSO 时不写节点；pair 必须指向已定义的资源；def 必须在 available 中（available 非空时） |
| 资源 ID 范围 | 1000-5999（每类 1000 个，共 5 类：MIN_RESOURCE_ID ~ MAX_RESOURCE_ID） |

### 代码与符号

| 子维度 | 内容 |
| --- | --- |
| 核心成员 | `path`、`pair`、`mode`、`def`、`available`（`include/socperf_common.h`） |
| 核心方法 | `SocPerfExecutorWirteNode::UpdateCurrentValue()`（`src/socperf_executor_wirte_node.cpp`） |

---

## GovResNode（Governor 资源）

### 实体概念

| 子维度 | 内容 |
| --- | --- |
| 实体名称 | GovResNode / Governor 资源 |
| 实体定义 | 多路径多级别映射的频率资源节点，一次写入涉及多个内核节点，每个节点写不同字符串值 |
| 核心特征 | `paths`（多个内核节点路径列表）、`levelToStr`（level → 各路径对应字符串值的映射，`|` 分隔）、`def`（默认 level） |

### 上下文与场景

| 子维度 | 内容 |
| --- | --- |
| 使用场景 | CPU Governor 调速器配置，需同时写多个 sysfs 节点（如 governor 名 + 调速参数） |
| 状态流转 | 收到 resId + level → 查 GovResNode.levelToStr[level] → 获取各路径对应字符串列表 → 逐路径 WriteNode(paths[i], targetStrs[i]) |

### 规格与约束

| 子维度 | 内容 |
| --- | --- |
| 业务规则 | persistMode=REPORT_TO_PERFSO 时不写节点；level 不在 levelToStr 中时跳过；XML 中 node 值用 `|` 分隔后数量必须与 paths 数量一致 |
| 并发安全 | levelToStr 的读写通过 `levelToStrMutex_`（std::mutex）保护 |

### 代码与符号

| 子维度 | 内容 |
| --- | --- |
| 核心成员 | `paths`、`levelToStr`、`levelToStrMutex_`（`include/socperf_common.h`） |
| 核心方法 | `SocPerfExecutorWirteNode::UpdateCurrentValue()`、`SocPerfConfig::LoadGovResourceAvailable()`（`src/socperf_executor_wirte_node.cpp` / `src/socperf_executor_config.cpp`） |

---

## ResourceNode（资源节点基类）

### 实体概念

| 子维度 | 内容 |
| --- | --- |
| 实体名称 | ResourceNode / 资源节点基类 |
| 实体定义 | 所有资源节点的基类，定义资源 ID、名称、持久模式、默认值、合法值集合等公共属性 |
| 核心特征 | `id`（资源 ID）、`name`（资源名称）、`isGov`（是否 Governor 资源）、`persistMode`（持久模式）、`def`（默认值）、`available`（合法值集合） |

### 上下文与场景

| 子维度 | 内容 |
| --- | --- |
| 使用场景 | 配置加载阶段构建资源节点实例，运行时通过 `resourceNodeInfo_` map 按 ID 查询 |
| 状态流转 | XML 解析 → 构建 ResNode(isGov=false) 或 GovResNode(isGov=true) → 存入 resourceNodeInfo_ → 运行时按 resId 查询并 static_pointer_cast 到子类 |

### 规格与约束

| 子维度 | 内容 |
| --- | --- |
| 业务规则 | persistMode 决定处理路径：WRITE_NODE(0) 写内核节点，REPORT_TO_PERFSO(1) 上报 SoC 性能服务（SA 1906）；available 非空时 def 必须在 available 中 |

### 代码与符号

| 子维度 | 内容 |
| --- | --- |
| 核心成员 | `id`、`name`、`isGov`、`persistMode`、`def`、`available`（`include/socperf_common.h`） |
| 核心常量 | `WRITE_NODE`、`REPORT_TO_PERFSO`、`MIN_RESOURCE_ID`、`MAX_RESOURCE_ID`、`RES_ID_NUMS_PER_TYPE`、`RES_ID_NUMS_PER_TYPE_EXT`、`NODE_DEFAULT_VALUE`（`include/socperf_common.h`） |
| 核心方法 | `SocPerfConfig::IsValidResId()`、`IsGovResId()`、`GetResIdNumsPerType()`（`src/socperf_executor_config.cpp`） |
