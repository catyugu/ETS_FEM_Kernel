# NeumannBC

Neumann边界条件类，用于指定边界上的法向通量或梯度。

## 类签名

```cpp
template<int TDim>
class NeumannBC : public BoundaryCondition<TDim>
```

## 概述

`NeumannBC`类实现Neumann边界条件，也称为自然边界条件。它用于指定边界上的法向通量或梯度，如热流密度、电场法向分量等。

## 构造函数

```cpp
NeumannBC(const std::string& boundary_name, double value)
```

### 参数

- `boundary_name` - 边界名称，用于标识网格中的特定边界
- `value` - 边界上的通量值

## 方法

### apply

应用Neumann边界条件。

```cpp
void apply(const Mesh& mesh, const DofManager& dof_manager,
           Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) const override
```

该方法在边界单元上进行积分，计算通量对载荷向量的贡献。

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