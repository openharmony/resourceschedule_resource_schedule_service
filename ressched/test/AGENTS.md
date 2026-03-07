# 资源调度服务 - 测试模块

本文件为 Claude Code 提供 RSS 测试模块的开发指导。

---

## 快速参考

### 编译命令

```bash
# 编译所有单元测试
hb build ressched/test/unittest:unittest

# 编译特定单元测试
hb build ressched/test/unittest:resschedservice_test
hb build ressched/test/unittest:suspendbaseclient_test
hb build ressched/test/unittest:resschedfwk_test

# 编译 Fuzz 测试
hb build ressched/test/fuzztest:fuzztest
hb build ressched/test/fuzztest/ressched_fuzzer:fuzztest
hb build ressched/test/fuzztest/loadfromconfigfile_fuzzer:fuzztest

# 编译测试工具
hb build ressched/test/testutil:ressched_test
```

### 运行测试

```bash
# 运行所有单元测试
./out/{product}/tests/ressched/unittest/resschedservice_test

# 运行特定测试套件
./resschedservice_test --gtest_filter="ResSchedServiceTest.*"

# 运行特定测试用例
./resschedservice_test --gtest_filter="PluginMgrTest.Init"

# 生成 XML 报告
./resschedservice_test --gtest_output=xml:test_results.xml

# 列出所有测试
./resschedservice_test --gtest_list_tests
```

### 测试输出位置

- **单元测试**: `out/{product}/tests/ressched/unittest/`
- **Fuzz 测试**: `out/{product}/tests/ressched/fuzztest/`
- **测试工具**: `out/{product}/resourceschedule/resource_schedule_service/`

---

## 概述

本目录包含资源调度服务（RSS）的测试代码，包括单元测试、Fuzz 测试和测试工具。

## 目录结构

```
test/
├── unittest/              # 单元测试（Google Test）
│   ├── src/               # 测试源文件
│   └── include/           # 测试头文件
├── fuzztest/              # Fuzz 测试
│   ├── ressched_fuzzer/           # RSS 主 fuzzer
│   ├── loadfromconfigfile_fuzzer/ # 配置文件加载 fuzzer
│   └── loadfromcustconfigfile_fuzzer/ # 自定义配置加载 fuzzer
├── testutil/              # 测试工具
└── resource/              # 测试资源文件
    └── resschedfwk/
        ├── parseconfig/   # 配置解析测试资源
        ├── parseswitch/   # 开关配置测试资源
        └── switchconfig/  # 开关测试资源
```

## 单元测试

### 主要测试文件

| 测试文件 | 测试目标 |
|----------|----------|
| `plugin_mgr_test.cpp` | 插件管理器 |
| `plugin_switch_test.cpp` | 插件开关 |
| `config_reader_test.cpp` | 配置读取器 |
| `event_listener_mgr_test.cpp` | 事件监听管理器 |
| `notifier_mgr_test.cpp` | 通知管理器 |
| `kill_event_listener_test.cpp` | 进程终止事件监听 |
| `batch_log_printer_test.cpp` | 批量日志打印 |
| `res_bundle_mgr_helper_test.cpp` | Bundle 管理助手 |
| `oobe_mgr_test.cpp` | OOBE（开箱即用体验）管理器 |
| `oobe_datashare_utils_test.cpp` | OOBE 数据共享工具 |

### 测试框架

- **Google Test (gtest)**: 主要测试框架
- **Google Mock (gmock)**: Mock 对象支持

### Mock 类命名规范

```cpp
class MockPluginMgr : public PluginMgr { ... };
class MockResSchedService : public ResSchedServiceStub { ... };
```

### 多线程测试

使用 `testing::mt::` 命名空间进行多线程测试。

## Fuzz 测试

### Fuzzer 类型

1. **ressched_fuzzer** - RSS 接口 fuzz 测试
   - 测试 RSS 服务的各种输入处理
   - 位于 `fuzztest/ressched_fuzzer/`

2. **loadfromconfigfile_fuzzer** - 配置文件解析 fuzz 测试
   - 测试配置文件的解析逻辑
   - 位于 `fuzztest/loadfromconfigfile_fuzzer/`

3. **loadfromcustconfigfile_fuzzer** - 自定义配置解析 fuzz 测试
   - 测试自定义配置的解析逻辑
   - 位于 `fuzztest/loadfromcustconfigfile_fuzzer/`

### Fuzz 语料库

每个 fuzzer 都有 `corpus/` 目录存放测试语料库。

## 测试工具

`testutil/` 目录包含测试工具类：
- 测试辅助函数
- Mock 对象定义
- 测试数据生成器

## 测试资源

`resource/` 目录包含测试所需的资源文件：
- XML 配置文件
- JSON 测试数据
- 开关配置文件

## 编译方法

### BUILD.gn 配置

测试模块使用 GN 构建系统，主要配置在 `unittest/BUILD.gn` 和 `fuzztest/BUILD.gn` 中。

### 单元测试编译目标

| 目标名称 | 说明 | 输出路径 |
|----------|------|----------|
| `resschedservice_test` | RSS 服务单元测试 | `resource_schedule_service/ressched` |
| `suspendbaseclient_test` | 暂停管理器客户端测试 | `resource_schedule_service/ressched` |
| `resschedservice_mock_test` | Mock 测试 | `resource_schedule_service/ressched` |
| `resschedfwk_test` | 调度框架测试 | `resource_schedule_service/ressched` |
| `suspend_manager_base_observer_test` | 暂停管理器观察者测试 | `resource_schedule_service/ressched` |

### Fuzz 测试编译目标

| 目标名称 | 说明 |
|----------|------|
| `ResSchedFuzzTest` | RSS 主 fuzzer |
| `LoadFromConfigFileFuzzTest` | 配置文件解析 fuzzer |
| `LoadFromCustConfigFileFuzzTest` | 自定义配置解析 fuzzer |

### 编译命令

```bash
# 编译所有单元测试
hb build ressched/test/unittest:unittest

# 编译特定单元测试
hb build ressched/test/unittest:resschedservice_test
hb build ressched/test/unittest:suspendbaseclient_test

# 编译所有 Fuzz 测试
hb build ressched/test/fuzztest:fuzztest

# 编译特定 Fuzz 测试
hb build ressched/test/fuzztest/ressched_fuzzer:fuzztest
hb build ressched/test/fuzztest/loadfromconfigfile_fuzzer:fuzztest

# 编译测试工具
hb build ressched/test/testutil:ressched_test
```

### 编译依赖

单元测试依赖的外部组件：
- `ability_runtime:ability_manager` - 应用运行时
- `access_token:libaccesstoken_sdk` - 访问令牌
- `c_utils:utils` - 通用工具
- `ffrt:libffrt` - 快速函数运行时
- `hilog:libhilog` - 日志
- `ipc:ipc_single` - IPC 通信
- `data_share:datashare_consumer` - 数据共享

### 编译选项

测试专用编译选项：
```gn
cflags = [
  "-Dprivate=public",    # 暴露私有成员用于测试
  "-Dprotected=public",  # 暴露保护成员用于测试
]
```

Fuzz 测试编译选项：
```gn
cflags = [
  "-g",                   # 调试信息
  "-O0",                  # 禁用优化
  "-fno-omit-frame-pointer",  # 保留帧指针
]
```

## 运行测试

### 构建测试

```bash
# 构建单元测试
hb build ressched/test/unittest:unittest

# 构建 fuzz 测试
hb build ressched_fuzzer
hb build loadfromconfigfile_fuzzer
```

### 执行测试

```bash
# 运行所有单元测试
./unittest

# 运行特定测试套件
./unittest --gtest_filter="ResSchedServiceTest.*"

# 运行特定测试用例
./unittest --gtest_filter="PluginMgrTest.Init"

# 生成 XML 报告
./unittest --gtest_output=xml:test_results.xml
```

## 测试规范

### 测试类命名

- 以 `Test` 结尾，如 `PluginMgrTest`
- 或使用被测类名加 `Test`，如 `ResSchedServiceTest`

### 测试用例命名

- 描述测试行为
- 使用 PascalCase
- 例如：`Init_Success`、`DispatchResource_InvalidData`

### 测试夹具

```cpp
class PluginMgrTest : public testing::Test {
protected:
    void SetUp() override;
    void TearDown() override;
    // 测试数据成员
};
```

### 断言使用

```cpp
EXPECT_EQ(expected, actual);      // 期望值比较
EXPECT_TRUE(condition);           // 布尔断言
ASSERT_NE(nullptr, ptr);          // 致命断言（失败终止）
```

## 覆盖率要求

- 核心业务逻辑：>= 80%
- 工具类函数：>= 60%
- 错误处理路径：>= 50%

## 持续集成

测试在每次代码提交时自动运行：
1. 编译检查
2. 单元测试执行
3. 代码覆盖率报告
4. Fuzz 回归测试
