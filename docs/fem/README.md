# fem 命名空间

fem 是有限元方法（Finite Element Method）内核的主要命名空间，包含了实现有限元分析所需的核心组件。

## 子命名空间

- [fem::core](core/README.md) - 核心计算类，包括自由度管理、有限元值计算、线性求解器等
- [fem::io](io/README.md) - 输入输出模块，负责模型导入和结果导出
- [fem::kernels](kernels/README.md) - 物理内核，实现不同物理问题的计算逻辑
- [fem::materials](materials/README.md) - 材料属性定义和管理
- [fem::mesh](mesh/README.md) - 网格结构定义，包括节点、单元等
- [fem::physics](physics/README.md) - 物理问题接口定义

## 概述

fem 命名空间提供了有限元方法的核心功能，包括网格管理、材料建模、物理问题定义、有限元计算和输入输出等功能。该命名空间下的各个子命名空间协同工作，提供一个模块化、可扩展的有限元求解器核心。

## 依赖关系

- Eigen 3.4.0 - 用于高效的矩阵运算
- utils - 工具类，如形函数、积分规则等