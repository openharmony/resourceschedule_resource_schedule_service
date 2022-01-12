# Resource Schedule Service

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [How to write a plugin](#section1312121216216)
    -   [Available APIs](#section114564657874)
    -   [Usage Guidelines](#section129654513264)
        -   [Restrictions on Using Transient Tasks](#section1551164914237)

-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

In the resourceschedule subsystem, it provides the awareness and distribution of system events, such as application start, exit, screen on and off, etc.
If you need to obtain system events and perform related resource schedule, you can choose to join the resource schedule service in the form of a plugin.

## Directory Structure<a name="section161941989596"></a>

```

── ressched
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

## Repositories Involved<a name="section1371113476307"></a>

Resource Schedule subsystem

**resource\_schedule\_service**

safwk

appexecfwk_standard

ipc
