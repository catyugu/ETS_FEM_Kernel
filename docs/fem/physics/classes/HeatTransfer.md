# HeatTransfer 类

## 描述

`HeatTransfer` 类表示热传导物理问题，负责管理热传导分析的内核并执行全局矩阵组装。该类使用模板设计，支持不同维度的热传导问题。通过 [IKernel](../../kernels/classes/KernelWrappers.md) 抽象接口和 [KernelWrapper](../../kernels/classes/KernelWrappers.md) 包装器，可以支持不同节点数的单元类型。

与之前版本相比，该类现在实现了单元类型过滤机制，确保只有适当类型的单元参与域内组装，并支持多物理场功能。

## 类定义

```cpp
template<int TDim, typename TScalar = double>
class HeatTransfer : public PhysicsField<TDim, TScalar>
```

**模板参数:**
- `TDim` - 问题的空间维度
- `TScalar` - 标量类型，默认为 `double`，也可支持 `std::complex<double>` 等类型

## 成员函数

### HeatTransfer()

构造函数。初始化变量名称为 "Temperature"。

### const std::string& getVariableName() const

获取物理场变量名称。

**返回值:**
- 字符串 "Temperature" 的常量引用

### void defineVariables(DofManager& dof_manager) const

在自由度管理器中定义温度变量。

**参数:**
- `dof_manager` - 自由度管理器引用

### template<typename KernelType> void addKernel(std::unique_ptr<KernelType> kernel)

添加内核到热传导问题中。

**模板参数:**
- `KernelType` - 内核类型

**参数:**
- `kernel` - 内核对象的智能指针

### void assemble_volume(const Mesh& mesh, const DofManager& dof_manager, std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global)

组装全局刚度矩阵和载荷向量。

**参数:**
- `mesh` - 网格对象的常量引用
- `dof_manager` - 自由度管理器的常量引用
- `triplet_list` - 稀疏矩阵的Triplet列表（输出）
- `F_global` - 全局载荷向量的引用

与之前版本相比，该方法现在会过滤单元类型，只对适当类型的单元进行组装：
- 一维问题：只组装线单元
- 二维问题：只组装三角形和四边形单元
- 三维问题：只组装四面体和六面体单元

此外，该方法现在使用Triplet列表而不是直接操作稀疏矩阵以提高性能。

### bool shouldAssembleElement(const Element& element, int problem_dim) const

判断单元是否应该参与组装。

**参数:**
- `element` - 单元对象的常量引用
- `problem_dim` - 问题维度

**返回值:**
- 布尔值，指示单元是否应该参与组装

### std::string getName() const

获取物理场名称。

**返回值:**
- 字符串 "HeatTransfer"

## 实现细节

与之前版本相比，该类的主要变化包括：

1. 实现了单元类型过滤机制，确保只有适当类型的单元参与域内组装
2. 添加了 `shouldAssembleElement` 方法，用于判断单元是否应该参与组装
3. 移除了硬编码的节点数
4. 实现了 `getVariableName` 和 `defineVariables` 方法以支持多物理场
5. 使用Triplet列表而不是直接操作稀疏矩阵以提高性能

这些改进使得物理场可以处理混合网格，即同时包含不同类型和节点数的单元，并确保只有适当类型的单元参与计算。

## 示例用法

```cpp
#include "fem/physics/HeatTransfer.hpp"

// 创建热传导问题对象
auto heat_transfer = std::make_unique<FEM::HeatTransfer<2>>();

// 添加材料和内核
auto material = std::make_unique<FEM::Material>();
material->setProperty("thermal_conductivity", 1.0); // 导热系数 W/(m·K)

heat_transfer->addKernel(
    std::make_unique<FEM::HeatDiffusionKernel<2>>(std::move(material))
);

// 添加边界条件
heat_transfer->addBoundaryCondition(
    std::make_unique<FEM::DirichletBC<2>>("Temperature", "hot_side", 100.0)
);
heat_transfer->addBoundaryCondition(
    std::make_unique<FEM::DirichletBC<2>>("Temperature", "cold_side", 0.0)
);

// 创建问题并求解
auto geometry = FEM::Mesh::create_uniform_2d_mesh(1.0, 1.0, 10, 10);
auto problem = std::make_unique<FEM::Problem<2>>(std::move(geometry), std::move(heat_transfer));
problem->assemble();
problem->applyBCs();
problem->solve();
```

## 依赖关系

- [PhysicsField](PhysicsField.md) - 基类
- [Kernel](../../kernels/classes/Kernel.md) - 内核基类
- [IKernel](../../kernels/classes/KernelWrappers.md) - 内核接口
- [KernelWrapper](../../kernels/classes/KernelWrappers.md) - 内核包装器
- [Mesh](../../mesh/classes/Mesh.md) - 网格
- [DofManager](../../core/classes/DofManager.md) - 自由度管理器