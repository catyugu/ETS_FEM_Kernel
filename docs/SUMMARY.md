# 文档摘要

## 项目概述

ETS_FEM_Kernel 是一个有限元方法（FEM）计算内核的 C++ 实现，用于求解工程和物理问题，如热传导和静电场问题。项目采用模块化设计，支持多物理场耦合和可扩展的架构。

## 主要模块

### 核心模块 (fem/core)

核心模块包含有限元方法的核心组件，包括Problem类、DofManager类、边界条件处理、线性求解器接口等。这些组件协同工作，提供有限元问题的完整求解流程。

- [Problem](fem/core/classes/Problem.md) - 有限元问题主控制器类，协调整个求解过程
- [DofManager](fem/core/classes/DofManager.md) - 自由度管理器，处理节点、边、面和体自由度
- [BoundaryCondition](fem/core/classes/BoundaryCondition.md) - 边界条件抽象基类
- [LinearSolver](fem/core/classes/LinearSolver.md) - 线性求解器接口
- [FEValues](fem/core/classes/FEValues.md) - 有限元值计算类
- [ReferenceElement](fem/core/classes/ReferenceElement.md) - 参考单元类

**性能优化**: 
1. Problem类的assemble方法现在使用Triplet列表而不是直接操作稀疏矩阵，以提高组装效率。
2. 引入了ReferenceElement类来缓存参考单元上的形函数值、导数以及积分点信息，避免重复计算。

### 网格模块 (fem/mesh)

网格模块负责处理有限元网格，包括节点、单元的定义与操作。

- [Node](fem/mesh/classes/Node.md) - 网格节点类
- [Element](fem/mesh/classes/Element.md) - 网格单元类
- [Mesh](fem/mesh/classes/Mesh.md) - 网格类，管理节点和单元集合

### 物理场模块 (fem/physics)

物理场模块定义了各种物理问题的抽象接口和具体实现。

- [PhysicsField](fem/physics/classes/PhysicsField.md) - 物理场抽象基类
- HeatTransfer - 热传导物理场
- Electrostatics - 静电场物理场

**性能优化**: PhysicsField类的assemble_volume和applyNaturalBCs方法现在使用Triplet列表而不是直接操作稀疏矩阵，以提高组装效率。

### 边界条件模块 (fem/bcs)

边界条件模块提供了各种类型的边界条件实现。

- [DirichletBC](fem/bcs/classes/DirichletBC.md) - Dirichlet边界条件（本质边界条件）
- [NeumannBC](fem/bcs/classes/NeumannBC.md) - Neumann边界条件（自然边界条件）
- [CauchyBC](fem/bcs/classes/CauchyBC.md) - Cauchy边界条件（Robin边界条件）

**性能优化**: 所有边界条件的apply方法现在使用Triplet列表而不是直接操作稀疏矩阵，以提高组装效率。

### 材料模块 (fem/materials)

材料模块负责定义和管理材料属性。

- [Material](fem/materials/classes/Material.md) - 材料基类
- [MaterialProperty](fem/materials/classes/MaterialProperty.md) - 材料属性类

### 内核模块 (fem/kernels)

内核模块包含具体的物理计算内核。

- [Kernel](fem/kernels/classes/Kernel.md) - 内核抽象基类
- HeatDiffusionKernel - 热扩散内核
- ElectrostaticsKernel - 静电内核

### 输入输出模块 (fem/io)

输入输出模块负责网格和结果的导入导出。

- [Importer](fem/io/classes/Importer.md) - 数据导入器
- [Exporter](fem/io/classes/Exporter.md) - 数据导出器

### 工具模块 (utils)

工具模块包含辅助功能，如形函数、积分规则、日志等。

- [ShapeFunctions](utils/classes/ShapeFunctions.md) - 形函数类
- [Quadrature](utils/classes/Quadrature.md) - 积分规则类
- [SimpleLogger](utils/classes/SimpleLogger.md) - 简单日志类

## 性能优化

最近我们对整个代码库进行了重要的性能优化。主要改进包括：

1. 使用Triplet列表而不是直接操作稀疏矩阵来提高组装效率
2. 避免在组装过程中频繁访问和修改稀疏矩阵
3. 引入ReferenceElement类来缓存参考单元上的形函数值、导数以及积分点信息，避免重复计算
4. 这些优化显著提高了大规模问题的求解性能

## 多物理场支持

项目支持多物理场耦合，可以通过Problem类同时处理多个物理场问题。

## 技术栈

- C++17
- Eigen 3.4.0 (内嵌)
- GoogleTest v1.14.0 (测试框架)
- CMake 3.16+ (构建系统)