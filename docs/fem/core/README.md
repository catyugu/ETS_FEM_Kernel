# FEM Core 模块

## 概述

Core 模块包含了有限元方法的核心组件，如网格、单元、节点、自由度管理器、求解器等。这些组件构成了有限元分析的基础框架。

## 类和功能

### [AnalysisTypes](classes/AnalysisTypes.md)

定义了分析类型相关的枚举和常量。

### [BoundaryCondition](classes/BoundaryCondition.md)

定义了边界条件的基类和相关类型。

### [DofManager](classes/DofManager.md)

自由度管理器，负责管理有限元网格中各个几何实体（节点、边、面、体）上的自由度。它为每个自由度分配全局唯一的索引，并提供计算稀疏矩阵模式的功能。

该类支持多种自由度类型，不仅支持传统的节点自由度，还支持边、面和体自由度。此外，还支持基于变量的自由度管理，可以同时处理多个物理场的自由度，为多物理场耦合问题提供了支持。

### [FEFaceValues](classes/FEFaceValues.md)

有限元面值计算类，用于计算单元面上的形函数值、梯度等。

### [FEValues](classes/FEValues.md)

有限元值计算类，用于计算单元内的形函数值、梯度等。

### [FiniteElement](classes/FiniteElement.md)

有限元基类，定义了有限元方法的基本接口。

### [LinearSolver](classes/LinearSolver.md)

线性求解器，提供多种求解线性方程组的方法。

### [Problem](classes/Problem.md)

问题类，封装了有限元问题的求解流程。

### [ReferenceElement](classes/ReferenceElement.md)

参考单元类，定义了参考单元上的形函数、积分点等。

## 依赖关系

Core 模块依赖于 mesh 模块和 physics 模块，同时也依赖于第三方库 Eigen。