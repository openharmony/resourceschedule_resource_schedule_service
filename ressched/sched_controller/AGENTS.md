# 资源调度服务 - 调度控制器模块

本文件为 Claude Code 提供 RSS 调度控制器模块的开发指导。

---

## 快速参考

### 编译命令

```bash
# 调度控制器作为 RSS 服务的一部分编译
hb build ressched/services:resschedsvc

# 编译单元测试
hb build ressched/sched_controller/test/unittest:event_controller_test

# 编译 Fuzz 测试
hb build ressched/sched_controller/test/fuzztest/resschedobserver_fuzzer:fuzztest
hb build ressched/sched_controller/test/fuzztest/resschedcommonevent_fuzzer:fuzztest
```

### 观察者源文件

```bash
ressched/sched_controller/observer/src/
├── observer_manager.cpp          # 观察者管理器
├── app_state_observer.cpp        # 应用状态观察
├── audio_observer.cpp            # 音频状态观察
├── window_state_observer.cpp     # 窗口状态观察
├── mmi_observer.cpp              # 多模输入观察
├── sched_telephony_observer.cpp  # 电话状态观察
├── background_task_observer.cpp  # 后台任务观察
└── ...                           # 其他观察者
```

---

## 概述

调度控制器模块负责采集系统和应用的各种事件，是资源调度服务（RSS）的"感知层"。通过监听系统各种状态变化和用户行为，为上层调度策略提供数据支持。

## 目录结构

```
sched_controller/
├── common_event/           # 公共事件处理
│   ├── src/                # 源文件
│   └── include/            # 头文件
├── observer/               # 事件观察者
│   ├── src/                # 源文件
│   └── include/            # 头文件
└── test/                   # 测试
    ├── unittest/           # 单元测试
    └── fuzztest/           # Fuzz 测试
```

## 公共事件 (common_event)

### 功能

处理系统公共事件，将系统事件转换为 RSS 内部事件格式。

### 核心类

| 类名 | 文件 | 功能 |
|------|------|------|
| ResSchedCommonEvent | `res_sched_common_event.cpp` | 公共事件处理器 |

### 事件类型

处理的主要公共事件：
- 屏幕状态变化（亮屏/灭屏）
- 电池状态变化
- 充电状态变化
- 网络连接变化
- 时区变化

### 处理流程

```
系统公共事件 → ResSchedCommonEvent → 转换为 ResData → 分发给 PluginMgr
```

## 事件观察者 (observer)

### 观察者管理器

**ObserverManager** - 统一管理所有观察者：

```cpp
class ObserverManager {
public:
    static ObserverManager& GetInstance();
    void Init();        // 初始化所有观察者
    void Disable();     // 停用所有观察者
    
private:
    void InitAppStateObserver();
    void InitAudioObserver();
    // ... 其他观察者初始化
};
```

### 观察者类型

| 观察者 | 文件 | 监听内容 | 输出事件 |
|--------|------|----------|----------|
| AppStateObserver | `app_state_observer.cpp` | 应用状态变化 | RES_TYPE_APP_STATE_CHANGE |
| AudioObserver | `audio_observer.cpp` | 音频状态 | RES_TYPE_AUDIO_RENDER_STATE_CHANGE |
| WindowStateObserver | `window_state_observer.cpp` | 窗口状态 | RES_TYPE_WINDOW_FOCUS |
| MmiObserver | `mmi_observer.cpp` | 输入事件 | RES_TYPE_CLICK_RECOGNIZE, RES_TYPE_SLIDE_RECOGNIZE |
| TelephonyObserver | `sched_telephony_observer.cpp` | 通话状态 | RES_TYPE_CALL_STATE_UPDATE |
| AccountObserver | `account_observer.cpp` | 账户变化 | RES_TYPE_ACCOUNT_STATE_CHANGE |
| BackgroundTaskObserver | `background_task_observer.cpp` | 后台任务 | RES_TYPE_BACKGROUND_TASK_STATE_CHANGE |
| AVSessionStateListener | `av_session_state_listener.cpp` | 音视频会话 | RES_TYPE_AV_SESSION_STATE_CHANGE |
| ConnectionSubscriber | `connection_subscriber.cpp` | 连接状态 | RES_TYPE_CONNECTION_STATE_CHANGE |
| DisplayPowerEventObserver | `display_power_event_observer.cpp` | 显示电源 | RES_TYPE_SCREEN_STATUS |
| DownloadUploadObserver | `download_upload_observer.cpp` | 下载上传 | RES_TYPE_DOWNLOAD_UPLOAD_STATE |
| FoldDisplayModeObserver | `fold_display_mode_observer.cpp` | 折叠屏模式 | RES_TYPE_FOLD_DISPLAY_MODE_CHANGE |
| HisysEventObserver | `hisysevent_observer.cpp` | 系统事件 | 多种事件类型 |
| AppStartupSceneRec | `app_startup_scene_rec.cpp` | 应用启动场景 | 场景识别事件 |

### 详细说明

#### 1. 应用状态观察者 (AppStateObserver)

**监听事件**：
- 应用启动
- 应用退出
- 应用前后台切换

**数据结构**：
```cpp
struct AppStateData {
    int32_t uid;           // 应用 UID
    int32_t pid;           // 进程 PID
    std::string bundleName; // 包名
    int32_t state;         // 状态 (FOREGROUND/BACKGROUND/TERMINATED)
};
```

#### 2. 音频观察者 (AudioObserver)

**监听事件**：
- 音频播放开始/停止
- 音频焦点变化
- 音量变化

**适用场景**：
- 音乐播放识别
- 通话状态检测
- 录音状态监控

#### 3. 窗口状态观察者 (WindowStateObserver)

**监听事件**：
- 窗口获得/失去焦点
- 窗口大小变化
- 窗口可见性变化

**输出**：
- 焦点窗口 PID
- 窗口类型
- 显示区域信息

#### 4. 多模输入观察者 (MmiObserver)

**监听事件**：
- 点击事件
- 滑动事件
- 按键事件

**数据处理**：
- 识别手势类型
- 计算滑动速度
- 检测用户交互模式

#### 5. 电话观察者 (SchedTelephonyObserver)

**监听事件**：
- 通话状态变化
- 来电/去电
- 通话结束

**通话状态**：
```cpp
enum CallState {
    CALL_STATE_IDLE = 0,      // 空闲
    CALL_STATE_RINGING,       // 响铃
    CALL_STATE_ACTIVE,        // 通话中
    CALL_STATE_DIALING,       // 拨号中
    CALL_STATE_DISCONNECTED   // 断开
};
```

#### 6. 后台任务观察者 (BackgroundTaskObserver)

**监听事件**：
- 后台任务提交
- 后台任务完成
- 长时任务状态

**任务类型**：
- 数据传输
- 音频播放
- 位置更新
- 蓝牙连接

#### 7. 显示电源观察者 (DisplayPowerEventObserver)

**监听事件**：
- 屏幕点亮
- 屏幕熄灭
- 屏幕状态查询

#### 8. HisysEvent 观察者 (HisysEventObserver)

**功能**：
- 监听系统 HisysEvent
- 将特定事件转换为 RSS 事件
- 支持事件过滤和转换

## 事件上报流程

```
系统服务（AMS/WMS/Audio等）
    ↓
观察者回调
    ↓
转换为 ResData
    ↓
调用 ResSchedClient::GetInstance().ReportData()
    ↓
IPC 调用 ResSchedService
    ↓
PluginMgr::DispatchResource()
    ↓
分发给订阅插件
```

## 配置与开关

### 观察者开关

部分观察者可通过配置启用/禁用：

```xml
<observer>
    <app_state enable="true"/>
    <audio enable="true"/>
    <telephony enable="true"/>
    <!-- ... -->
</observer>
```

### 编译开关

在 `ressched.gni` 中控制：

```gn
ressched_with_telephony_state_registry_enable = true
ressched_with_resourceschedule_multimedia_audio_framework_enable = true
rss_mmi_enable = true
```

## 性能考虑

### 事件频率控制

- 高频事件节流（如触摸事件）
- 批量处理相似事件
- 避免重复上报

### 内存管理

- 观察者生命周期管理
- 缓存大小限制
- 及时清理过期数据

### 线程安全

- 回调函数线程安全
- 共享数据加锁保护
- 避免死锁

## 调试方法

### 日志输出

```cpp
RESSCHED_LOGI("[Observer] Event received: %{public}d", eventType);
```

### 事件追踪

```cpp
HitraceScoped trace("AppStateObserver::OnForegroundApplicationChanged");
```

### 事件统计

使用 HiSysEvent 记录事件统计：
```cpp
HiSysEventWrite(HiviewDFX::HiSysEvent::Domain::RSS, "OBSERVER_EVENT_STAT",
    HiviewDFX::HiSysEvent::EventType::STATISTIC,
    "EVENT_TYPE", resType,
    "COUNT", count);
```

## 添加新观察者

### 步骤

1. **创建观察者类**

```cpp
class MyObserver : public IMyServiceCallback {
public:
    void OnEvent(const EventData& data);
    void Init();
    void Disable();
};
```

2. **在 ObserverManager 中注册**

```cpp
void ObserverManager::Init() {
    // ... 其他观察者
    InitMyObserver();
}

void ObserverManager::InitMyObserver() {
    myObserver_ = new MyObserver();
    // 订阅系统服务回调
}
```

3. **实现事件转换**

```cpp
void MyObserver::OnEvent(const EventData& data) {
    nlohmann::json payload;
    payload["key"] = data.value;
    
    ResSchedClient::GetInstance().ReportData(
        ResType::RES_TYPE_MY_EVENT, 
        data.value, 
        payload.dump());
}
```

## 测试

### 单元测试

测试文件位于 `test/unittest/`：
- 模拟系统服务回调
- 验证事件转换逻辑
- 测试边界条件

### Fuzz 测试

测试文件位于 `test/fuzztest/`：
- `resschedobserver_fuzzer` - 观察者 fuzz 测试
- `resschedcommonevent_fuzzer` - 公共事件 fuzz 测试

## 常见问题

1. **观察者初始化失败**：检查系统服务是否可用
2. **事件丢失**：确认观察者已正确注册
3. **高频事件导致卡顿**：实现事件节流机制
4. **内存增长**：检查观察者是否正确释放

## 编译方法

### BUILD.gn 配置

调度控制器模块的编译配置位于 `ressched/sched_controller/` 及其子目录的 BUILD.gn 文件中。

### 编译命令

```bash
# 编译 RSS 服务（包含调度控制器）
hb build ressched/services:resschedsvc

# 编译单元测试
hb build ressched/sched_controller/test/unittest:event_controller_test

# 编译 Fuzz 测试
hb build ressched/sched_controller/test/fuzztest/resschedobserver_fuzzer:fuzztest
hb build ressched/sched_controller/test/fuzztest/resschedcommonevent_fuzzer:fuzztest

# 编译所有调度控制器测试
hb build ressched/sched_controller/test/unittest:unittest
hb build ressched/sched_controller/test/fuzztest:fuzztest
```

### 模块结构编译

```bash
# 公共事件模块（包含在 resschedsvc 中）
# 源文件：common_event/src/res_sched_common_event.cpp

# 观察者模块（包含在 resschedsvc 中）
# 源文件：observer/src/*.cpp
```

### 作为依赖使用

在 BUILD.gn 中引用调度控制器：

```gn
deps = [
  "${ressched_services}:resschedsvc",
]

include_dirs = [
  "${ressched_sched_controller}/observer/include",
  "${ressched_sched_controller}/common_event/include",
]
```

### 编译选项

单元测试专用选项：
```gn
cflags = [
  "-Dprivate=public",
  "-Dprotected=public",
]

defines = [ "OBSERVER_EVENT_TEST" ]
```

### 条件编译

根据功能开关条件编译：
```gn
if (ressched_with_telephony_state_registry_enable) {
  defines += [ "RESSCHED_TELEPHONY_STATE_REGISTRY_ENABLE" ]
  external_deps += [
    "core_service:tel_core_service_api",
    "state_registry:tel_state_registry_api",
  ]
}

if (ressched_with_resourceschedule_multimedia_audio_framework_enable) {
  defines += [ "RESSCHED_AUDIO_FRAMEWORK_ENABLE" ]
  external_deps += [
    "audio_framework:audio_client",
    "audio_framework:audio_foundation",
  ]
}

if (communication_bluetooth_perception_enable) {
  defines += [ "RESSCHED_COMMUNICATION_BLUETOOTH_ENABLE" ]
  external_deps += [ "bluetooth:btframework" ]
}

if (rss_mmi_enable) {
  defines += [ "MMI_ENABLE" ]
  external_deps += [ "input:libmmi-client" ]
}
```

### 外部依赖

调度控制器依赖的系统服务：
```gn
external_deps = [
  "ability_runtime:app_manager",           # 应用管理器
  "ability_runtime:connection_obs_manager", # 连接观察者
  "background_task_mgr:bgtaskmgr_innerkits", # 后台任务
  "bundle_framework:appexecfwk_base",      # Bundle 框架
  "common_event_service:cesfwk_innerkits", # 公共事件
  "input:libmmi-client",                   # 多模输入
  "ipc:ipc_single",                        # IPC
  "os_account:os_account_innerkits",       # OS 账户
  "samgr:samgr_proxy",                     # SA 管理器
  "window_manager:libwm",                  # 窗口管理
]
```

### 安全编译选项

```gn
sanitize = {
  cfi = true
  cfi_cross_dso = true
  debug = false
}
branch_protector_ret = "pac_ret"
```

### 源文件列表

| 源文件 | 说明 |
|--------|------|
| `res_sched_common_event.cpp` | 公共事件处理 |
| `app_state_observer.cpp` | 应用状态观察 |
| `audio_observer.cpp` | 音频状态观察 |
| `window_state_observer.cpp` | 窗口状态观察 |
| `mmi_observer.cpp` | 多模输入观察 |
| `sched_telephony_observer.cpp` | 电话状态观察 |
| `background_task_observer.cpp` | 后台任务观察 |
| `observer_manager.cpp` | 观察者管理器 |
| `display_power_event_observer.cpp` | 显示电源观察 |
| `hisysevent_observer.cpp` | HisysEvent 观察 |
