# cgroup_sched_plugin 模块 - AI 编码代理指南

## 基本信息

### 功能描述

本组件负责系统事件与进程运行态的感知。依据 Ability 状态、窗口焦点、进程状态、音频播放、摄像头、连续任务、瞬态任务、锁屏、系统负载等系统事件，动态计算每个进程应归属的调度策略组，并写入内核 cgroup 文件系统完成实际迁移。

### 核心功能

- **事件订阅与分发**：以 `Plugin` 形式接入 ressched 框架，订阅应用状态、窗口焦点、进程创建/销毁、瞬态任务、持续任务、音频、摄像头、RunningLock 等系统事件
- **进程模型维护**：在 `Supervisor` 中维护 uid → `Application` → pid → `ProcessRecord` 的两级缓存。`Application` 持有同 uid 下所有进程的映射、焦点进程指针与宿主进程集合；`ProcessRecord` 记录进程的调度组别（last/cur/set/special）、进程类型、进程状态、能力列表、窗口列表、瞬态任务、连续任务、音频播放/采集、相机、蓝牙、Wi-Fi、MMI、运行锁、屏幕录制、视频、Nap、键线程角色、序列号、宿主 pid 等运行态字段，作为 `ComputeProcessGroup` 的唯一决策输入
- **调度组决策**：`CgroupAdjuster` 综合 `ProcessRecord` 的可见性、焦点、音频、瞬态任务、持续任务、屏幕捕获等标志，计算进程应归属的 `SchedPolicy`

## 目录结构

```
cgroup_sched_plugin/
├── common/include/
│   └── cgroup_sched_log.h              # CGS_LOGX 日志宏、ChronoScope 耗时统计
├── interfaces/innerkits/include/
│   └── cgroup_sched.h                  # 公共头：extern "C" GetProcessGroup
├── framework/
│   ├── sched_controller/
│   │   ├── include/
│   │   │   ├── sched_controller.h      # Plugin 实现
│   │   │   ├── supervisor.h            # 进程模型（Application/ProcessRecord/WindowInfo）
│   │   │   ├── cgroup_adjuster.h       # 调度组决策与ApplySource 枚举
│   │   │   └── cgroup_event_handler.h  # 事件处理与 ffrt 队列
│   │   ├── sched_controller.cpp        # 事件订阅与分发路由
│   │   ├── supervisor.cpp              # AppManager 接入与进程记录查询
│   │   ├── cgroup_adjuster.cpp         # ComputeProcessGroup/ApplyProcessGroup
│   │   └── cgroup_event_handler.cpp    # 各 ResType 的 Handle* 实现
│   ├── process_group/
│   │   ├── include/
│   │   │   ├── sched_policy.h          # SchedPolicy 枚举与 API
│   │   │   ├── cgroup_action.h         # CgroupAction 单例门面
│   │   │   ├── cgroup_controller.h     # 单 controller 抽象
│   │   │   └── cgroup_map.h            # controller 注册表
│   │   └── src/                        # 配置加载与 cgroup 写入实现
│   └── utils/
│       ├── include/ressched_utils.h    # 扩展库 dlopen 与 IPC 转发
│       └── ressched_utils.cpp
├── profiles/
│   ├── cgroup_action_config.json       # cgroup 路径与策略映射（装到 etc/cgroup_sched/）
│   └── BUILD.gn
├── test/
│   ├── unittest/                       # ProcessGroupSubTest、ResschedUtilsTest
│   └── fuzztest/cgroupsched_fuzzer/    # CgroupSchedFuzzTest
├── cgroup_sched.gni                    # GN 变量与 power_manager 开关
└── AGENTS.md                           # 本文件
```

## 知识索引

稳定背景知识放在 `docs/knowledge/`，改动前按场景读取对应文件：

| 场景 | 先读 |
|------|------|
| 理解业务术语 | docs/knowledge/glossary.md |
| 业务背景与系统上下文 | docs/knowledge/business-context.md |
| 架构设计 | docs/knowledge/architecture.md |
| 编码前必读 | docs/knowledge/coding-standards.md |
| 业务实体与策略计算 | docs/knowledge/cgroup-Entity.md |
| 技术框架与并发/双通道/扩展库/配置 | docs/knowledge/cgroup-Technology.md |
