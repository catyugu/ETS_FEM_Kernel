# fem::kernels 命名空间

fem::kernels 命名空间包含了各种物理问题的计算内核，每个内核负责实现特定物理现象的有限元计算。

## 类列表

- [Kernel](classes/Kernel.md) - 内核基类
- [HeatDiffusionKernel](classes/HeatDiffusionKernel.md) - 热传导内核
- [ElectrostaticsKernel](classes/ElectrostaticsKernel.md) - 静电场内核
- [HeatCapacityKernel](classes/HeatCapacityKernel.md) - 热容内核（频域分析）
- [KernelWrappers](classes/KernelWrappers.md) - 内核包装器

## 概述

fem::kernels 命名空间定义了不同物理问题的计算内核。这些内核负责实现特定物理现象的有限元计算，例如热传导、静电场等。每个内核都继承自基类 [Kernel](classes/Kernel.md)，并实现特定物理问题的计算逻辑。

为了支持多物理场和不同类型单元的计算，我们引入了[KernelWrappers](classes/KernelWrappers.md)来统一管理不同类型的内核。

## 依赖关系

- [fem::core](../core) - 核心计算功能
- [fem::mesh](../mesh) - 网格数据结构
- [fem::materials](../materials) - 材料属性
- [utils](../../utils) - 工具函数