# FEM 核心模块 (fem/core)

## 概述

核心模块包含有限元方法的核心组件，包括Problem类、DofManager类、边界条件处理、线性求解器接口等。这些组件协同工作，提供有限元问题的完整求解流程。

## 主要类

- [Problem](classes/Problem.md) - 有限元问题主控制器类，协调整个求解过程
- [DofManager](classes/DofManager.md) - 自由度管理器，处理节点、边、面和体自由度
- [BoundaryCondition](classes/BoundaryCondition.md) - 边界条件抽象基类
- [LinearSolver](classes/LinearSolver.md) - 线性求解器接口
- [FEValues](classes/FEValues.md) - 有限元值计算类
- [ReferenceElement](classes/ReferenceElement.md) - 参考单元类

## 性能优化

最近，我们对核心模块进行了重大性能优化。引入了 [ReferenceElement](classes/ReferenceElement.md) 类来缓存参考单元上的形函数值、导数以及积分点信息。Problem类的assemble方法现在使用Triplet列表而不是直接操作稀疏矩阵，以提高组装效率。这种优化避免了在组装过程中频繁访问和修改稀疏矩阵，从而显著提高了性能。

## 使用方法

核心模块通过Problem类协调整个求解过程：

```cpp
auto problem = std::make_unique<FEM::Problem<2>>(std::move(mesh), std::move(physics));
problem->assemble();
problem->solve();
```

## 注意事项

1. Problem类是有限元求解的主要接口
2. DofManager负责管理所有自由度
3. 为了提高性能，现在使用Triplet列表而不是直接操作稀疏矩阵
4. 支持多物理场耦合
5. 通过 [ReferenceElement](classes/ReferenceElement.md) 类利用缓存机制避免重复计算

## 依赖关系

Core 模块依赖于 mesh 模块和 physics 模块，同时也依赖于第三方库 Eigen。