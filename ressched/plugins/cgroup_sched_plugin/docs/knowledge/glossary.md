# 术语表

## 三层架构

本插件采用三层分层架构：消息接收层（`sched_controller.cpp`）负责接入 ressched 框架并分发事件；事件处理层（`cgroup_event_handler.cpp`）在 ffrt 队列上串行处理事件，维护 `Application`/`ProcessRecord` 状态并修改 `SchedPolicy`；执行层（`cgroup_adjuster.cpp` + `process_group/`）计算并下发策略到内核 cgroup。处理层另通过 `ResSchedUtils` 以 dlopen 加载扩展动态库作为扩展通道。

## 调度策略组（SchedPolicy）

对进程/线程归属的 cgroup 子组的逻辑抽象，取值固定为 5 类：默认（SP_DEFAULT=0）、后台（SP_BACKGROUND=1）、前台（SP_FOREGROUND=2）、系统后台（SP_SYSTEM_BACKGROUND=3）、顶级应用（SP_TOP_APP=4），上限值 SP_UPPER_LIMIT=0xff。

## 焦点进程（focusedProcess）

同一应用内当前持有窗口焦点的进程，决定该应用下顶级应用组归属。

## 宿主进程（Host Process）

渲染进程（RENDER）、GPU 进程（GPU）、子进程（CHILD）的父进程；宿主进程策略变更会联动其所有附属子进程的策略。

## 进程类型（ProcRecordType）

进程的业务分类，取值固定为 5 类：普通（NORMAL=0）、Extension（EXTENSION=1）、渲染（RENDER=2）、GPU（GPU=3）、子进程（CHILD=4）。

## 扩展通道（dlopen Extension）

事件处理层通过 `ResSchedUtils` 以 `dlopen`/`dlsym` 加载 `libresschedsvc.z.so` 与 `libcgroup_sched_ext.z.so`，获取上报、仲裁结果回报、系统事件回报、扩展分发、扩展资源订阅等函数指针并回调，符号缺失时安全返回。
