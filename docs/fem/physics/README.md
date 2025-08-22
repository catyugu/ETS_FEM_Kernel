# FEM 物理场模块 (fem/physics)

## 概述

物理场模块定义了各种物理问题的抽象接口和具体实现。通过继承PhysicsField抽象类，可以实现各种具体的物理场问题，如热传导、静电场等。

## 主要类

- [PhysicsField](classes/PhysicsField.md) - 物理场抽象基类
- HeatTransfer - 热传导物理场
- Electrostatics - 静电场物理场

## 性能优化

最近，我们对物理场模块进行了性能优化。PhysicsField类的assemble_volume和applyNaturalBCs方法现在使用Triplet列表而不是直接操作稀疏矩阵，以提高组装效率。这种优化避免了在组装过程中频繁访问和修改稀疏矩阵，从而显著提高了性能。

## 使用方法

物理场模块通过Problem类使用：

```cpp
auto physics = std::make_unique<FEM::HeatTransfer<2>>();
physics->addKernel(std::make_unique<FEM::HeatDiffusionKernel<2>>(*material));
auto problem = std::make_unique<FEM::Problem<2>>(std::move(mesh), std::move(physics));
```

## 注意事项

1. 所有具体的物理场类都必须继承自PhysicsField并实现所有纯虚函数
2. 为了提高性能，现在使用Triplet列表而不是直接操作稀疏矩阵
3. 支持多物理场耦合
