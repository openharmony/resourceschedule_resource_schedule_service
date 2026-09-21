# 编码铁律

## 事件处理

- **禁止**在事件处理流程中执行耗时阻塞操作，采用快速校验后下发命令的模式（超过 10ms 告警，超过 50ms 触发插件修复）

- **禁止**未校验 payload 直接访问字段，采用依次检查 `!= nullptr` → `is_null()` 的校验；resIdVec 和 valueVec 大小必须一致

## 日志

- **禁止**直接使用 HILOG 宏，采用 SOC_PERF_LOGD/LOGI/LOGW/LOGE/LOGF 宏

## 配置解析

- **禁止**资源 ID 超出有效范围，采用 `IsValidRangeResId` 校验（WRITE_NODE: 1000-5999，REPORT_TO_PERFSO: 10000-10999）

- **禁止**pair 指向未定义的资源，采用 `CheckPairResIdValid` 在加载完成后统一校验

- **禁止**default 值不在 available 列表中，采用 `CheckDefValid` 在加载完成后统一校验

- **禁止**GovResource 的 node 值路径数与 paths 数不匹配，采用 `LoadGovResourceAvailable` 逐条校验

## 节点写入

- **禁止**对 REPORT_TO_PERFSO 模式的资源执行 WriteNode，采用 `persistMode == REPORT_TO_PERFSO` 提前返回

- **禁止**使用未缓存且 realpath 校验失败的路径，采用 `GetFdForFilePath` 统一打开和缓存

- **禁止**手动 close fd，采用 fdsan_close_with_tag 在析构时统一释放

## 单例与命名

- **禁止**手动实现单例，采用 DECLARE_SINGLE_INSTANCE / IMPLEMENT_SINGLE_INSTANCE 宏（SocPerfConfig 使用 static 局部变量模式）
