# Profiler 类

## 描述

`Profiler` 类是一个性能分析工具，用于测量代码执行时间。它可以帮助开发者识别性能瓶颈并优化代码。该类使用单例模式，确保在整个应用程序中只有一个Profiler实例。

## 类签名

```cpp
class Profiler {
public:
    static Profiler& instance();
    
    void start(const std::string& tag);
    void stop(const std::string& tag);
    void reset();
    void print_results() const;
    
private:
    Profiler() = default;
    ~Profiler() = default;
    Profiler(const Profiler&) = delete;
    Profiler& operator=(const Profiler&) = delete;
};
```

## 静态方法

### instance

```cpp
static Profiler& instance();
```

**描述**: 获取Profiler单例实例。

**返回值**: Profiler类的引用

## 成员方法

### start

```cpp
void start(const std::string& tag);
```

**描述**: 开始计时一个特定的任务或代码段。

**参数**:
- `tag` - 任务标识符

### stop

```cpp
void stop(const std::string& tag);
```

**描述**: 停止计时一个特定的任务或代码段。

**参数**:
- `tag` - 任务标识符

### reset

```cpp
void reset();
```

**描述**: 重置所有计时数据。

### print_results

```cpp
void print_results() const;
```

**描述**: 打印所有任务的性能分析结果。

## 示例用法

```cpp
#include "utils/Profiler.hpp"

// 开始计时
Utils::Profiler::instance().start("matrix_assembly");

// 执行需要计时的代码
assemble_matrix();

// 停止计时
Utils::Profiler::instance().stop("matrix_assembly");

// 打印结果
Utils::Profiler::instance().print_results();
```

## 注意事项

1. 该类使用单例模式，通过`instance()`方法获取实例
2. 可以同时计时多个不同的任务，使用不同的tag标识
3. 计时结果包括执行次数、总时间、平均时间等信息
4. 在多线程环境中使用时需要注意同步问题