# 文档索引

## 命名空间

### [fem](fem/README.md)

#### 核心模块

- [core](fem/core/README.md)
  - 类:
    - [DofManager](fem/core/classes/DofManager.md)
    - [FEValues](fem/core/classes/FEValues.md)
    - [LinearSolver](fem/core/classes/LinearSolver.md)
    - [Problem](fem/core/classes/Problem.md)
    - [ReferenceElement](fem/core/classes/ReferenceElement.md)

- [kernels](fem/kernels/README.md)
  - 类:
    - [Kernel](fem/kernels/classes/Kernel.md)
    - [HeatDiffusionKernel](fem/kernels/classes/HeatDiffusionKernel.md)
    - [HeatCapacityKernel](fem/kernels/classes/HeatCapacityKernel.md)
    - [ElectrostaticsKernel](fem/kernels/classes/ElectrostaticsKernel.md)
    - [KernelWrappers](fem/kernels/classes/KernelWrappers.md)

- [materials](fem/materials/README.md)
  - 类:
    - [Material](fem/materials/classes/Material.md)
    - [MaterialProperty](fem/materials/classes/MaterialProperty.md)

- [mesh](fem/mesh/README.md)
  - 类:
    - [Element](fem/mesh/classes/Element.md)
    - [Mesh](fem/mesh/classes/Mesh.md)
    - [Node](fem/mesh/classes/Node.md)

- [physics](fem/physics/README.md)
  - 类:
    - [PhysicsField](fem/physics/classes/PhysicsField.md)
    - [HeatTransfer](fem/physics/classes/HeatTransfer.md)
    - [Electrostatics](fem/physics/classes/Electrostatics.md)

- [bcs](fem/bcs/README.md)
  - 类:
    - [BoundaryCondition](fem/bcs/classes/BoundaryCondition.md)
    - [DirichletBC](fem/bcs/classes/DirichletBC.md)
    - [NeumannBC](fem/bcs/classes/NeumannBC.md)
    - [CauchyBC](fem/bcs/classes/CauchyBC.md)

- [io](fem/io/README.md)
  - 类:
    - [Exporter](fem/io/classes/Exporter.md)
    - [Importer](fem/io/classes/Importer.md)

#### 工具模块

### [utils](utils/README.md)

- 类:
  - [Profiler](utils/classes/Profiler.md)
  - [Quadrature](utils/classes/Quadrature.md)
  - [ShapeFunctions](utils/classes/ShapeFunctions.md)
  - [SimpleLogger](utils/classes/SimpleLogger.md)

## 更新日志

### 重大更新 - 泛型设计重构

1. **Kernel 类重构**
   - 移除了模板参数中的 `TNumNodes_`
   - 使用动态大小矩阵替代固定大小矩阵
   - 支持任意节点数的单元

2. **具体 Kernel 实现更新**
   - [HeatDiffusionKernel](fem/kernels/classes/HeatDiffusionKernel.md)、[HeatCapacityKernel](fem/kernels/classes/HeatCapacityKernel.md) 和 [ElectrostaticsKernel](fem/kernels/classes/ElectrostaticsKernel.md) 移除了 `TNumNodes` 模板参数
   - 支持运行时确定单元节点数
   - 矩阵大小和循环边界动态化

3. **PhysicsField 类更新**
   - [HeatTransfer](fem/physics/classes/HeatTransfer.md) 和 [Electrostatics](fem/physics/classes/Electrostatics.md) 类实现了单元类型过滤机制
   - 添加了 `shouldAssembleElement` 方法，确保只有适当类型的单元参与域内组装
   - 移除了硬编码的节点数

4. **混合网格支持**
   - 内核现在可以处理混合网格，即同时包含不同类型和节点数的单元
   - 根据问题维度自动过滤参与组装的单元类型
   - 提高了代码的通用性和可扩展性

这些更改使有限元内核更加通用，能够处理不同类型的单元（混合网格），而不仅限于特定节点数的单元，符合项目的设计目标。