# MaterialProperty 类

## 描述

`MaterialProperty` 类表示材料的单个物理属性，可以是常数值或依赖于其他变量的函数值。该类为材料属性的计算提供了统一接口，支持常数属性和复杂函数属性。

## 类定义

```cpp
class MaterialProperty
```

## 构造函数

### MaterialProperty(double constant_value)

构造一个常数属性。

**参数:**
- `constant_value` - 常数值

### MaterialProperty(std::function<double(const std::map<std::string, double>&)> func)

构造一个函数属性。

**参数:**
- `func` - 计算属性值的函数，接受一个变量映射并返回属性值

## 成员函数

### double evaluate(const std::map<std::string, double>& variables = {}) const

评估属性值。

**参数:**
- `variables` - 变量映射，用于函数属性计算，默认为空映射

**返回值:**
- 属性值

**异常:**
- `std::runtime_error` - 当属性未正确定义时抛出

## 示例用法

```cpp
// 创建常数属性
FEM::MaterialProperty constant_prop(401.0); // 铜的热导率 W/(m·K)
double k = constant_prop.evaluate(); // 返回 401.0

// 创建函数属性
FEM::MaterialProperty function_prop([](const std::map<std::string, double>& vars) {
    double T = vars.at("temperature");
    // 温度依赖的热导率模型
    return 401.0 * (1.0 - 0.0001 * (T - 300.0));
});

// 评估函数属性
std::map<std::string, double> vars = {{"temperature", 400.0}};
double k_T = function_prop.evaluate(vars); // 根据温度计算热导率
```

## 实现细节

`MaterialProperty` 类使用一个布尔标志`is_constant`来区分常数属性和函数属性。对于常数属性，直接返回存储的值；对于函数属性，调用存储的函数对象进行计算。

该设计允许材料属性在运行时根据环境条件（如温度、压力等）动态变化，提供了灵活性和扩展性。

## 依赖关系

- STL - 函数对象、映射、异常等标准库组件