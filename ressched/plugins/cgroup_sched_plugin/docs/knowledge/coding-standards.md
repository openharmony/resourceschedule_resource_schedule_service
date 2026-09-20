# 编码铁律

## 并发模型

- **禁止**在 `CgroupEventHandlerQueue` 之外并发修改 `Supervisor`/`Application`/`ProcessRecord` 状态，采用 ffrt 队列串行化提交
- **禁止**在调度层加锁串行化进程模型访问，采用单队列隐式串行

## 错误处理

- **禁止**在扩展库符号缺失时崩溃，采用空指针保护并安全返回
- **禁止**在 cgroup 写入返回 ESRCH（线程退出中）时报错，视为成功
- **禁止**在策略下发失败时更新 `curSchedGroup` 记录，采用保持原值并记录错误日志

## 文件描述符安全

- **禁止**使用未打标签的 fd 操作 `/proc` 或 cgroup tasks/procs 文件，采用 `fdsan_exchange_owner_tag` 以 `SCHEDULE_CGROUP_FDSAN_TAG=0xd001702` 打标签，关闭时使用对应标签
