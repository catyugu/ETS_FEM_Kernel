# FEValues 类

## 描述

`FEValues` 类负责计算和存储有限元分析中的形状函数值、梯度以及相关的几何信息。它在每个积分点上提供必要的数据，用于计算单元矩阵和向量。

该类是有限元计算中的关键组件，它封装了从参考单元到实际单元的变换过程，并提供了访问形状函数值、梯度以及雅可比行列式等信息的接口。

与之前版本相比，该类现在使用 [ReferenceElement](ReferenceElement.md) 类来处理形函数和积分规则，通过缓存机制避免重复计算，提高了代码的性能。此外，B矩阵的构建已从该类中移出，由各个Kernel负责构建，使该类更加通用。

该类已整合了之前的 `FEFaceValues` 类功能，现在可以处理任何类型的单元（体单元或面单元）。同时引入了现代C++迭代器接口，使得使用更加安全和便捷。

## 类定义

```cpp
class FEValues
```

## 构造函数

### FEValues(const Element& elem, int order, AnalysisType analysis_type)

构造函数，初始化FEValues对象。

**参数:**
- `elem` - 要计算的单元引用
- `order` - 积分阶数
- `analysis_type` - 分析类型（如标量扩散问题）

**异常:**
- `std::runtime_error` - 当雅可比行列式非正时抛出

## 构造函数

### FEValues(const Element& elem, int order, AnalysisType analysis_type)

构造函数，初始化FEValues对象。

**参数:**
- `elem` - 要计算的单元引用
- `order` - 积分阶数
- `analysis_type` - 分析类型（如标量扩散问题）

**异常:**
- `std::runtime_error` - 当雅可比行列式非正时抛出

### FEValues(const Element& elem, AnalysisType analysis_type)

构造函数，使用推荐的积分阶数初始化FEValues对象。

**参数:**
- `elem` - 要计算的单元引用
- `analysis_type` - 分析类型（如标量扩散问题）

## 成员函数

### size_t n_quad_points() const

获取积分点数量。

**返回值:**
- 积分点总数

### Iterator begin() const

获取积分点迭代器的起始位置。

**返回值:**
- 指向第一个积分点的迭代器

### Iterator end() const

获取积分点迭代器的结束位置。

**返回值:**
- 指向最后一个积分点之后位置的迭代器

### double shape_value(size_t i, size_t q) const

获取指定节点在指定积分点上的形函数值。

**参数:**
- `i` - 节点索引
- `q` - 积分点索引

**返回值:**
- 指定节点在指定积分点上的形函数值


## QuadraturePoint 类

`FEValues` 类内部定义了 `QuadraturePoint` 类，用于表示单个积分点的信息。

### 成员函数

#### const Eigen::VectorXd& N() const

获取当前积分点上的形状函数值。

**返回值:**
- 当前积分点上所有节点的形状函数值向量

#### const Eigen::MatrixXd& dN_dx() const

获取当前积分点上的形状函数梯度（相对于实际坐标）。

**返回值:**
- 当前积分点上所有节点的形状函数梯度矩阵

#### double JxW() const

获取当前积分点的雅可比行列式与权重的乘积。

**返回值:**
- 当前积分点的雅可比行列式与积分权重的乘积

## 示例用法

```cpp
// 假设已经有一个单元对象element和分析类型
FEM::FEValues fe_values(element, 2, FEM::AnalysisType::SCALAR_DIFFUSION);

// 使用新的迭代器接口遍历所有积分点
for (const auto& q_point : fe_values) {
    // 获取当前积分点的形状函数值和梯度
    const auto& N = q_point.N();
    const auto& dN_dx = q_point.dN_dx();
    
    // 获取雅可比行列式与权重的乘积
    double JxW = q_point.JxW();
    
    // 在Kernel中构建B矩阵
    Eigen::MatrixXd B = dN_dx; // 对于标量问题，B矩阵就是dN_dx
    
    // 使用这些值进行有限元计算...
}

// 或者使用传统的索引方式访问特定积分点信息
for (size_t q = 0; q < fe_values.n_quad_points(); ++q) {
    // 获取指定节点在指定积分点上的形函数值
    double value = fe_values.shape_value(node_index, q);
    // ...
}
```

## 实现细节

`FEValues` 类在构造时计算所有积分点上的数据并缓存起来，包括：
- 雅可比矩阵及其行列式
- 形状函数在实际坐标下的梯度

与之前的实现相比，现在通过 [ReferenceElement](ReferenceElement.md) 类获取形函数和积分点信息。这种设计利用了缓存机制，避免了重复计算，提高了性能。此外，B矩阵的构建已从该类中移出，由各个物理Kernel负责构建。这样使得 `FEValues` 类更加通用，可以适用于不同类型的物理问题，而不仅仅是标量扩散问题。

现在 `FEValues` 类已整合了 `FEFaceValues` 的功能，可以处理任何类型的单元（体单元或面单元）。同时引入了现代C++迭代器接口，使得使用更加安全和便捷，避免了手动调用 `reinit()` 函数可能带来的错误。

## 依赖关系

- [Element](../../mesh/classes/Element.md) - 单元类
- [ReferenceElement](ReferenceElement.md) - 参考单元类
- [AnalysisType](AnalysisTypes.md) - 分析类型枚举
- Eigen - 矩阵运算库