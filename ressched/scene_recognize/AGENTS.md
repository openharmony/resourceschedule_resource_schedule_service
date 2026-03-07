# 资源调度服务 - 场景识别模块

本文件为 Claude Code 提供 RSS 场景识别模块的开发指导。

---

## 快速参考

### 编译命令

```bash
# 场景识别模块作为 RSS 服务的一部分编译
hb build ressched/services:resschedsvc

# 编译静态库版本（用于测试依赖）
hb build ressched/services:resschedsvc_static
```

### 源文件

```bash
# 场景识别源文件位置
ressched/scene_recognize/src/
├── scene_recognizer_base.cpp
├── scene_recognizer_mgr.cpp
├── slide_recognizer.cpp
├── continuous_app_install_recognizer.cpp
├── system_upgrade_scene_recognizer.cpp
└── background_sensitive_task_overlapping_scene_recognizer.cpp
```

---

## 概述

场景识别模块负责识别系统和应用的各种运行场景，为资源调度决策提供上下文信息。通过分析系统事件和用户行为模式，识别出特定的应用场景并触发相应的调度策略。

## 目录结构

```
scene_recognize/
├── include/          # 头文件
└── src/              # 源文件
```

## 核心组件

### 场景识别器基类

`scene_recognizer_base.h` - 所有场景识别器的抽象基类：

```cpp
class SceneRecognizerBase {
public:
    virtual ~SceneRecognizerBase() = default;
    virtual void OnEvent(const std::shared_ptr<ResData>& data) = 0;
    virtual void Init() = 0;
    virtual void Disable() = 0;
};
```

### 场景识别管理器

`scene_recognizer_mgr.h/cpp` - 统一管理所有场景识别器：

**职责**：
- 注册/注销场景识别器
- 分发事件到各识别器
- 管理识别器生命周期

## 场景识别器类型

| 识别器 | 文件 | 功能描述 |
|--------|------|----------|
| 滑动识别器 | `slide_recognizer.cpp` | 识别用户滑动手势场景 |
| 应用安装识别器 | `continuous_app_install_recognizer.cpp` | 识别连续应用安装场景 |
| 系统升级识别器 | `system_upgrade_scene_recognizer.cpp` | 识别系统升级场景 |
| 后台敏感任务识别器 | `background_sensitive_task_overlapping_scene_recognizer.cpp` | 识别后台敏感任务重叠场景 |

## 详细说明

### 1. 滑动识别器 (SlideRecognizer)

**功能**：识别用户滑动手势

**识别逻辑**：
- 监听触摸事件
- 分析滑动速度和方向
- 识别连续滑动模式

**输出**：
- `RES_TYPE_SLIDE_RECOGNIZE` 事件
- 滑动速度、方向等参数

### 2. 连续应用安装识别器 (ContinuousAppInstallRecognizer)

**功能**：识别批量应用安装场景

**识别逻辑**：
- 监听应用安装事件
- 统计单位时间内安装数量
- 判断是否达到连续安装阈值

**触发条件**：
- 短时间内多次应用安装
- 批量更新场景

### 3. 系统升级识别器 (SystemUpgradeSceneRecognizer)

**功能**：识别系统升级场景

**识别逻辑**：
- 监听系统升级事件
- 识别升级阶段（下载、安装、重启）

**调度策略**：
- 升级期间降低后台任务优先级
- 预留足够系统资源

### 4. 后台敏感任务重叠识别器 (BackgroundSensitiveTaskOverlappingSceneRecognizer)

**功能**：识别多个后台敏感任务同时运行

**识别逻辑**：
- 监控后台任务状态
- 检测敏感任务重叠

**处理策略**：
- 调整任务优先级
- 资源分配优化

## 事件处理流程

```
系统事件 → SceneRecognizerMgr → 各识别器 OnEvent() → 场景识别 → 触发调度策略
```

## 使用示例

### 添加新的场景识别器

1. **继承基类**

```cpp
class MySceneRecognizer : public SceneRecognizerBase {
public:
    void Init() override;
    void Disable() override;
    void OnEvent(const std::shared_ptr<ResData>& data) override;
private:
    void HandleAppStateChange(const std::shared_ptr<ResData>& data);
    bool isActive_ = false;
};
```

2. **实现识别逻辑**

```cpp
void MySceneRecognizer::OnEvent(const std::shared_ptr<ResData>& data) {
    switch (data->resType) {
        case ResType::RES_TYPE_APP_STATE_CHANGE:
            HandleAppStateChange(data);
            break;
        // 处理其他事件
    }
}
```

3. **注册到管理器**

```cpp
void SceneRecognizerMgr::Init() {
    recognizers_.push_back(std::make_unique<MySceneRecognizer>());
    for (auto& recognizer : recognizers_) {
        recognizer->Init();
    }
}
```

## 配置参数

场景识别的配置参数通常定义在：
- `res_sched_config.xml` - 阈值、时间窗口等
- 代码中硬编码的默认值

### 典型配置项

```xml
<scene_recognize>
    <slide>
        <velocity_threshold>1000</velocity_threshold>
        <time_window>100</time_window>
    </slide>
    <app_install>
        <count_threshold>3</count_threshold>
        <time_window>60000</time_window>
    </app_install>
</scene_recognize>
```

## 性能考虑

### 事件处理延迟

- 场景识别应快速完成
- 避免阻塞事件分发线程
- 复杂计算使用异步处理

### 内存占用

- 限制历史数据缓存大小
- 及时清理过期数据
- 使用 LRU 缓存策略

## 调试方法

### 日志输出

```cpp
RESSCHED_LOGI("[SceneRecognize] Slide detected, velocity: %{public}d", velocity);
```

### HiTrace 跟踪

```cpp
HitraceScoped trace("SceneRecognizerMgr::OnEvent");
```

## 测试

### 单元测试

测试用例应覆盖：
- 正常场景识别
- 边界条件
- 并发事件处理
- 错误恢复

### 测试示例

```cpp
TEST(SlideRecognizerTest, DetectSlide) {
    SlideRecognizer recognizer;
    recognizer.Init();
    
    // 模拟滑动事件
    auto data = std::make_shared<ResData>();
    data->resType = ResType::RES_TYPE_CLICK_RECOGNIZE;
    data->payload = {{"velocity", 1200}};
    
    recognizer.OnEvent(data);
    
    // 验证识别结果
    EXPECT_TRUE(recognizer.IsSliding());
}
```

## 编译方法

### BUILD.gn 配置

场景识别模块是 RSS 核心服务的一部分，编译配置在 `ressched/services/BUILD.gn` 中。

### 编译命令

```bash
# 编译整个资源调度服务（包含场景识别模块）
hb build ressched/services:resschedsvc

# 编译静态库版本
hb build ressched/services:resschedsvc_static
```

### 作为依赖编译

其他模块依赖场景识别模块时，在 BUILD.gn 中：

```gn
deps = [
  "${ressched_services}:resschedsvc",
]

include_dirs = [
  "${ressched_sched_controller}/../scene_recognize/include",
]
```

### 源文件列表

| 源文件 | 说明 |
|--------|------|
| `scene_recognizer_base.cpp` | 场景识别器基类 |
| `scene_recognizer_mgr.cpp` | 场景识别管理器 |
| `slide_recognizer.cpp` | 滑动识别器 |
| `continuous_app_install_recognizer.cpp` | 应用安装识别器 |
| `system_upgrade_scene_recognizer.cpp` | 系统升级识别器 |
| `background_sensitive_task_overlapping_scene_recognizer.cpp` | 后台任务识别器 |

### 编译依赖

```gn
external_deps = [
  "c_utils:utils",
  "hilog:libhilog",
  "hitrace:libhitracechain",
  "json:nlohmann_json_static",
]
```

### 头文件引用

```cpp
#include "scene_recognizer_base.h"
#include "scene_recognizer_mgr.h"
#include "slide_recognizer.h"
```

## 集成注意事项

1. **事件订阅**：在 `Init()` 中订阅需要的事件
2. **资源释放**：在 `Disable()` 中清理资源
3. **线程安全**：识别器可能被多线程调用
4. **状态管理**：维护正确的内部状态机
