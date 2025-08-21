# fem::kernels 命名空间

## 概述

`fem::kernels` 命名空间包含有限元方法的核心计算内核。这些内核负责实现特定物理问题的单元矩阵计算，是有限元计算的核心组件。

与之前版本相比，该命名空间中的类现在支持动态节点数的单元，可以处理混合网格。

## 类列表

### [Kernel](classes/Kernel.md)

所有物理内核的抽象基类。与之前版本相比，该类现在使用动态大小的矩阵而不是固定大小的矩阵，这使得内核可以处理任意节点数的单元。

### [HeatDiffusionKernel](classes/HeatDiffusionKernel.md)

热传导问题的物理内核实现。该类负责计算热传导问题的单元刚度矩阵。与之前版本相比，该类现在支持动态节点数的单元。

### [HeatCapacityKernel](classes/HeatCapacityKernel.md)

热容问题的物理内核实现。该类负责计算热传导问题的单元质量矩阵。与之前版本相比，该类现在支持动态节点数的单元。

### [ElectrostaticsKernel](classes/ElectrostaticsKernel.md)

静电场问题的物理内核实现。该类负责计算静电场问题的单元刚度矩阵。与之前版本相比，该类现在支持动态节点数的单元。

### [KernelWrappers](classes/KernelWrappers.md)

内核包装器类，提供统一的接口来处理不同类型的内核。与之前版本相比，该类现在支持动态节点数的单元。

## 设计模式

该命名空间使用了以下设计模式：

1. **模板方法模式**: [Kernel](classes/Kernel.md) 基类定义了计算单元矩阵的接口，具体实现由派生类完成。
2. **策略模式**: 通过 [KernelWrappers](classes/KernelWrappers.md) 包装器，可以在运行时选择不同的内核实现。
3. **抽象工厂模式**: 通过 [IKernel](classes/KernelWrappers.md) 接口和 [KernelWrapper](classes/KernelWrappers.md) 实现，可以创建和管理不同类型的内核。

## 依赖关系

- [fem::mesh](../mesh/README.md) - 单元和网格数据结构
- [fem::core](../core/README.md) - 核心有限元计算组件
- [fem::materials](../materials/README.md) - 材料属性
- Eigen - 矩阵运算库

## 更新说明

与之前版本相比，该命名空间的主要变化包括：

1. 所有内核类移除了模板参数中的单元节点数
2. 使用动态大小矩阵替代固定大小矩阵
3. 支持运行时确定单元节点数
4. 矩阵大小和循环边界动态化
5. 可以处理混合网格，即同时包含不同类型和节点数的单元

这些改进使得内核更加通用和灵活，能够适应各种类型的有限元网格。