# 资源调度服务

- [资源调度服务](#资源调度服务)
  - [简介<a name="section11660541593"></a>](#简介)
  - [目录<a name="section161941989596"></a>](#目录)
  - [如何编写一个插件<a name="section1312121216216"></a>](#如何编写一个插件)
    - [接口说明<a name="section114564657874"></a>](#接口说明)
    - [使用说明<a name="section129654513264"></a>](#使用说明)
      - [插件事件处理约束<a name="section1551164914237"></a>](#插件事件处理约束)
  - [分组策略](#分组策略)
    - [基础分组策略](#基础分组策略)
    - [策略配置](#策略配置)
    - [配置约束](#配置约束)
  - [统一调频](#统一调频)
    - [调频接口说明](#调频接口说明)
    - [调频配置说明](#调频配置说明)
  - [相关仓<a name="section1371113476307"></a>](#相关仓)

## 简介<a name="section11660541593"></a>

在资源调度子系统中，提供系统事件的感知以及分发，例如应用启动、退出、亮灭屏等。如果需要获取系统事件，并且进行相关资源调度，那么可以选择以插件形式加入资源调度服务中。

作为资源调度子系统的子模块，智能分组模块通过系统内应用前后台切换、用户焦点输入、后台任务的执行状态，决策进程的分组调度策略，并支持通过配置将调度策略映射到不同的CGROUP分组，为系统的性能、功耗均衡调度提供决策依据。同时该模块向资源调度框架转发应用状态、焦点状态、后台任务状态等系统事件，供插件订阅。

## 目录<a name="section161941989596"></a>

```
├── cgroup_sched
|   ├── common
|   │   └── include                   # 公共头文件
|   ├── framework                     # 主体代码
|   │   ├── process_group             # 分组设置接口
|   │   ├── sched_controller          # 分组计算
|   │   └── utils                     # RMS数据转发适配接口
|   │       ├── include
|   │       │   └── ressched_utils.h
|   │       └── ressched_utils.cpp
|   ├── interfaces                    # 供RSS初始化、查询分组接口
|   │   └── innerkits
|   └── profiles
└── ressched
|   ├── common                     # 公共头文件
|   ├── interfaces
|   │   └── innerkits              # 对外接口目录
|   │       └── ressched_client    # 外部同步事件通知接口
|   ├── plugins                    # 插件代码实现
|   ├── profile                    # 插件开关以及私有配置
|   ├── sa_profile                 # 系统元能力配置
|   └── services
|       ├── resschedmgr
|       │   ├── pluginbase         # 插件结构定义
|       │   └── resschedfwk        # 资源调度服务框架实现
|       └── resschedservice        # 资源调度服务层
└── soc_perf
    ├── configs                     # 调频配置文件
    ├── include
    |       ├── client              # SocPerf客户端头文件
    |       ├── server              # SocPerf服务端头文件
    |       └── server              # SocPerf核心逻辑代码头文件
    ├── src
    |    ├── client                 # SocPerf客户端代码，给调用者使用的接口
    |    ├── server                 # SocPerf服务器代码，用于接受客户端发送的调频请求
    |    └── server                 # SocPerf核心业务逻辑代码，仲裁并生效最终的调频结果
    └── sa_profile                  # 系统元能力配置

```
## 如何编写一个插件<a name="section1312121216216"></a>

### 接口说明<a name="section114564657874"></a>

| 接口名                                                                        | 接口描述                         |
|-------------------------------------------------------------------------------|----------------------------------|
| function OnPluginInit(std::string& libName): bool;                            | 插件初始化                       |
| function OnPluginDisable(): void;                                             | 插件退出                         |
| function OnDispatchResource(const std::shared_ptr<ResData>& data):void;       | 获取分发的事件                   |

### 使用说明<a name="section129654513264"></a>

插件初始化的时候，指定需要注册监听的事件。在这些事件发生的时候，框架会依次分发给各个插件，
此时插件需要快速处理消息接收进行资源调度（需要有耗时任务则需另起线程处理），处理完成后返回。

#### 插件事件处理约束<a name="section1551164914237"></a>

1、插件可以用C/C++实现。

2、插件的事件处理，必须快速完成，超过1ms会打印相关告警，超过10ms，框架认为插件异常而进行插件退出操作。

## 分组策略

### 基础分组策略

| 场景描述  | 分组策略  |
|----------|-------|
| 前台焦点应用，UID相同的进程  | top_app  |
| 前台可见的，包括分屏场景下的非焦点应用进程、悬浮窗对应的进程、foundation进程等  | foreground  |
| 一些系统级后台常驻进程，不由分组计算主动设置。在rc/cfg文件中写定，直接写入system分组   | system  |
| 退到后台的前一个应用，及其相关的进程  | background |
| OS内核和其他native进程，以及上面没有涉及到的一些进程 | root |

### 策略配置

通过JSON配置文件，对每个分组策略，绑定指定的CGROUP分组。
```
  "Cgroups": [
    {
      "controller": "cpuctl",
      "path": "/dev/cpuctl",
      "sched_policy": {
        "sp_default": "",
        "sp_background": "background",
        "sp_foreground": "foreground",
        "sp_system": "system",
        "sp_top_app": "top-app"
      }
    }
  ]
```

### 配置约束

策略配置路径：cgroup_action_config.json文件; 每个sp_xxx策略都需要配置

配置参数说明：

| 配置项 | 说明 |
|--------|--------|
|controller|当前配置项对应的cgroup分组控制器|
|path|cgroup分组对应的路径|
|sched_policy|不同分组调度策略对应到的具体分组|
|sp_xx|不同分组调度策略标识|

## 统一调频

### 调频接口说明

当前可支持的调频接口说明

| 接口  | 说明  |
|----------|-------|
| PerfRequest(int cmdId, const std::string& msg) | 用于性能提频使用 |
| PerfRequestEx(int cmdId, bool onOffTag, const std::string& msg) | 用于性能提频使用且支持ON/OFF事件 |
| PowerRequest(int cmdId, const std::string& msg) | 用于功耗限频使用 |
| PowerRequestEx(int cmdId, bool onOffTag, const std::string& msg) | 用于功耗限频使用且支持ON/OFF事件 |
| PowerLimitBoost(bool onOffTag, const std::string& msg) | 用于限制boost无法突破功耗限频 |
| ThermalRequest(int cmdId, const std::string& msg) | 用于热限频使用  |
| ThermalRequestEx(int cmdId, bool onOffTag, const std::string& msg) | 用于热限频使用且支持ON  |
| ThermalLimitBoost(bool onOffTag, const std::string& msg) | 用于限制boost无法突破热限频 |

如表格所示，所有的调频接口都以cmdID为核心，将调频场景和调频参数互相关联，实现提频或者限频的功能。
带onOffTag参数的接口表示该接口支持ON/OFF的开关调频模式，一般用于生效时间不固定的长期调频事件，需要调用者手动开启或者关闭。
msg参数为拓展字符串信息，可承载例如调用客户端pid/tid等信息。

### 调频配置说明

当前configs目录下的配置文件

| 配置文件  | 说明  |
|----------|-------|
| socperf_resource_config.xml | 定义产品可支持的资源配置，例如CPU/GPU/DDR/NPU等 |
| socperf_boost_config.xml | 用于性能提频的配置文件 |
| socperf_resource_config.xml | 用于功耗限频的配置文件 |
| socperf_thermal_config.xml | 用于热限频的配置文件 |

各个xml配置文件都需要按产品定制，不同产品的配置不相同
对于指定的某产品，所有可支持配置的资源都定义在socperf_resource_config.xml内，支持单路径/多路径配置，任何资源都有唯一的resID
socperf_boost_config.xml、socperf_resource_config.xml、socperf_thermal_config.xml使用的cmdID不能重复

## 相关仓<a name="section1371113476307"></a>
- [aafwk_standard](https://gitee.com/openharmony/aafwk_standard)
- [windowmanager](https://gitee.com/openharmony/windowmanager)
- [communication_ipc](https://gitee.com/openharmony/communication_ipc)
- [hiviewdfx_hilog](https://gitee.com/openharmony/hiviewdfx_hilog)