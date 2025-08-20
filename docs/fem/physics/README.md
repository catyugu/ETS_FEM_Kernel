# FEM::Physics Namespace

物理场命名空间，包含各种物理问题的实现。

## 概述

Physics命名空间提供了各种物理场问题的抽象和具体实现，支持热传导、静电场等物理现象的数值模拟。

## 类列表

### 基础类
- [PhysicsField](classes/PhysicsField.md) - 物理场抽象基类

### 具体物理场实现
- [HeatTransfer](classes/HeatTransfer.md) - 热传导物理场
- [Electrostatics](classes/Electrostatics.md) - 静电场物理场

## 设计原则

1. **面向接口编程**：通过PhysicsField基类提供统一接口
2. **可扩展性**：易于添加新的物理场类型
3. **边界条件集成**：物理场类管理其相关的边界条件
4. **多物理场支持**：支持多个物理场的耦合求解

## 边界条件管理

物理场类通过以下方法管理边界条件：

```cpp
// 添加边界条件
physics->addBoundaryCondition(std::make_unique<DirichletBC<3>>("boundary_name", value));

// 获取边界条件
const auto& boundary_conditions = physics->getBoundaryConditions();

// 应用自然边界条件（Neumann和Cauchy）
physics->applyNaturalBCs(mesh, dof_manager, K_global, F_global);
```

## 使用示例

```cpp
// 创建热传导物理场
auto heat_physics = std::make_unique<HeatTransfer<3>>();

// 添加计算核
heat_physics->addKernel(std::make_unique<HeatDiffusionKernel<3, 8>>(material));

// 添加边界条件
heat_physics->addBoundaryCondition(
    std::make_unique<DirichletBC<3>>("inlet", 300.0)
);
heat_physics->addBoundaryCondition(
    std::make_unique<NeumannBC<3>>("outlet", 0.0)
);

// 创建问题并求解
Problem<3> problem(std::move(mesh), std::move(heat_physics));
problem.assemble();
problem.solve();
```