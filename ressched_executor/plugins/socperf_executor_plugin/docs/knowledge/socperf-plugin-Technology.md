# 技术术语知识

socperf_executor_plugin 模块核心技术框架和机制的知识定义，涵盖价值认知、原理、使用方式和代码关联。

## XML 配置驱动的资源节点解析

### 解决的问题（Problem）

| 子维度 | 内容 |
| --- | --- |
| 业务痛点 | 不同芯片方案的 CPU/GPU/DDR 频率节点路径不同，硬编码导致每个方案需重新编译 |
| 技术目标 | 资源节点定义由 XML 配置声明，运行时解析构建资源 ID → 节点路径映射，支持多配置文件叠加 |

### 原理（Principles）

| 子维度 | 内容 |
| --- | --- |
| 核心机制 | `SocPerfConfig` 通过 `GetCfgFiles` 获取配置文件链（CfgFiles），反转后依次加载（先 vendor 后 system，后者覆盖前者）。XML 根节点 `<Configs>` 下含 `<Resource>`（普通频率资源）和 `<GovResource>`（Governor 资源）两类。解析结果存入 `resourceNodeInfo_`（`map<int32_t, shared_ptr<ResourceNode>>`），ID 重复时跳过 |
| 关键流程 | GetCfgFiles → 反转文件链 → 依次 LoadConfigXmlFile → xmlReadFile → ParseResourceXmlFile → LoadResource/LoadGovResource → TraversalFreqResource/TraversalGovResource → 构建 ResNode/GovResNode → CheckPairResIdValid + CheckDefValid 校验 |

### 代码关联（Code）

| 子维度 | 内容 |
| --- | --- |
| 核心成员 | `resourceNodeInfo_`、`resourceNodeMutex_`（`include/socperf_executor_config.h`） |
| 核心方法 | `SocPerfConfig::Init()`、`LoadAllConfigXmlFile()`、`ParseResourceXmlFile()`、`LoadResource()`、`LoadGovResource()`（`src/socperf_executor_config.cpp`） |

### 约束与限制（Constraints）

| 子维度 | 内容 |
| --- | --- |
| 业务规则 | 资源 ID 需在有效范围内（WRITE_NODE: 1000-5999，REPORT_TO_PERFSO: 10000-10999）；pair 指向的资源必须已定义；default 值必须在 available 列表中；GovResource 的 node 值用 `|` 分隔后数量必须与 paths 数量一致 |
| 并发安全 | resourceNodeInfo_ 的写入通过 `resourceNodeMutex_`（std::mutex）保护；GovResNode 的 levelToStr 写入通过 `levelToStrMutex_` 保护 |

---

## fd 缓存与内核节点写入

### 解决的问题（Problem）

| 子维度 | 内容 |
| --- | --- |
| 业务痛点 | 频繁调频时反复 open/write/close 内核节点产生大量系统调用，影响性能 |
| 技术目标 | fd 初次打开后缓存复用，后续写入仅 lseek + write，减少系统调用开销 |

### 原理（Principles）

| 子维度 | 内容 |
| --- | --- |
| 核心机制 | `fdInfo_`（`map<string, int32_t>`）缓存路径到 fd 的映射。初次写入时 `GetFdForFilePath` 执行 realpath 校验 + open(O_RDWR\|O_CLOEXEC) + fdsan 打标签，后续直接从缓存取 fd。写入时 lseek(fd, 0, SEEK_SET) + write(fd, value)。值为 `NODE_DEFAULT_VALUE`(0) 时回退到资源节点的 def 值 |
| 关键流程 | WriteNodeThreadWraps(resIdVec, valueVec) → 遍历 UpdateResIdCurrentValue → IsGovResId 分支 → GovResNode 查 levelToStr 逐路径写 / ResNode 直接写 → WriteNode → GetFdForFilePath → lseek + write |

### 代码关联（Code）

| 子维度 | 内容 |
| --- | --- |
| 核心成员 | `fdInfo_`、`socPerfConfig_`（`include/socperf_executor_wirte_node.h`） |
| 核心方法 | `SocPerfExecutorWirteNode::WriteNodeThreadWraps()`、`UpdateCurrentValue()`、`WriteNode()`、`GetFdForFilePath()`（`src/socperf_executor_wirte_node.cpp`） |

### 约束与限制（Constraints）

| 子维度 | 内容 |
| --- | --- |
| 业务规则 | REPORT_TO_PERFSO 模式的资源不写节点；resourceNodeInfo_ 中不存在的 resId 跳过；GovResNode 的 level 不在 levelToStr 中时跳过 |
| 安全约束 | fd 通过 fdsan_exchange_owner_tag 打上 SCHEDULE_CGROUP_FDSAN_TAG 标签；析构时 fdsan_close_with_tag 释放所有缓存 fd；realpath 校验防止符号链接攻击 |

---

## 配置叠加加载机制

### 解决的问题（Problem）

| 子维度 | 内容 |
| --- | --- |
| 业务痛点 | 不同产品层级（system / vendor / 芯片方案）需要覆盖或新增资源节点，单一配置文件无法满足 |
| 技术目标 | 支持 CfgFiles 链式加载，低优先级配置先加载，高优先级配置覆盖（ID 重复时跳过不覆盖） |

### 原理（Principles）

| 子维度 | 内容 |
| --- | --- |
| 核心机制 | `GetAllRealConfigPath` 调用 `GetCfgFiles` 获取配置文件链（系统按优先级排列），然后 `reverse` 反转后依次 `LoadConfigXmlFile`。加载时若 `resourceNodeInfo_.find(resId)` 已存在则跳过（先加载的低优先级配置保留，后加载的高优先级配置被跳过） |
| 关键流程 | GetCfgFiles → 遍历 paths 收集 → reverse → 依次加载 → ID 去重（已存在跳过） |

### 代码关联（Code）

| 子维度 | 内容 |
| --- | --- |
| 核心方法 | `SocPerfConfig::GetAllRealConfigPath()`、`LoadAllConfigXmlFile()`（`src/socperf_executor_config.cpp`） |

### 约束与限制（Constraints）

| 子维度 | 内容 |
| --- | --- |
| 业务规则 | ID 重复时保留先加载的配置（反转后低优先级先加载），非覆盖语义；任一配置文件加载失败则整体失败 |
