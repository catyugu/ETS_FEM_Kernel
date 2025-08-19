# 文档摘要

本文档提供了 ETS_FEM_Kernel 项目的完整文档索引，方便用户快速查找所需信息。

## 命名空间文档

### fem 命名空间
[fem 命名空间](fem/README.md) - 有限元方法内核的主要命名空间

子命名空间:
- [fem::core](fem/core/README.md) - 核心计算类
- [fem::io](fem/io/README.md) - 输入输出模块
- [fem::kernels](fem/kernels/README.md) - 物理内核
- [fem::materials](fem/materials/README.md) - 材料属性
- [fem::mesh](fem/mesh/README.md) - 网格结构
- [fem::physics](fem/physics/README.md) - 物理问题接口

### utils 命名空间
[utils 命名空间](utils/README.md) - 工具类

## 核心类文档

### fem::core 命名空间类

- [DofManager](fem/core/classes/DofManager.md) - 自由度管理器
- [FEValues](fem/core/classes/FEValues.md) - 有限元值计算器
- [LinearSolver](fem/core/classes/LinearSolver.md) - 线性求解器
- [Problem](fem/core/classes/Problem.md) - 问题定义基类
- [ReferenceElement](fem/core/classes/ReferenceElement.md) - 参考单元类

### fem::io 命名空间类

- [Exporter](fem/io/classes/Exporter.md) - 数据导出器
- [Importer](fem/io/classes/Importer.md) - 数据导入器

### fem::kernels 命名空间类

- [Kernel](fem/kernels/classes/Kernel.md) - 内核基类
- [HeatDiffusionKernel](fem/kernels/classes/HeatDiffusionKernel.md) - 热传导内核
- [ElectrostaticsKernel](fem/kernels/classes/ElectrostaticsKernel.md) - 静电场内核
- [KernelWrappers](fem/kernels/classes/KernelWrappers.md) - 内核包装器

### fem::materials 命名空间类

- [Material](fem/materials/classes/Material.md) - 材料类
- [MaterialProperty](fem/materials/classes/MaterialProperty.md) - 材料属性类

### fem::mesh 命名空间类

- [Element](fem/mesh/classes/Element.md) - 单元类
- [Mesh](fem/mesh/classes/Mesh.md) - 网格类
- [Node](fem/mesh/classes/Node.md) - 节点类

### fem::physics 命名空间类

- [HeatTransfer](fem/physics/classes/HeatTransfer.md) - 热传导问题类
- [Electrostatics](fem/physics/classes/Electrostatics.md) - 静电场问题类
- [PhysicsField](fem/physics/classes/PhysicsField.md) - 物理场抽象基类

### utils 命名空间类

- [InterpolationUtilities](utils/classes/InterpolationUtilities.md) - 插值工具类
- [Profiler](utils/classes/Profiler.md) - 性能分析器
- [Quadrature](utils/classes/Quadrature.md) - 积分规则类
- [ShapeFunctions](utils/classes/ShapeFunctions.md) - 形函数类
- [SimpleLogger](utils/classes/SimpleLogger.md) - 简单日志类

## 开发文档

- [项目概述](../README.md) - 项目简介和使用指南
- [项目规范](../PROMPT.md) - 开发规范和约定
- [开发计划](../PLAN.md) - 未来开发计划和优化方向