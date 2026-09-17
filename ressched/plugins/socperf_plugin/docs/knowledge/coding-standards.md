# 编码铁律

## 事件处理

- **禁止**在事件处理流程中执行耗时阻塞操作，采用快速校验后下发命令的模式（超过 10ms 告警，超过 50ms 触发插件修复）

- **禁止**未校验 payload 直接访问字段，采用依次检查 `!= nullptr` → `contains(key)` → `is_string()`/`is_number_integer()` 的三步校验

## 日志

- **禁止**直接使用 HILOG 宏，采用 SOC_PERF_LOGD/LOGI/LOGW/LOGE/LOGF 宏

## 并发安全

- **禁止**在 ffrt 异步任务中不加锁访问屏幕状态，采用 screenMutex_（ffrt::mutex）保护

## 配置与订阅

- **禁止**云端配置解析异常时部分应用，采用整体回滚保证配置一致性

## 调频命令

- **禁止**配对调频（PerfRequestEx）的停止请求遗漏，采用开始/停止成对下发

- **禁止**限频阈值与上次相同时重复下发限频请求，采用 lastBatteryLimitCap_ 去重

## 单例与命名

- **禁止**手动实现单例，采用 DECLARE_SINGLE_INSTANCE / IMPLEMENT_SINGLE_INSTANCE 宏
