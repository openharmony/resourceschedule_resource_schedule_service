# 术语表

## 持久模式（persistMode / switch）

资源节点的持久化策略，由 XML `switch` 属性指定。写节点模式（`WRITE_NODE`=0）直接写内核 sysfs 节点；上报模式（`REPORT_TO_PERFSO`=1）仅将频率请求转发给 SoC 性能服务（SA 1906），不写节点也不解析 path。上报模式的资源 ID 范围扩展到 10000-10999（`RES_ID_NUMS_PER_TYPE_EXT`），写节点模式为 1000-5999（`RES_ID_NUMS_PER_TYPE`）。

## GovResource（Governor 资源）

需要多路径多级别映射的资源类型。`GovResNode` 含 `paths`（多个内核节点路径）和 `levelToStr`（level → 各路径对应字符串值的映射）。写入时按 level 查 `levelToStr` 获取每个路径要写的字符串，`|` 分隔多路径值。与普通 `ResNode`（单路径写数值）不同，GovResNode 一次写入可能涉及多个内核节点，每个节点写不同字符串值。

## 资源配对（pair）

普通资源节点的联动机制。`ResNode.pair` 指向另一个资源 ID，用于频率资源的开始/停止配对（如最小频率配最大频率）。`pair` 值非 `INVALID_VALUE`(-1) 时，配置加载阶段通过 `CheckPairResIdValid` 校验 pairResId 必须已定义，否则加载失败。

## fd 缓存（fdInfo_）

`SocPerfExecutorWirteNode` 维护的文件描述符缓存（`map<string, int32_t>`）。初次写节点时 `open(path, O_RDWR | O_CLOEXEC)` 打开并缓存 fd，后续写入复用 fd 仅 `lseek + write`。fd 通过 `fdsan_exchange_owner_tag` 打上 `SCHEDULE_CGROUP_FDSAN_TAG` 标签防止误关闭，析构时用 `fdsan_close_with_tag` 释放。
