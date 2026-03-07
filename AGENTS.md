# 资源调度服务 (Resource Schedule Service) - Agent 指南

本文件为 Claude Code 提供资源调度服务 (RSS) 项目的开发指导。

---

## 快速参考

### 构建命令

```bash
# 构建整个资源调度服务组件
hb build resource_schedule_service

# 构建主服务
hb build ressched/services:resschedsvc

# 构建执行器服务
hb build ressched_executor/services:resschedexesvc

# 构建客户端库
hb build ressched/interfaces/innerkits/ressched_client:ressched_client

# 构建插件
hb build ressched/plugins/cgroup_sched_plugin/framework:cgroup_sched
hb build ressched/plugins/socperf_plugin:socperf_plugin

# 构建场景识别模块
hb build ressched/scene_recognize:scene_recognize
```

### 测试命令

```bash
# 构建单元测试
hb build ressched/test/unittest:unittest

# 构建 Fuzz 测试
hb build ressched/test/fuzztest:fuzztest

# 运行特定单元测试
./out/{product}/tests/ressched/unittest/resschedservice_test

# 运行特定测试用例
./resschedservice_test --gtest_filter=PluginMgrTest.Init
```

### 构建输出位置

- **服务可执行文件**: `out/{product}/resourceschedule/resource_schedule_service/`
- **插件库**: `out/{product}/resourceschedule/resource_schedule_service/*.z.so`
- **测试文件**: `out/{product}/tests/ressched/`
- **构建日志**: `out/{product}/build.log`

---

## 项目概述

**资源调度服务 (RSS)** 是 OpenHarmony 资源调度子系统的核心组件，负责系统事件感知、分发和资源调度策略执行。

### 核心能力

**事件管理**: 
- 感知系统事件（应用启动/退出、屏幕亮/灭、窗口焦点变化等）
- 通过外部接口和监听器采集事件
- 将事件分发到订阅的插件

**资源调度**:
- 进程 Cgroup 管理和调度
- SoC 性能频率调制
- 帧率感知和延迟控制
- 设备待机状态管理

**插件架构**:
- 基于插件的扩展机制
- 动态加载共享库 (.z.so)
- 灵活的事件订阅和处理

### 系统定位

```
┌─────────────────────────────────────────────────────────┐
│  应用层 (System Apps / 3rd Party Apps)                  │
└─────────────────────────────────────────────────────────┘
                           │
                           ↓
┌─────────────────────────────────────────────────────────┐
│  系统服务层                                              │
│  - Ability Runtime (应用运行时)                         │
│  - Window Manager (窗口管理)                            │
│  - Audio Framework (音频框架)                           │
│  - Telephony Service (电话服务)                         │
└─────────────────────────────────────────────────────────┘
                           │ 上报事件
                           ↓
┌─────────────────────────────────────────────────────────┐
│  资源调度服务 (RSS)                                      │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐ │
│  │ 事件采集层   │ -> │ 插件调度层   │ -> │ 策略执行层   │ │
│  │ (Observer)  │    │ (PluginMgr) │    │ (Plugins)   │ │
│  └─────────────┘    └─────────────┘    └─────────────┘ │
└─────────────────────────────────────────────────────────┘
                           │ 调度指令
                           ↓
┌─────────────────────────────────────────────────────────┐
│  内核层                                                  │
│  - Cgroup (进程分组)                                    │
│  - CPU/GPU 调频 (SocPerf)                              │
│  - 电源管理                                             │
└─────────────────────────────────────────────────────────┘
```

### 技术栈

| 技术领域 | 技术选型 |
|---------|---------|
| **语言** | C++ (C++11/14/17) |
| **构建系统** | GN (Generate Ninja) |
| **测试框架** | Google Test / Google Mock |
| **IPC** | OHOS IPC (IRemoteObject, IPCSkeleton) |
| **数据格式** | JSON (nlohmann/json), XML (libxml2) |
| **日志** | HiLog |
| **跟踪** | HiTrace |
| **事件** | HiSysEvent |

---

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
│   │   └── ...                        # 其他观察者
│   └── common_event/                  # 公共事件处理
├── scene_recognize/                   # 场景识别层
│   ├── SceneRecognizerMgr             # 场景识别管理器
│   ├── SlideRecognizer                # 滑动识别
│   └── ...                            # 其他识别器
└── plugins/                           # 策略插件层
    ├── cgroup_sched_plugin/           # Cgroup 调度插件
    ├── socperf_plugin/                # SoC 性能插件
    ├── frame_aware_plugin/            # 帧感知插件
    └── device_standby_plugin/         # 设备待机插件

ressched_executor/                     # 执行器服务 (SA 1918)
└── plugins/                           # 执行器插件
    └── socperf_executor_plugin/       # SoC 性能执行器
```

### 数据流向

```
┌─────────────────────────────────────────────────────────┐
│  系统事件产生                                             │
│  (AMS/WMS/Audio/Telephony/...)                         │
└─────────────────────────────────────────────────────────┘
                           │
                           ↓
┌─────────────────────────────────────────────────────────┐
│  事件采集层 (sched_controller)                          │
│  - 观察者模式订阅系统服务                                │
│  - 将系统事件转换为 ResData                             │
└─────────────────────────────────────────────────────────┘
                           │ ResData (resType, value, payload)
                           ↓
┌─────────────────────────────────────────────────────────┐
│  调度管理层 (services/resschedmgr)                       │
│  - PluginMgr::DispatchResource()                        │
│  - 事件路由到订阅插件                                    │
└─────────────────────────────────────────────────────────┘
                           │
           ┌───────────────┼───────────────┐
           ↓               ↓               ↓
    ┌──────────┐    ┌──────────┐    ┌──────────┐
    │ cgroup   │    │ socperf  │    │ frame    │
    │ 插件     │    │ 插件     │    │ 感知插件  │
    └──────────┘    └──────────┘    └──────────┘
           │               │               ↓
           └───────────────┴───────────────┘
                           │
                           ↓
┌─────────────────────────────────────────────────────────┐
│  内核/硬件层                                             │
│  - Cgroup 文件系统                                      │
│  - CPU/GPU 频率调节                                     │
│  - 电源管理                                             │
└─────────────────────────────────────────────────────────┘
```

---

## 核心组件

### 1. 插件架构

插件是 RSS 的核心扩展机制，所有调度策略都以插件形式实现。

**插件接口** (`services/resschedmgr/pluginbase/include/plugin.h`):
```cpp
class Plugin {
public:
    virtual ~Plugin() = default;
    virtual void Init() = 0;                    // 插件初始化
    virtual void Disable() = 0;                 // 插件停用
    virtual void DispatchResource(const std::shared_ptr<ResData>& data) = 0;
    virtual int32_t DeliverResource(const std::shared_ptr<ResData>& data);
};
```

**资源数据结构** (`services/resschedmgr/pluginbase/include/res_data.h`):
```cpp
struct ResData {
    uint32_t resType;           // 事件类型 (ResType 枚举)
    int64_t value;              // 事件值 (上下文相关)
    nlohmann::json payload;     // 附加数据 (JSON 格式)
    nlohmann::json* reply;      // 回复数据 (同步事件)
};
```

**事件类型示例**:
| 事件类型 | 值 | 说明 |
|---------|---|------|
| `RES_TYPE_SCREEN_STATUS` | 1 | 屏幕亮/灭 |
| `RES_TYPE_APP_STATE_CHANGE` | 2 | 应用状态变化 |
| `RES_TYPE_WINDOW_FOCUS` | 3 | 窗口焦点变化 |
| `RES_TYPE_CGROUP_ADJUSTER` | 4 | Cgroup 调整 |
| `RES_TYPE_CLICK_RECOGNIZE` | 5 | 点击事件识别 |
| `RES_TYPE_SLIDE_RECOGNIZE` | 6 | 滑动事件识别 |
| `RES_TYPE_AUDIO_RENDER_STATE_CHANGE` | 7 | 音频播放状态变化 |
| `RES_TYPE_CALL_STATE_UPDATE` | 8 | 电话通话状态更新 |

### 2. 插件管理器 (PluginMgr)

**职责**:
- 加载/卸载插件共享库
- 维护插件生命周期
- 分发事件到订阅插件
- 同步事件处理支持

**关键方法**:
```cpp
class PluginMgr {
public:
    static PluginMgr& GetInstance();
    void LoadPlugin(const std::string& pluginName);
    void UnloadPlugin(const std::string& pluginName);
    void DispatchResource(const std::shared_ptr<ResData>& data);
    int32_t DeliverResource(const std::shared_ptr<ResData>& data);
};
```

**插件加载流程**:
```
读取配置 -> 加载 .z.so -> dlsym 获取实例 -> 调用 Init() -> 注册事件订阅
```

**性能约束**:
- 事件处理 > 1ms: 记录警告日志
- 事件处理 > 10ms: 框架报告错误

### 3. 事件采集系统

**观察者管理器** (`sched_controller/observer/observer_manager.cpp`):
```cpp
class ObserverManager {
public:
    static ObserverManager& GetInstance();
    void Init();        // 初始化所有观察者
    void Disable();     // 停用所有观察者
};
```

**主要观察者**:
| 观察者 | 监听内容 | 输出事件 |
|--------|---------|---------|
| AppStateObserver | 应用生命周期 | RES_TYPE_APP_STATE_CHANGE |
| AudioObserver | 音频播放状态 | RES_TYPE_AUDIO_RENDER_STATE_CHANGE |
| WindowStateObserver | 窗口焦点 | RES_TYPE_WINDOW_FOCUS |
| MmiObserver | 输入事件 | RES_TYPE_CLICK_RECOGNIZE |
| SchedTelephonyObserver | 通话状态 | RES_TYPE_CALL_STATE_UPDATE |

### 4. 场景识别系统

**场景识别器基类** (`scene_recognize/include/scene_recognizer_base.h`):
```cpp
class SceneRecognizerBase {
public:
    virtual void OnEvent(const std::shared_ptr<ResData>& data) = 0;
    virtual void Init() = 0;
    virtual void Disable() = 0;
};
```

**场景识别器列表**:
- `SlideRecognizer` - 滑动手势识别
- `ContinuousAppInstallRecognizer` - 连续应用安装识别
- `SystemUpgradeSceneRecognizer` - 系统升级场景识别
- `BackgroundSensitiveTaskOverlappingSceneRecognizer` - 后台任务重叠识别

---

## 构建系统

### 构建配置

**主构建文件**:
- `bundle.json` - OHOS 组件定义和依赖
- `ressched/ressched.gni` - ressched 模块构建变量
- `ressched_executor/ressched_executor.gni` - 执行器构建变量
- `BUILD.gn` - 各模块构建目标

**功能开关** (在 `ressched.gni` 中定义):
```gn
ressched_with_telephony_state_registry_enable    # 电话支持
ressched_with_communication_netmanager_base_enable # 网络管理器支持
resource_schedule_service_with_ffrt_enable       # FFRT 支持
ressched_with_resourceschedule_soc_perf_enable   # SoC 性能支持
ressched_with_resourceschedule_multimedia_audio_framework_enable  # 音频支持
communication_bluetooth_perception_enable        # 蓝牙支持
rss_mmi_enable                                   # 多模输入支持
ressched_frame_aware_sched_enable                # 帧感知调度
```

### 编译目标类型

| 目标类型 | GN 模板 | 说明 |
|---------|---------|------|
| 共享库 | `ohos_shared_library` | 插件 (.z.so) |
| 静态库 | `ohos_source_set` | 静态链接集合 |
| 可执行文件 | `ohos_executable` | 测试程序 |
| 单元测试 | `ohos_unittest` | 测试用例 |
| Fuzz 测试 | `ohos_fuzztest` | 模糊测试 |

### 安全编译选项

```gn
sanitize = {
  cfi = true              # 控制流完整性
  cfi_cross_dso = true    # 跨 DSO CFI
  debug = false
}
branch_protector_ret = "pac_ret"      # 返回地址保护
cflags_cc = [ "-fstack-protector-strong" ]  # 栈保护
asmflags = [ "-Wl,-z,relro,-z,now" ]  # 重定位保护
```

---

## 开发指南

### 代码风格

**文件头** (Apache 2.0 许可证):
```cpp
/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
```

**命名规范**:
| 类型 | 规范 | 示例 |
|-----|------|------|
| 类名 | PascalCase | `ResSchedService`, `PluginMgr` |
| 方法 | PascalCase | `DispatchResource`, `Init` |
| 变量 | camelCase | `resType`, `pluginMgr` |
| 常量 | UPPER_SNAKE_CASE | `RES_TYPE_SCREEN_STATUS` |
| 私有成员 | 后缀 `_` | `pluginMgr_` |
| 命名空间 | OHOS::ResourceSchedule | - |

**头文件保护**:
```cpp
#ifndef RESSCHED_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_PLUGIN_H
#define RESSCHED_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_PLUGIN_H
// ...
#endif // RESSCHED_SERVICES_RESSCHEDMGR_PLUGINBASE_INCLUDE_PLUGIN_H
```

### 添加新插件

1. **创建插件类**:
```cpp
#include "plugin.h"

class MyPlugin : public OHOS::ResourceSchedule::Plugin {
public:
    void Init() override;
    void Disable() override;
    void DispatchResource(const std::shared_ptr<ResData>& data) override;
};

extern "C" Plugin* CreatePlugin() {
    return new MyPlugin();
}
```

2. **实现关键方法**:
```cpp
void MyPlugin::Init() {
    // 初始化资源
}

void MyPlugin::DispatchResource(const std::shared_ptr<ResData>& data) {
    switch (data->resType) {
        case ResType::RES_TYPE_SCREEN_STATUS:
            HandleScreenStatus(data);
            break;
        // ...
    }
}
```

3. **创建 BUILD.gn**:
```gn
ohos_shared_library("my_plugin") {
  sources = [ "src/my_plugin.cpp" ]
  include_dirs = [ "include" ]
  deps = [ "//ressched/services/resschedmgr:resschedmgr" ]
  part_name = "resource_schedule_service"
  subsystem_name = "resourceschedule"
}
```

4. **配置插件** (res_sched_plugin_switch.xml):
```xml
<pluginSwitch>
    <plugin name="my_plugin" enable="true"/>
</pluginSwitch>
```

### 日志使用

```cpp
#include "res_sched_log.h"

RESSCHED_LOGD("Debug message: %{public}s", str.c_str());  // Debug
RESSCHED_LOGI("Info message");                             // Info
RESSCHED_LOGW("Warning message");                          // Warning
RESSCHED_LOGE("Error message");                            // Error
RESSCHED_LOGF("Fatal message");                            // Fatal
```

### 性能跟踪

```cpp
#include "res_sched_hitrace_chain.h"

HitraceScoped trace("MyPlugin::DispatchResource");
// 跟踪代码块执行时间
```

---

## 测试

### 测试结构

```
ressched/test/
├── unittest/              # 单元测试 (Google Test)
│   ├── src/               # 测试源文件
│   └── include/           # 测试头文件
├── fuzztest/              # Fuzz 测试
│   ├── ressched_fuzzer/
│   ├── loadfromconfigfile_fuzzer/
│   └── loadfromcustconfigfile_fuzzer/
└── testutil/              # 测试工具
```

### 编写单元测试

```cpp
#include <gtest/gtest.h>
#include "plugin_mgr.h"

class PluginMgrTest : public testing::Test {
protected:
    void SetUp() override {
        // 测试前准备
    }
    void TearDown() override {
        // 测试后清理
    }
};

TEST_F(PluginMgrTest, Init_Success) {
    auto& mgr = PluginMgr::GetInstance();
    EXPECT_NO_THROW(mgr.Init());
}
```

### Mock 对象

```cpp
class MockPluginMgr : public PluginMgr {
public:
    MOCK_METHOD(void, DispatchResource, (const std::shared_ptr<ResData>& data), (override));
};
```

---

## 核心工作原则

### 1. 代码验证原则

**只使用实际代码**:
- ✅ 使用 Read/Grep 工具定位和读取实际源代码
- ✅ 提供完整的文件路径和行号引用
- ❌ 不要猜测或编造代码实现

**代码引用示例**:
```markdown
Source: `ressched/services/resschedmgr/resschedfwk/src/plugin_mgr.cpp:123-145`

```cpp
void PluginMgr::DispatchResource(const std::shared_ptr<ResData>& data)
{
    for (auto& plugin : plugins_) {
        plugin->DispatchResource(data);
    }
}
```
```

### 2. 推测管理

对于不确定的内容:
- 明确标记为 "推测"
- 提供推测的理由
- 请求用户确认

```markdown
✅ 正确:
基于 plugin_mgr.cpp:123 的分析，推测插件加载顺序是...（推测）

❌ 错误:
插件加载顺序是...（未标注推测）
```

### 3. 错误学习

当发现错误时:
1. 记录错误和更正
2. 确定误解的根本原因
3. 添加预防措施
4. 在知识库中分享

```markdown
## 经验教训

### 错误: 错误的插件加载路径
**日期**: 2025-01-27
**问题**: 假设插件路径是 /system/lib/
**根本原因**: 未验证实际配置
**更正**: 实际路径是 /system/lib/resourceschedule/
**预防措施**: 始终检查配置文件中的路径定义
**参考**: ressched/profile/res_sched_config.xml:45-50
```

---

## 故障排查

### 常见问题

| 问题 | 可能原因 | 解决方法 |
|-----|---------|---------|
| 服务启动失败 | 配置文件不存在 | 检查 `ressched/profile/` 下的 XML 文件 |
| 插件加载失败 | 共享库依赖缺失 | 使用 `ldd` 检查依赖，确认路径正确 |
| 事件未接收 | 观察者未注册 | 检查 `ObserverManager::Init()` 调用 |
| 内存泄漏 | 插件未正确释放 | 确保 `Disable()` 中清理资源 |
| 高频事件卡顿 | 事件处理耗时过长 | 使用异步处理，检查 >10ms 警告 |

### 调试方法

**日志调试**:
```cpp
RESSCHED_LOGI("[Debug] Event received: type=%{public}d, value=%{public}lld", 
              resType, value);
```

**HiSysEvent 事件**:
```cpp
HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "EVENT_NAME",
    HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
    "KEY", value);
```

**查看系统日志**:
```bash
hilog | grep RSS
```

---

## 子目录指南

| 目录 | 指南文件 | 主要内容 |
|-----|---------|---------|
| `ressched/test/` | [test/AGENTS.md](ressched/test/AGENTS.md) | 单元测试、Fuzz 测试、测试工具 |
| `ressched/plugins/` | [plugins/AGENTS.md](ressched/plugins/AGENTS.md) | 插件架构、内置插件、开发指南 |
| `ressched/scene_recognize/` | [scene_recognize/AGENTS.md](ressched/scene_recognize/AGENTS.md) | 场景识别框架、识别器实现 |
| `ressched/services/` | [services/AGENTS.md](ressched/services/AGENTS.md) | 服务层、调度框架、插件基类 |
| `ressched/sched_controller/` | [sched_controller/AGENTS.md](sched_controller/AGENTS.md) | 事件采集、观察者系统 |
| `ressched/interfaces/` | [interfaces/AGENTS.md](interfaces/AGENTS.md) | 多语言接口、IDL 定义 |
| `ressched/common/` | [common/AGENTS.md](common/AGENTS.md) | 公共工具、日志、缓存 |

---

## 参考资源

- **架构图**: `figures/resource_schedule_service_architecture_ZH.png`
- **README**: `README_ZH.md`
- **进程管理规范**: `Resource_Schedule_Service_Process_Management_Specifications_ZH.md`
- **代码OWNERS**: `CODEOWNERS`
