# FEValues 类

## 描述

`FEValues` 类负责计算和存储有限元分析中的形状函数值、梯度以及相关的几何信息。它在每个积分点上提供必要的数据，用于计算单元矩阵和向量。

该类是有限元计算中的关键组件，它封装了从参考单元到实际单元的变换过程，并提供了访问形状函数值、梯度以及雅可比行列式等信息的接口。

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

## 成员函数

### void reinit(int q_index)

重新初始化当前积分点索引。

**参数:**
- `q_index` - 新的积分点索引

### size_t n_quad_points() const

获取积分点数量。

**返回值:**
- 积分点总数

### const Eigen::VectorXd& N() const

获取当前积分点上的形状函数值。

**返回值:**
- 当前积分点上所有节点的形状函数值向量

### const Eigen::MatrixXd& dN_dx() const

获取当前积分点上的形状函数梯度（相对于实际坐标）。

**返回值:**
- 当前积分点上所有节点的形状函数梯度矩阵

### double JxW() const

获取当前积分点的雅可比行列式与权重的乘积。

**返回值:**
- 当前积分点的雅可比行列式与积分权重的乘积

### const Eigen::MatrixXd& B() const

获取当前积分点的B矩阵（应变-位移矩阵或梯度算子）。

**返回值:**
- 当前积分点的B矩阵

## 示例用法

```cpp
// 假设已经有一个单元对象element和分析类型
FEM::FEValues fe_values(element, 2, FEM::AnalysisType::SCALAR_DIFFUSION);

// 遍历所有积分点
for (size_t q = 0; q < fe_values.n_quad_points(); ++q) {
    fe_values.reinit(q);
    
    // 获取当前积分点的形状函数值和梯度
    const auto& N = fe_values.N();
    const auto& dN_dx = fe_values.dN_dx();
    
    // 获取雅可比行列式与权重的乘积
    double JxW = fe_values.JxW();
    
    // 获取B矩阵
    const auto& B = fe_values.B();
    
    // 使用这些值进行有限元计算...
}
```

## 实现细节

`FEValues` 类在构造时计算所有积分点上的数据并缓存起来，包括：
- 雅可比矩阵及其行列式
- 形状函数在实际坐标下的梯度
- B矩阵（根据分析类型构建）

通过这种方式，避免了在每次积分点计算时重复计算这些昂贵的操作，提高了计算效率。