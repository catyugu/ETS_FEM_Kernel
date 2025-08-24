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

`NeumannBC` 类实现 Neumann 边界条件，用于指定边界上的法向通量（如热流密度、电通量密度等）。该类通过在边界面上进行数值积分来计算边界条件对系统矩阵和右端向量的贡献。

该类使用 [FEValues](../../core/classes/FEValues.md) 类来计算面单元上的形函数值和几何信息，并通过现代C++迭代器接口遍历积分点。

## 构造函数

```cpp
NeumannBC(const std::string& variable_name, const std::string& boundary_name, TScalar value)
```

### 参数

- `variable_name` - 变量名称，用于标识此边界条件应用到哪个物理场变量
- `boundary_name` - 边界名称，用于标识网格中的特定边界
- `value` - 边界上的通量值

## 方法

### apply

应用Neumann边界条件到系统矩阵和右端向量。

```cpp
void apply(const Geometry& geometry, const DofManager& dof_manager,
           std::vector<Eigen::Triplet<TScalar>>& triplet_list, 
           Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const override
```

**参数:**
- `geometry` - 几何对象，包含网格和边界定义
- `dof_manager` - 自由度管理器
- `triplet_list` - 稀疏矩阵的Triplet列表
- `F_global` - 全局右端向量

### getValue

获取边界条件的值。

```cpp
TScalar getValue() const { return value_; }
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

返回`BCType::Neumann`。

## 示例用法

```cpp
#include "fem/bcs/NeumannBC.hpp"

// 创建热流边界条件
auto heat_flux_bc = std::make_unique<NeumannBC<3>>("Temperature", "heated_surface", 1000.0); // 1000 W/m²

// 创建电场通量边界条件
auto electric_flux_bc = std::make_unique<NeumannBC<2>>("Voltage", "insulated_boundary", 0.0); // 0 C/m²

// 添加到物理场
heat_physics->addBoundaryCondition(std::move(heat_flux_bc));
electrostatics_physics->addBoundaryCondition(std::move(electric_flux_bc));
```

## 实现细节

在 `apply` 方法中，该类执行以下步骤：

1. 获取指定边界上的所有面单元
2. 对每个面单元创建 [FEValues](../../core/classes/FEValues.md) 对象
3. 使用范围for循环遍历所有积分点：
   - 获取形函数值和雅可比信息
   - 计算积分贡献
4. 将计算结果组装到全局右端向量中

与之前的版本相比，`NeumannBC` 现在需要一个额外的 `variable_name` 参数。这是因为系统现在支持多物理场，每个边界条件必须明确指定它应用到哪个变量上。

## 注意事项

- Neumann边界条件是对边界条件的自然弱形式实现，直接贡献到右端向量中
- 该类使用 [FEValues](../../core/classes/FEValues.md) 的现代迭代器接口，避免了手动管理积分点状态
- 边界条件的值应根据具体物理问题确定单位和符号
- 变量名称必须与物理场中定义的变量名称匹配