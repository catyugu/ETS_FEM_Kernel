# FEM::BCs Namespace

边界条件（Boundary Conditions）命名空间包含有限元方法中各种边界条件的实现。

## 概述

该命名空间提供了一套完整的边界条件类体系，支持Dirichlet、Neumann和Cauchy边界条件。所有边界条件类都继承自抽象基类BoundaryCondition，实现了统一的接口。

## 类列表

- [BoundaryCondition](classes/BoundaryCondition.md) - 边界条件抽象基类
- [DirichletBC](classes/DirichletBC.md) - Dirichlet边界条件类
- [NeumannBC](classes/NeumannBC.md) - Neumann边界条件类
- [CauchyBC](classes/CauchyBC.md) - Cauchy边界条件类

## 设计原则

1. **抽象化**：所有边界条件类型都继承自统一的基类
2. **可扩展性**：易于添加新的边界条件类型
3. **物理场解耦**：边界条件与具体物理场解耦，通过物理场管理边界条件
4. **统一接口**：提供一致的API用于边界条件的定义和应用

## 使用示例

```cpp
// 创建Dirichlet边界条件
auto dirichlet_bc = std::make_unique<DirichletBC<3>>("boundary_name", 100.0);

// 添加到物理场
physics->addBoundaryCondition(std::move(dirichlet_bc));
```