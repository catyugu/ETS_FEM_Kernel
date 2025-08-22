# NeumannBC

Neumann边界条件类，用于指定边界上的法向通量或梯度。

## 类签名

```cpp
template<int TDim, typename TScalar = double>
class NeumannBC : public BoundaryCondition<TDim, TScalar>
```

**模板参数:**
- `TDim` - 问题的空间维度
- `TScalar` - 标量类型，默认为double，也可支持std::complex<double>等类型

## 概述

`NeumannBC`类实现Neumann边界条件，也称为自然边界条件。它用于指定边界上的法向通量或梯度，如热流密度、电场法向分量等。

## 构造函数

```cpp
NeumannBC(const std::string& boundary_name, TScalar value)
```

### 参数

- `boundary_name` - 边界名称，用于标识网格中的特定边界
- `value` - 边界上的通量值

## 方法

### apply

应用Neumann边界条件。

```cpp
void apply(const Geometry& geometry, const DofManager& dof_manager,
           std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const override;
```

### getValue

获取边界条件的值。

```cpp
TScalar getValue() const { return value_; }
```

### getType

获取边界条件类型。

```cpp
BCType getType() const override
```

返回`BCType::Neumann`。

## 示例用法

```cpp
#include "fem/bcs/NeumannBC.hpp"

// 创建热流边界条件
auto heat_flux_bc = std::make_unique<NeumannBC<3>>("heated_surface", 1000.0); // 1000 W/m²

// 创建电场通量边界条件
auto electric_flux_bc = std::make_unique<NeumannBC<2>>("insulated_boundary", 0.0); // 0 C/m²

// 添加到物理场
heat_physics->addBoundaryCondition(std::move(heat_flux_bc));
electrostatics_physics->addBoundaryCondition(std::move(electric_flux_bc));
```

## 注意事项

- Neumann边界条件直接修改全局载荷向量
- 边界名称必须与网格中的边界标识符匹配
- 值的单位取决于具体物理场类型
- 该实现使用FEFaceValues进行边界积分计算
- 为了提高性能，现在使用Triplet列表而不是直接操作稀疏矩阵
- apply方法现在接受Geometry对象而不是Mesh对象