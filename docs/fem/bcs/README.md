# FEM 边界条件模块 (fem/bcs)

## 概述

边界条件模块提供了各种类型的边界条件实现，包括Dirichlet、Neumann和Cauchy边界条件。这些边界条件用于定义有限元问题在域边界上的行为。

## 类结构

- [BoundaryCondition](classes/BoundaryCondition.md) - 边界条件抽象基类
- [DirichletBC](classes/DirichletBC.md) - Dirichlet边界条件（本质边界条件）
- [NeumannBC](classes/NeumannBC.md) - Neumann边界条件（自然边界条件）
- [CauchyBC](classes/CauchyBC.md) - Cauchy边界条件（Robin边界条件）

## 性能优化

最近，我们对边界条件模块进行了性能优化。现在所有边界条件的`apply`方法都使用Triplet列表而不是直接操作稀疏矩阵，以提高组装效率。这种优化避免了在组装过程中频繁访问和修改稀疏矩阵，从而显著提高了性能。

## 使用方法

边界条件通过物理场类添加：

```cpp
auto physics = std::make_unique<FEM::HeatTransfer<2>>();
physics->addBoundaryCondition(
    std::make_unique<FEM::DirichletBC<2>>("left", 100.0)
);
```

## 注意事项

1. Dirichlet边界条件由Problem类统一处理，其apply方法为空实现
2. Neumann和Cauchy边界条件直接修改全局系统矩阵和向量
3. 所有边界条件都需要与网格中的边界标识符匹配
4. 为了提高性能，现在使用Triplet列表而不是直接操作稀疏矩阵