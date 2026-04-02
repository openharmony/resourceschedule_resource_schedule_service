# 资源调度执行器 (Ressched Executor)

## 概述

**ressched_executor** 是资源调度服务的执行器模块 (SA 1918)，负责接收调度指令并直接执行底层硬件/内核操作。

**核心职责**: 接收指令 → 执行底层操作 (SoC频率调节、节点写入等)

## 架构结构

```
ressched_executor/
├── interfaces/                           # API 接口层
│   └── innerkits/
│       └── ressched_executor_client/     # 执行器客户端库
│           ├── res_sched_exe_client.h    # 客户端接口
│           ├── res_exe_type.h            # 执行类型枚举
│           ├── res_sched_exe_constants.h # 常量定义
│           └── IResSchedExeService.idl   # IPC 接口定义
├── services/                             # 服务实现层
│   ├── resschedexeservice/               # 服务主体
│   │   ├── res_sched_exe_service.h/cpp  # 执行服务实现
│   ├── resschedexemgr/                   # 执行管理器
│   │   ├── res_sched_exe_mgr.h/cpp      # 执行管理逻辑
│   └── libresschedexeservice.versionscript # 符号版本控制
├── common/                               # 公共组件
│   ├── executor_hitrace_chain.h/cpp      # HiTrace 链路追踪
│   └── res_sched_exe_log.h               # 日志宏定义
├── plugins/                              # 执行插件层
│   └── socperf_executor_plugin/          # SoC 性能执行插件
│       ├── framework/
│       │   ├── socperf_executor_plugin.h/cpp
│       │   ├── socperf_executor_config.h/cpp
│       │   ├── socperf_executor_wirte_node.h/cpp
│       │   └── socperf_common.h
│       └── libsocperf_executor_plugin.versionscript
├── sa_profile/                           # 系统能力配置
│   └── 1918.json                         # SA 1918 配置文件
└── etc/init/                             # 启动配置
    └── resource_schedule_executor.cfg    # init 启动脚本
```

## 核心接口

### 执行器客户端

```cpp
// res_sched_exe_client.h
class ResSchedExeClient {
public:
    int32_t SendRequestSync(uint32_t resType, int64_t value, 
                           const nlohmann::json& context, nlohmann::json& reply);
    void SendRequestAsync(uint32_t resType, int64_t value, 
                         const nlohmann::json& context);
};
```

### 执行类型枚举

```cpp
// res_exe_type.h
namespace ResExeType {
    constexpr uint32_t RES_TYPE_DEBUG = 0;
    constexpr uint32_t EWS_TYPE_SOCPERF_EXECUTOR_ASYNC_EVENT = 10001;
}
```

## 执行插件

### SocPerf 执行器插件

**功能**: 执行 SoC 性能调节指令，直接写入内核节点

**关键实现**: `plugins/socperf_executor_plugin/framework/src/socperf_executor_plugin.cpp`

```cpp
class SocPerfExecutorPlugin : public Plugin {
    void Init() override;      // 注册订阅、初始化节点
    void Disable() override;   // 清理订阅
    void DispatchResource(const std::shared_ptr<ResData>& data) override;
private:
    void HandleSocperfWirteNode(const std::shared_ptr<ResData>& data);
    std::unordered_map<uint32_t, int64_t> resTypeWithVal_;
    std::unordered_map<uint32_t, std::function<void(...)>> functionMap_;
};
```

## 测试

**测试路径**: `ressched_executor/test/unittest/`

**测试文件**:
- `res_sched_exe_mgr_test.cpp` - 执行管理器测试
- `res_sched_exe_service_test.cpp` - 执行服务测试
- `res_sched_exe_client_test.cpp` - 客户端测试
- `executor_hitrace_chain_test.cpp` - HiTrace 链路测试

**插件测试**: `plugins/socperf_executor_plugin/framework/test/unittest/`

## 与 ressched 的关系

ressched_executor 与主服务 ressched (SA 1901) 的协作:

```
ressched (SA 1901)                   ressched_executor (SA 1918)
┌─────────────────┐                  ┌──────────────────────┐
│ 事件采集        │                  │ 执行器服务           │
│ 场景识别        │ ──IPC请求──▶     │ 插件管理器           │
│ 插件管理器      │                  │ SoC Perf 执行插件    │
│ (策略决策)      │                  │ (直接执行)           │
└─────────────────┘                  └──────────────────────┘
                                            │
                                            ▼
                                     内核节点/硬件
```

## 配置文件

- **SA 配置**: `sa_profile/1918.json` - 系统能力声明
- **启动配置**: `etc/init/resource_schedule_executor.cfg` - init 进程启动脚本

