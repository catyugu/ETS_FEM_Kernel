# fem::core 命名空间

fem::core 命名空间包含了有限元计算的核心类和功能，负责管理自由度、计算有限元值、求解线性方程组等核心计算任务。

## 类列表

- [DofManager](classes/DofManager.md) - 自由度管理器
- [FEValues](classes/FEValues.md) - 有限元值计算器
- [LinearSolver](classes/LinearSolver.md) - 线性求解器
- [Problem](classes/Problem.md) - 问题定义基类
- [ReferenceElement](classes/ReferenceElement.md) - 参考单元类

## 概述

fem::core 命名空间提供了有限元方法的核心计算功能。这些类负责处理有限元分析中最基本的操作，包括自由度的管理、有限元值的计算、线性系统的构建和求解等。

最新更新增加了对稀疏模式预计算的支持，以提高矩阵组装的性能，以及对多种求解器类型的支持。

## 依赖关系

- Eigen 3.4.0 - 矩阵运算库
- fem::mesh - 网格数据结构
- utils - 工具函数，如形函数和积分点