# 技术术语知识

socperf_plugin 模块核心技术框架和机制的知识定义，涵盖价值认知、原理、使用方式和代码关联。

## 函数映射表事件分发

### 解决的问题（Problem）

| 子维度 | 内容 |
| --- | --- |
| 业务痛点 | 约 40 种事件类型若用 switch-case 硬编码，新增事件需修改分发逻辑，维护成本高 |
| 技术目标 | 以数据结构驱动事件路由，新增事件仅需注册映射条目，分发逻辑无需修改 |

### 原理（Principles）

| 子维度 | 内容 |
| --- | --- |
| 核心机制 | `functionMap`（`unordered_map<uint32_t, function<void(ResData)>>`）将 `ResType` 映射到处理函数 Lambda；`DispatchResource` 先尝试 `cmdidPolicy_` 通用路径，未命中再查 `functionMap`，均未匹配则静默忽略 |
| 关键流程 | 事件到达 → 查 cmdidPolicy_（(resType, value) → (cmdid, onoff)）→ 命中则 PerfRequestEx 下发 → 未命中查 functionMap → 命中则执行处理函数 → 未命中静默忽略 |

### 代码关联（Code）

| 子维度 | 内容 |
| --- | --- |
| 核心成员 | `functionMap`、`cmdidPolicy_`、`resTypes`、`resTypeWithVal_`（`include/socperf_plugin.h`） |
| 核心方法 | `SocPerfPlugin::DispatchResource()`、`SocPerfPlugin::HandleCmdidPolicy()`（`src/socperf_plugin.cpp`） |

---

## 配置驱动策略与云端热更新

### 解决的问题（Problem）

| 子维度 | 内容 |
| --- | --- |
| 业务痛点 | 调频策略参数硬编码导致策略调整必须重新编译，产品差异化需求难以快速响应 |
| 技术目标 | 策略参数由配置定义，运行时解析加载，支持云端下发热更新，无需修改代码 |

### 原理（Principles）

| 子维度 | 内容 |
| --- | --- |
| 核心机制 | Init 阶段从 PluginMgr 加载 cmdidPolicy、specialExtension、weakInterAction、bundleNameBoostList、batteryCapacityLimitFreq、socperfCrucialFunc 等配置；运行时通过 HandleRssCloudConfigUpdate 接收云端配置，增量加载到现有策略表，不清理已有策略 |
| 关键流程 | PluginMgr.GetConfig → 解析配置 Item/SubItem → 构建运行时映射表 → 云端下发 → 解析 params.SOCPERF → 增量加载 → 异常时整体回滚 |

### 代码关联（Code）

| 子维度 | 内容 |
| --- | --- |
| 核心方法 | `InitCmdidPolicy()`、`InitSpecialExtension()`、`LoadWeakInterAction()`、`InitBundleNameBoostList()`、`InitBatteryConfig()`、`HandleRssCloudConfigUpdate()`（`src/socperf_plugin.cpp`） |

### 约束与限制（Constraints）

| 子维度 | 内容 |
| --- | --- |
| 业务规则 | 云端配置解析异常时整体回滚，不应用部分配置；云端配置覆盖本地同名配置项，未覆盖项保持本地值 |

---

## 亮灭屏延迟状态机

### 解决的问题（Problem）

| 子维度 | 内容 |
| --- | --- |
| 业务痛点 | 亮灭屏频繁切换导致调频命令抖动，频繁频率波动影响用户体验和功耗 |
| 技术目标 | 通过延迟执行和超时检测平滑亮灭屏切换，避免频繁调频命令下发 |

### 原理（Principles）

| 子维度 | 内容 |
| --- | --- |
| 核心机制 | 灭屏事件延迟 5000ms 执行（`SCREEN_OFF_TIME_DELAY`），期间收到亮屏则取消延迟任务；唤醒开始事件立即执行亮屏调频并启动 3000ms 超时任务（`WAKE_UP_TIME_DELAY`），超时后查询 PowerMgr 屏幕状态，未亮起则触发灭屏调频。所有操作通过 `screenMutex_`（ffrt::mutex）保护 |
| 关键流程 | 灭屏事件 → 提交 5s ffrt 延迟任务 → 期间收到亮屏 → 取消任务 → 唤醒开始 → 立即亮屏调频 + 3s 超时任务 → 超时查 PowerMgr → 未亮起则灭屏调频 |

### 代码关联（Code）

| 子维度 | 内容 |
| --- | --- |
| 核心成员 | `screenStatus_`、`screenMutex_`、`wakeUpTimeOutTask_`（`include/socperf_plugin.h`） |
| 核心方法 | `HandleScreenStatus()`、`HandleScreenOn()`、`HandleScreenOff()`、`HandleWakeUp()`（`src/socperf_plugin.cpp`） |

### 约束与限制（Constraints）

| 子维度 | 内容 |
| --- | --- |
| 系统限制 | ffrt 延迟任务需通过 screenMutex_ 加锁保护屏幕状态读写；屏幕初始值默认为亮屏（值 1） |
