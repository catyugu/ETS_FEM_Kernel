# SimpleLogger 类

## 描述

`SimpleLogger` 类是一个线程安全的简单日志记录器，采用单例模式设计。它支持不同级别的日志记录（信息、警告、错误），可以同时输出到控制台和文件。控制台输出带有颜色标识，便于区分不同级别的日志。

## 类定义

```cpp
class Logger
```

## 枚举类型

### LogLevel

日志级别枚举：

- `info` - 信息级别
- `warn` - 警告级别
- `error` - 错误级别

## 成员函数

### static Logger& instance()

获取日志记录器的单例实例。

**返回值:**
- 日志记录器实例的引用

### void set_logfile(const std::string& log_filename = "")

设置日志文件。

**参数:**
- `log_filename` - 日志文件名，默认为空字符串（不输出到文件）

### void set_loglevel(int log_level = 0)

设置日志级别阈值。

**参数:**
- `log_level` - 日志级别（0=info, 1=warn, 2=error），默认为0

### template<typename... Args> void info(Args... args)

记录信息级别日志。

**模板参数:**
- `Args` - 可变参数类型

**参数:**
- `args` - 要记录的消息参数

### template<typename... Args> void warn(Args... args)

记录警告级别日志。

**模板参数:**
- `Args` - 可变参数类型

**参数:**
- `args` - 要记录的消息参数

### template<typename... Args> void error(Args... args)

记录错误级别日志。

**模板参数:**
- `Args` - 可变参数类型

**参数:**
- `args` - 要记录的消息参数

## 示例用法

```cpp
// 获取日志记录器实例
auto& logger = Utils::Logger::instance();

// 设置日志文件
logger.set_logfile("application.log");

// 设置日志级别（只记录警告和错误）
logger.set_loglevel(1);

// 记录不同级别的日志
logger.info("Application started successfully");
logger.warn("This is a warning message");
logger.error("An error occurred: ", error_code);

// 在类中使用日志记录
class MyClass {
public:
    void doSomething() {
        Utils::Logger::instance().info("Doing something...");
        // ... 做一些工作 ...
        Utils::Logger::instance().info("Finished doing something");
    }
};
```

## 实现细节

`SimpleLogger` 采用单例模式设计，通过静态函数 `instance()` 获取唯一实例。使用 `std::mutex` 保证线程安全，支持同时从多个线程记录日志。

日志消息通过可变参数模板函数实现，可以接受任意数量和类型的参数。时间戳使用 `std::chrono` 获取，并以 "YYYY-MM-DD HH:MM:SS" 格式显示。

控制台输出使用 ANSI 颜色代码：
- 信息（绿色）
- 警告（黄色）
- 错误（红色）

在 Windows 系统上，通过启用虚拟终端处理来支持 ANSI 颜色代码。

## 依赖关系

- STL - 输入输出流、字符串、互斥量、时间处理等标准库组件
- Windows API - 仅在 Windows 平台上使用，用于启用虚拟终端处理