# fem::physics 命名空间

fem::physics 命名空间定义了物理问题的接口和实现，目前主要包含热传导问题的实现。

## 类列表

- [HeatTransfer](classes/HeatTransfer.md) - 热传导问题类

## 概述

fem::physics 命名空间提供了物理问题的抽象和具体实现。每个物理问题类都定义了该问题的控制方程、边界条件和初始条件的处理方法。

## 依赖关系

- fem::core - 核心计算功能
- fem::kernels - 计算内核
- fem::materials - 材料属性
- fem::mesh - 网格结构