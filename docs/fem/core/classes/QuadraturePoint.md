# QuadraturePoint 类

## 描述

`QuadraturePoint` 类表示有限元计算中的单个积分点，包含该点上的形函数值、梯度以及雅可比信息。该类是 [FEValues](FEValues.md) 类的内部组件，通过 [FEValues](FEValues.md) 的迭代器接口访问。

## 类定义

```cpp
class QuadraturePoint
```

## 成员函数

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

## 使用示例

```cpp
// 创建FEValues对象
FEM::FEValues fe_values(element, 2, FEM::AnalysisType::SCALAR_DIFFUSION);

// 使用范围for循环遍历所有积分点
for (const auto& q_point : fe_values) {
    // 获取当前积分点的形状函数值和梯度
    const auto& N = q_point.N();
    const auto& dN_dx = q_point.dN_dx();
    
    // 获取雅可比行列式与权重的乘积
    double JxW = q_point.JxW();
    
    // 使用这些值进行有限元计算...
}
```

## 实现细节

`QuadraturePoint` 是 [FEValues](FEValues.md) 类的友元类，可以直接访问 [FEValues](FEValues.md) 对象中的数据。该类采用惰性求值策略，只在调用相应函数时才从 [FEValues](FEValues.md) 对象中获取数据，避免了不必要的数据复制。

## 注意事项

- `QuadraturePoint` 对象只能通过 [FEValues](FEValues.md) 的迭代器接口获得
- 该对象不拥有数据，仅提供对 [FEValues](FEValues.md) 内部数据的引用访问
- 不应在 [FEValues](FEValues.md) 对象销毁后使用 `QuadraturePoint` 对象