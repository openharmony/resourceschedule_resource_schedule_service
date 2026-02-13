# CLAUDE.md

## 架构概览

**资源调度服务** (`@ohos/resource_schedule_service`) 是 OpenHarmony 系统级服务，通过智能调度管理 CPU、GPU 和功耗资源。架构分为两个主要服务：

### 主服务 (`ressched/`)

采用插件架构的核心资源调度器：

- **入口点**: `services/resschedservice/src/res_sched_service.cpp`
  - 实现 System Ability 服务 (`ResourceScheduleService`)
  - 通过 `PluginMgr` 加载和管理所有插件

- **框架层** (`services/resschedmgr/resschedfwk/`):
  - `plugin_mgr.cpp`: 使用 `dlopen()` 动态加载插件
  - `event_listener_mgr.cpp`: 管理事件订阅和分发
  - `config_reader.cpp`: 读取 XML 配置文件
  - `res_sched_mgr.cpp`: 中央调度协调器

- **事件控制器** (`sched_controller/`):
  - `common_event/`: 接收系统级公共事件
  - `observer/`: 基于回调的事件监听器：
    - `audio_observer.cpp`: 音频框架事件（渲染状态、铃声模式、音量）
    - `camera_observer.cpp`: 相机状态变化
    - `window_state_observer.cpp`: 窗口焦点/可见性事件
    - `app_state_observer.cpp`: 应用生命周期事件
    - `telephony_observer.cpp`: 电话状态（条件编译）
    - `display_power_event_observer.cpp`: 屏幕开关事件

- **场景识别** (`scene_recognize/`):
  - 复杂场景的模式检测，如应用安装、升级、重叠任务

### 执行器服务 (`ressched_executor/`)

针对性能关键操作的分离架构：
- 独立服务 (`resschedexesvc`) 处理 SOC 频率调制
- `plugins/socperf_executor_plugin/`: CPU/GPU 频率请求的专用执行器

---

## 插件开发

### 插件接口定义

插件必须实现以下 API（定义于 `services/resschedmgr/pluginbase/include/plugin.h`）：

```cpp
class Plugin {
    virtual void Init() = 0;
    virtual void Disable() = 0;
    virtual void DispatchResource(const std::shared_ptr<ResData>& data) = 0;
};
```

| API | 描述 |
|-----|------|
| `Init()` | 插件初始化 |
| `Disable()` | 插件禁用 |
| `DispatchResource(const std::shared_ptr<ResData>& data)` | 接收并处理资源事件 |

### 开发规范

1. **实现语言**: 插件使用 C/C++ 实现

2. **事件订阅**: 插件初始化时指定需要监听的事件，框架自动分发匹配事件

3. **处理原则**:
   - 事件处理必须快速完成
   - 如有耗时任务，需由独立线程处理
   - 处理完成后立即返回

4. **性能要求**:
   - 超过 1ms 打印警告
   - 超过 10ms 视为插件异常并报错

### 内置插件列表

| 插件 | 功能 |
|------|------|
| `cgroup_sched_plugin/` | 基于 Linux cgroup 的进程调度 |
| `socperf_plugin/` | SOC 性能管理（CPU/GPU 频率） |
| `device_standby_plugin/` | 设备待机状态管理 |
| `frame_aware_plugin/` | 帧感知调度（条件编译） |

---

## 配置文件

| 配置文件 | 路径 | 用途 |
|----------|------|------|
| 插件开关 | `ressched/profile/res_sched_plugin_switch.xml` | 控制加载哪些插件 |
| Cgroup 策略 | `plugins/cgroup_sched_plugin/profiles/cgroup_action_config.json` | 调度策略映射到 cgroup 路径 |
| 框架配置 | `ressched/profile/res_sched_config.xml` | 框架级配置 |

---

## Cgroup 调度策略

### 策略映射

| 场景 | 调度策略 |
|------|----------|
| 当前聚焦的前台应用及同 uid 进程 | `top_app` |
| 前台可见进程（分屏未聚焦窗口、悬浮窗、foundation 进程） | `foreground` |
| 系统级守护进程 | `system` |
| 后台应用及同 uid 进程 | `background` |
| 内核/原生进程 | `root` |

### 配置项说明

配置文件: `cgroup_action_config.json`

| 配置项 | 描述 |
|--------|------|
| `controller` | cgroup 控制器类型：`cpuset`、`cpuctl`、`blkio` 等 |
| `path` | cgroup 绝对路径 |
| `sched_policy` | 调度策略与 cgroup 的绑定关系 |
| `sp_xx` | 具体策略类型（如 `sp_top_app`、`sp_background`） |

---

## SocPerf

### 接口列表

| 接口 | 功能 |
|------|------|
| `PerfRequest(int32_t cmdId, const std::string& msg)` | 性能提升频率 |
| `PerfRequestEx(int32_t cmdId, bool onOffTag, const std::string& msg)` | 性能提升（支持 ON/OFF） |
| `PowerLimitBoost(bool onOffTag, const std::string& msg)` | 功耗限制频率 |
| `ThermalLimitBoost(bool onOffTag, const std::string& msg)` | 热限制频率 |
| `LimitRequest(int32_t clientId, const std::vector<int32_t>& tags, const std::vector<int64_t>& configs, const std::string& msg)` | 功耗/热限制（可批量设置） |

**参数说明**:
- `cmdId`: 连接场景和配置的关键参数，用于提升或限制频率
- `onOffTag`: 支持 ON/OFF 切换，用于长期事件
- `msg`: 额外信息（如客户端 pid、tid）

### SocPerf 配置

| 文件 | 描述 |
|------|------|
| `socperf_resource_config.xml` | 可修改资源定义（CPU/GPU/DDR/NPU） |
| `socperf_boost_config.xml` | 性能提升配置 |
| `socperf_thermal_config.xml` | 热限制配置 |

**注意**: 三个配置文件中的 `cmdID` 必须互不相同，且针对不同产品内容可能不同。

---

## 回调事件

当前支持的框架事件：

- **音频框架**: renderState 变化、ringMode 变化、volumeKey 变化
- **电话**: 电话状态变化
- **相机**: 相机状态变化

---

## 数据流

```
事件源 → EventController → EventListenerMgr → 插件
                                    ↓
                            根据 resid 订阅分发
                                    ↓
                         调用 SocPerf API / 设置 cgroup
                                    ↓
                         CPU/GPU 频率调整 / 进程优先级变更
```

1. **接收**: 通过 `common_event` 订阅者或 `observer` 回调
2. **转发**: EventController 转发到 EventListenerMgr
3. **分发**: 根据 `resid` 分发给订阅的插件
4. **处理**: 插件调用相应 API
5. **执行**: 完成调度动作

---

## 特性开关

构建特性由 `ressched.gni` 中的 `declare_args()` 控制：

| 特性开关 | 功能 |
|----------|------|
| `ressched_with_telephony_state_registry_enable` | 电话支持 |
| `ressched_with_communication_netmanager_base_enable` | 网络管理器 |
| `resource_schedule_service_with_ffrt_enable` | FFRT 支持 |
| `resource_schedule_service_with_ext_res_enable` | 扩展资源 |
| `resource_schedule_service_with_app_nap_enable` | 应用小憩 |
| `resource_schedule_service_cust_soc_perf_enable` | 客户化 SOC perf |
| `resource_schedule_service_subscribe_click_recognize_enable` | 点击识别 |
| `resource_schedule_service_has_sys_nice_enable` | 系统 nice |
| `resource_schedule_service_depend_wm_enable` | 窗口管理器依赖 |
| `ressched_frame_aware_sched_enable` | 帧感知调度插件 |

---

## IPC 和客户端 API

- **客户端库**: `interfaces/innerkits/ressched_client/` - 进程内上报 API
- **IPC**: 使用 OpenHarmony IPC/SA 框架 (`samgr`、`ipc_single`)
- **语言绑定**: `interfaces/kits/js/napi/` - NAPI 绑定（JS 应用）

---

## 构建系统

使用 GN (Generate Ninja) 构建的 OpenHarmony 系统服务。配置文件: `ressched/ressched.gni`

### 构建命令

```bash
# 在 OpenHarmony 根目录执行
hb build -f resource_schedule_service
```

### 测试

```bash
# 运行单元测试
./resource_schedule_service/ressched/test/unittest/resschedfwk_test
./resource_schedule_service/ressched/test/unittest/resschedservice_test
./resource_schedule_service/ressched/test/unittest/resschedservice_mock_test

# 重新构建测试
hb build -f resource_schedule_service_unittest
```

测试目标定义于各模块的 `test/unittest/BUILD.gn` 和 `test/fuzztest/BUILD.gn`

---

## 相关仓库

- [windowmanager](https://gitee.com/openharmony/windowmanager)
- [communication_ipc](https://gitee.com/openharmony/communication_ipc)
- [hiviewdfx_hilog](https://gitee.com/openharmony/hiviewdfx_hilog)
