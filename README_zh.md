# 资源调度服务

-   [简介](#section11660541593)
-   [目录](#section161941989596)
-   [如何编写一个插件](#section1312121216216)
    -   [接口说明](#section114564657874)
    -   [使用说明](#section129654513264)

-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

在资源调度子系统中，提供系统事件的感知以及分发，例如应用启动、退出、亮灭屏等。如果需要获取系统事件，并且进行相关资源调度，那么可以选择以插件形式加入资源调度服务中。

## 目录<a name="section161941989596"></a>

```
── ressched
    ├── common                     # 公共头文件
    ├── interfaces
    │   └── innerkits              # 对外接口目录
    │       └── ressched_client    # 外部同步事件通知接口
    ├── plugins                    # 插件代码实现
    ├── profile                    # 插件开关以及私有配置
    ├── sa_profile                 # 系统元能力配置
    └── services 
        ├── resschedmgr
        │   ├── pluginbase         # 插件结构定义
        │   └── resschedfwk        # 资源调度服务框架实现
        └── resschedservice        # 资源调度服务层

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

## 相关仓<a name="section1371113476307"></a>

资源调度子系统

**resource\_schedule\_service**

safwk

appexecfwk_standard

ipc
