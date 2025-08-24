# DirichletBC

Dirichlet边界条件类，用于指定边界上的固定值。

## 类签名

```cpp
template<int TDim, typename TScalar = double>
class DirichletBC : public BoundaryCondition<TDim, TScalar>
```

**模板参数:**
- `TDim` - 问题的空间维度
- `TScalar` - 标量类型，默认为double，也可支持std::complex<double>等类型

## 概述

`DirichletBC`类实现Dirichlet边界条件，也称为本质边界条件。它用于指定边界上解的固定值，如固定温度、固定电势等。

## 构造函数

```cpp
DirichletBC(const std::string& variable_name, const std::string& boundary_name, TScalar value)
```

### 参数

- `variable_name` - 变量名称，用于标识此边界条件应用到哪个物理场变量
- `boundary_name` - 边界名称，用于标识网格中的特定边界
- `value` - 边界上的固定值

## 方法

### apply

应用Dirichlet边界条件（空实现）。

```cpp
void apply(const Geometry& geometry, const DofManager& dof_manager,
           std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const override
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
TScalar getValue() const
```

### getVariableName

获取变量名称。

```cpp
const std::string& getVariableName() const { return variable_name_; }
```

## 示例用法

```cpp
#include "fem/bcs/DirichletBC.hpp"

// 创建固定温度边界条件 (使用默认double类型)
auto temperature_bc = std::make_unique<DirichletBC<3>>("Temperature", "inlet_boundary", 300.0); // 300K

// 创建固定电势边界条件 (使用默认double类型)
auto potential_bc = std::make_unique<DirichletBC<2>>("Voltage", "electrode", 10.0); // 10V

// 创建复数边界条件 (用于频域分析)
auto complex_bc = std::make_unique<DirichletBC<2, std::complex<double>>>("Voltage", "port", std::complex<double>(1.0, 0.5));

// 添加到物理场
heat_physics->addBoundaryCondition(std::move(temperature_bc));
electrostatics_physics->addBoundaryCondition(std::move(potential_bc));
wave_physics->addBoundaryCondition(std::move(complex_bc));
```

## 实现细节

与之前的版本相比，`DirichletBC` 现在需要一个额外的 `variable_name` 参数。这是因为系统现在支持多物理场，每个边界条件必须明确指定它应用到哪个变量上。

## 注意事项

- Dirichlet边界条件的处理方式特殊，由Problem类统一处理
- 边界名称必须与网格中的边界标识符匹配
- 变量名称必须与物理场中定义的变量名称匹配
- 值的单位取决于具体物理场类型
- apply方法现在接受Geometry对象而不是Mesh对象