# DirichletBC

Dirichlet边界条件类，用于指定边界上的固定值。

## 类签名

```cpp
template<int TDim>
class DirichletBC : public BoundaryCondition<TDim>
```

## 概述

`DirichletBC`类实现Dirichlet边界条件，也称为本质边界条件。它用于指定边界上解的固定值，如固定温度、固定电势等。

## 构造函数

```cpp
DirichletBC(const std::string& boundary_name, double value)
```

### 参数

- `boundary_name` - 边界名称，用于标识网格中的特定边界
- `value` - 边界上的固定值

## 方法

### apply

应用Dirichlet边界条件（空实现）。

```cpp
void apply(const Mesh& mesh, const DofManager& dof_manager,
           Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) const override
```

Dirichlet边界条件由Problem类统一处理，因此此方法为空实现。

### getType

获取边界条件类型。

```cpp
BCType getType() const override
```

返回`BCType::Dirichlet`。

### getValue

获取边界条件值。

```cpp
double getValue() const
```

## 示例用法

```cpp
#include "fem/bcs/DirichletBC.hpp"

// 创建固定温度边界条件
auto temperature_bc = std::make_unique<DirichletBC<3>>("inlet_boundary", 300.0); // 300K

// 创建固定电势边界条件
auto potential_bc = std::make_unique<DirichletBC<2>>("electrode", 10.0); // 10V

// 添加到物理场
heat_physics->addBoundaryCondition(std::move(temperature_bc));
electrostatics_physics->addBoundaryCondition(std::move(potential_bc));
```

## 注意事项

- Dirichlet边界条件的处理方式特殊，由Problem类统一处理
- 边界名称必须与网格中的边界标识符匹配
- 值的单位取决于具体物理场类型