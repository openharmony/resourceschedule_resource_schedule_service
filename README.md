# Resource Schedule Service

- [Resource Schedule Service](#resource-schedule-service)
  - [Introduction<a name="section11660541593"></a>](#introduction)
  - [Directory Structure<a name="section161941989596"></a>](#directory-structure)
  - [How to write a plugin<a name="section1312121216216"></a>](#how-to-write-a-plugin)
    - [Available APIs<a name="section114564657874"></a>](#available-apis)
    - [Usage Guidelines<a name="section129654513264"></a>](#usage-guidelines)
      - [Restrictions on Using Transient Tasks<a name="section1551164914237"></a>](#restrictions-on-using-transient-tasks)
  - [Cgroup Schedule Policy](#cgroup-schedule-policy)
    - [Basic Policy](#basic-policy)
    - [Policy Configuration](#policy-configuration)
    - [Restrictions](#restrictions)
  - [Repositories Involved<a name="section1371113476307"></a>](#repositories-involved)

## Introduction<a name="section11660541593"></a>

In the resourceschedule subsystem, it provides the awareness and distribution of system events, such as application start, exit, screen on and off, etc.
If you need to obtain system events and perform related resource schedule, you can choose to join the resource schedule service in the form of a plugin.

In resource schedule subsystem, the module of cgroup schedule decides the group schedule policy of each process by analyzing application state, window status and background task status. And with proper configuration, it can bind each process to certain cgroup node. These schedule policies can be used to gain better performance. And this module forward different kinds of events to plugin manager and then be distributed to subscribers.

## Directory Structure<a name="section161941989596"></a>

```
├── cgroup_sched
|   ├── common
|   │   └── include                   # common header files
|   ├── framework
|   │   ├── process_group             # utils to set cgroup by pid
|   │   ├── sched_controller          # cgroup schedule
|   │   └── utils                     # adaption interface to forwarding data
|   │       ├── include
|   │       │   └── ressched_utils.h
|   │       └── ressched_utils.cpp
|   ├── interfaces                    # Init/Denit/Query interface for rss
|   │   └── innerkits
|   └── profiles
└── ressched
    ├── common                     # Common header file
    ├── interfaces
    │   └── innerkits              # Interface APIs
    │       └── ressched_client    # Report data in process
    ├── plugins                    # Plugin code
    ├── profile                    # Plugin switch xml and plugin private xml
    ├── sa_profile                 # System ability xml
    └── services 
        ├── resschedmgr
        │   ├── pluginbase         # Plugin struct definition
        │   └── resschedfwk        # Resource schedule framework
        └── resschedservice        # Resource schedule service

```
## How to write a plugin<a name="section1312121216216"></a>

### Available APIs<a name="section114564657874"></a>

| API                                                                           | Description                      |
|-------------------------------------------------------------------------------|----------------------------------|
| function OnPluginInit(std::string& libName): bool;                            | plugin init                      |
| function OnPluginDisable(): void;                                             | plugin disable                   |
| function OnDispatchResource(const std::shared_ptr<ResData>& data):void;       | dispatch resource event          |

### Usage Guidelines<a name="section129654513264"></a>

When the plugin is initialized, specify the events that need to be registered for monitoring. When these events occur, the framework will be distributed to each plugin in turn,

At this point, the plugin needs to quickly process message reception for resource schedule (if time-consuming tasks need to be processed by another thread), and the processing is completed, return.

#### Restrictions on Using Transient Tasks<a name="section1551164914237"></a>

1. The plugin can be implemented with C/C++.

2. The event processing of the plugin must be completed quickly. If it exceeds 1ms, warning will be printed.
If it exceeds 10ms, the framework thinks the plugin is abnormal and reports an error.

## Cgroup Schedule Policy

### Basic Policy

| Scene  | Schedule policy  |
|----------|-------|
| Currently focused oreground application and processes with same uid  | top_app  |
| Foreground visible processes, include unfocused window process in split mode, float window process, foundation process  | foreground  |
| system level daemon processes. Assign system schedule policy to them when started,   | system  |
| Background application and processes with same uid | background |
| kernel processes, most native processes and others have not mensioned | root |

### Policy Configuration

Each schedule policy is configured in a json file, and is bound to different cgroup.
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

### Restrictions

Configuration file: cgroup_action_config.json
Every schedule policy defined should be configured.

Introduction to each config item：

| Item | Description |
|--------|--------|
|controller|cgroup controller: cpuset, cpuctl, blkio etc.|
|path|Absolute path of current cgroup|
|sched_policy|Binding between each schedule policy and cgroup|
|sp_xx|Different kinds of schedule policy|

## Repositories Involved<a name="section1371113476307"></a>

- [aafwk_standard](https://gitee.com/openharmony/aafwk_standard)
- [windowmanager](https://gitee.com/openharmony/windowmanager)
- [communication_ipc](https://gitee.com/openharmony/communication_ipc)
- [hiviewdfx_hilog](https://gitee.com/openharmony/hiviewdfx_hilog)