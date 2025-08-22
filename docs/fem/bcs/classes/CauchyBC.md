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
CauchyBC(const std::string& boundary_name, TScalar h_val, TScalar T_inf_val)
```

### 参数

- `boundary_name` - 边界名称，用于标识网格中的特定边界
- `h_val` - 换热系数或传导系数
- `T_inf_val` - 环境值（如环境温度）

## 方法

### apply

应用Cauchy边界条件。

```cpp
void apply(const Mesh& mesh, const DofManager& dof_manager,
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
auto convection_bc = std::make_unique<CauchyBC<3>>("cooling_surface", 25.0, 293.15); 
// 换热系数 25 W/(m²·K)，环境温度 293.15 K

// 创建一般Robin边界条件
auto robin_bc = std::make_unique<CauchyBC<2>>("robin_boundary", 10.0, 5.0);

// 添加到物理场
heat_physics->addBoundaryCondition(std::move(convection_bc));
physics->addBoundaryCondition(std::move(robin_bc));
```

## 注意事项

- Cauchy边界条件同时修改全局刚度矩阵和载荷向量
- 边界名称必须与网格中的边界标识符匹配
- 参数值的单位取决于具体物理场类型
- 该实现使用FEFaceValues进行边界积分计算
- 为了提高性能，现在使用Triplet列表而不是直接操作稀疏矩阵