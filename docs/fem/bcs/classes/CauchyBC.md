# CauchyBC

Cauchy边界条件类，用于实现Robin型边界条件，如对流换热边界条件。

## 类签名

```cpp
template<int TDim, typename TScalar = double>
class CauchyBC : public BoundaryCondition<TDim, TScalar>
```

**模板参数:**
- `TDim` - 问题的空间维度
- `TScalar` - 标量类型，默认为double，也可支持std::complex<double>等类型

## 概述

`CauchyBC`类实现Cauchy边界条件，也称为Robin边界条件。它结合了Dirichlet和Neumann边界条件的特点，常用于对流换热等物理现象的建模。

## 构造函数

```cpp
CauchyBC(const std::string& variable_name, const std::string& boundary_name, TScalar h_val, TScalar T_inf_val)
```

### 参数

- `variable_name` - 变量名称，用于标识此边界条件应用到哪个物理场变量
- `boundary_name` - 边界名称，用于标识网格中的特定边界
- `h_val` - 换热系数或传导系数
- `T_inf_val` - 环境值（如环境温度）

## 方法

### apply

应用Cauchy边界条件。

```cpp
void apply(const Geometry& geometry, const DofManager& dof_manager,
           std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const override;
```

### getH

获取换热系数。

```cpp
TScalar getH() const { return h_; }
```

### getTInf

获取环境值。

```cpp
TScalar getTInf() const { return T_inf_; }
```

### getVariableName

获取变量名称。

```cpp
const std::string& getVariableName() const { return variable_name_; }
```

### getType

获取边界条件类型。

```cpp
BCType getType() const override
```

返回`BCType::Cauchy`。

## 示例用法

```cpp
#include "fem/bcs/CauchyBC.hpp"

// 创建对流换热边界条件
auto convection_bc = std::make_unique<FEM::CauchyBC<3, double>>("Temperature", "cooling_surface", 25.0, 293.15); 
// 换热系数 25 W/(m²·K)，环境温度 293.15 K

// 将边界条件添加到问题中
problem.addBoundaryCondition(std::move(convection_bc));
```

## 实现细节

在 `apply` 方法中，该类执行以下步骤：

1. 获取指定边界上的所有面单元
2. 对每个面单元创建 [FEValues](../../core/classes/FEValues.md) 对象
3. 使用范围for循环遍历所有积分点：
   - 获取形函数值和雅可比信息
   - 计算积分贡献
4. 将计算结果组装到全局矩阵和右端向量中

与之前的版本相比，`CauchyBC` 现在需要一个额外的 `variable_name` 参数。这是因为系统现在支持多物理场，每个边界条件必须明确指定它应用到哪个变量上。

## 注意事项

- Cauchy边界条件同时修改全局刚度矩阵和载荷向量
- 边界名称必须与网格中的边界标识符匹配
- 参数值的单位取决于具体物理场类型
- 该实现使用FEFaceValues进行边界积分计算
- 使用[FEValues](../../core/classes/FEValues.md)类进行边界积分计算，采用现代C++迭代器接口
- apply方法接受Geometry对象而不是Mesh对象
- 使用Triplet列表而不是直接操作稀疏矩阵来提高性能
- 变量名称必须与物理场中定义的变量名称匹配