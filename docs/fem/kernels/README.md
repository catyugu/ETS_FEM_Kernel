# fem::kernels 命名空间

fem::kernels 命名空间包含了各种物理问题的计算内核，每个内核负责实现特定物理现象的有限元计算。

## 类列表

- [Kernel](classes/Kernel.md) - 内核基类
- [HeatDiffusionKernel](classes/HeatDiffusionKernel.md) - 热传导内核

## 概述

fem::kernels 命名空间定义了不同物理问题的计算内核。这些内核负责实现特定物理现象的有限元计算，例如热传导、结构力学等。每个内核都继承自基类 [Kernel](classes/Kernel.md)，并实现特定物理问题的计算逻辑。

## 依赖关系

- fem::core - 核心计算功能
- fem::mesh - 网格数据结构
- fem::materials - 材料属性
- utils - 工具函数