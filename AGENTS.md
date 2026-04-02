# 资源调度服务 (Resource Schedule Service)

## 项目概述

**资源调度服务 (RSS)** 是 OpenHarmony 资源调度子系统的核心组件，负责系统事件感知、分发和资源调度策略执行。

**核心能力**：事件管理、资源调度（Cgroup/SoC/帧率）、插件架构

**技术栈**：C++17 / GN / OHOS IPC / JSON

## 架构设计

## 架构设计

### 整体架构
```
ressched/                              # 主资源调度服务
├── interfaces/                        # API 接口层
│   ├── innerkits/                     # C++ 内部客户端
│   │   ├── ressched_client/           # RSS 客户端库
│   │   └── suspend_manager_base_client/  # 暂停管理器客户端
│   └── kits/                          # 多语言绑定
│       ├── c/                         # C API
│       ├── js/                        # JavaScript/Node-API
│       └── ets/                       # ArkTS/ETS (taihe)
├── services/                          # 服务实现层
│   ├── resschedmgr/                   # 资源调度管理器
│   │   ├── resschedfwk/               # 调度框架核心
│   │   │   ├── PluginMgr              # 插件管理器
│   │   │   ├── ConfigReader           # 配置读取器
│   │   │   └── ResSchedMgr            # 调度管理器主类
│   │   └── pluginbase/                # 插件基类定义
│   │       ├── Plugin                 # 插件接口
│   │       ├── ResData                # 资源数据结构
│   │       └── ResType                # 事件类型枚举
│   ├── resschedservice/               # 服务层 (SA 1901)
│   └── suspend_manager_base/          # 暂停管理器
├── sched_controller/                  # 事件采集层
│   ├── observer/                      # 事件观察者
│   │   ├── AppStateObserver           # 应用状态观察
│   │   ├── AudioObserver              # 音频状态观察
│   │   ├── WindowStateObserver        # 窗口状态观察
│   │   ├── MmiObserver                # 多模输入观察
│   │   └── ...
│   └── common_event/                  # 公共事件处理
├── scene_recognize/                   # 场景识别层
│   ├── SceneRecognizerMgr             # 场景识别管理器
│   ├── SlideRecognizer                # 滑动识别
│   └── ...
└── plugins/                           # 策略插件层
    ├── cgroup_sched_plugin/           # Cgroup 调度插件
    ├── socperf_plugin/                # SoC 性能插件
    ├── frame_aware_plugin/            # 帧感知插件
    └── device_standby_plugin/         # 设备待机插件

ressched_executor/                     # 执行器服务 (SA 1918) - 详见 docs/ressched_executor.md
└── plugins/                           # 执行器插件
    └── socperf_executor_plugin/       # SoC 性能执行器
```

### 数据流向

系统事件 → 事件采集层(`sched_controller`) → 调度管理层(`PluginMgr::DispatchResource()`) → 插件 → 内核/硬件

## 核心组件

### 1. 插件架构

插件是 RSS 的核心扩展机制。

**接口定义**：`services/resschedmgr/pluginbase/include/plugin.h`

```cpp
class Plugin {
public:
    virtual void Init() = 0;
    virtual void DispatchResource(const std::shared_ptr<ResData>& data) = 0;
};

struct ResData {
    uint32_t resType;           // 事件类型 (ResType)
    int64_t value;
    nlohmann::json payload;
};
```

**事件类型定义**：`ressched/interfaces/innerkits/ressched_client/include/res_type.h`

### 2. 插件管理器

- **关键类**：`PluginMgr` (单例)
- **代码**：`ressched/services/resschedmgr/resschedfwk/src/plugin_mgr.cpp`
- **职责**：加载插件(.z.so)、分发事件、管理生命周期

### 3. 事件采集与场景识别

| 模块 | 关键类 | 功能 |
|------|--------|------|
| 事件采集 | `ObserverManager` | 管理观察者，采集系统事件 |
| 场景识别 | `SlideRecognizer` | 滑动手势识别 |

## 构建系统

**构建文件**：`bundle.json` / `ressched.gni` / `BUILD.gn`

**编译目标**：

| 类型 | GN 模板 | 说明 |
|------|---------|------|
| 共享库 | `ohos_shared_library` | 插件 (.z.so) |
| 单元测试 | `ohos_unittest` | 测试用例 |
| Fuzz 测试 | `ohos_fuzztest` | 模糊测试 |

## 开发指南

### 代码风格

- 类名: PascalCase (`PluginMgr`)
- 方法: PascalCase (`DispatchResource`)
- 变量: camelCase (`resType`)
- 常量: UPPER_SNAKE_CASE (`RES_TYPE_SCREEN_STATUS`)
- 私有成员: 后缀 `_` (`pluginMgr_`)

### 添加新插件

1. 继承 `Plugin` 基类实现插件逻辑
2. 配置 `BUILD.gn` 构建共享库
3. 在 `res_sched_plugin_switch.xml` 中启用插件

**参考实现**：`ressched/plugins/cgroup_sched_plugin/`

## 测试

**测试目录**：`ressched/test/`

- `unittest/` - 单元测试
- `fuzztest/` - Fuzz 测试
- `testutil/` - 测试工具

## 子目录指南

| 目录 | 说明 |
|------|------|
| ressched_executor | [docs/ressched_executor.md](docs/ressched_executor.md) |

## 核心工作原则

### 代码验证

- ✅ 使用 Read/Grep 读取实际源代码，提供完整路径和行号
- ❌ 禁止猜测或编造代码实现

**代码规范**: C++17 标准，每行 ≤120 字符，函数 ≤50 行有效代码，深度 ≤4 层

### 推测管理

不确定的内容必须标注 **(推测)** 并说明理由。

## 参考资源

- **README**: `README_ZH.md`
- **进程管理规范**: `Resource_Schedule_Service_Process_Management_Specifications_ZH.md`
- **执行器模块**: [docs/ressched_executor.md](docs/ressched_executor.md)