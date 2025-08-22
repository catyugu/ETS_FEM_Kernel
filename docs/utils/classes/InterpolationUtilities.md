# InterpolationUtilities 类

## 描述

`InterpolationUtilities` 类提供在有限元网格中进行场变量插值的工具函数。在有限元分析中，解通常只在节点上已知，而需要在单元内部或积分点上获取场变量值时，需要通过形函数进行插值。该类目前提供在积分点上插值场变量的功能。

## 类定义

```cpp
class InterpolationUtilities
```

## 成员函数

### static std::map<std::string, double> interpolateAtQuadraturePoint(const Element& element, const Eigen::VectorXd& shape_values, const Problem<1>& problem)

在积分点上插值场变量的值。

**参数:**
- `element` - 当前单元的常量引用
- `shape_values` - 当前积分点上的形函数值向量(N)
- `problem` - `Problem`实例，用于访问所有物理场和解（目前简化为1D情况）

**返回值:**
- 一个从变量名到插值结果的映射

## 示例用法

```cpp
// 假设已有单元、形函数值和问题对象
const FEM::Element& element = ...;
const Eigen::VectorXd& shape_values = ...;
const FEM::Problem<1>& problem = ...;

// 在积分点上插值场变量
auto interpolated_values = FEM::Utils::InterpolationUtilities::interpolateAtQuadraturePoint(
    element, shape_values, problem);

// 获取插值的温度值
double temperature = interpolated_values["Temperature"];
```

## 实现细节

该函数通过形函数对节点上的解进行插值，计算公式为：

interpolated_value = Σ(N_i * u_i)

其中：
- N_i 是第i个节点的形函数值
- u_i 是第i个节点上的解值

目前实现仅支持温度场的插值，并且简化为1D情况。在完整的多场框架中，会支持更多类型的场变量插值。

## 依赖关系

- [Element](../../fem/mesh/classes/Element.md) - 单元类
- [Problem](../../fem/core/classes/Problem.md) - 问题类
- Eigen - 向量运算库
- STL - 映射、字符串、向量等标准库组件