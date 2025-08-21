# FEM Namespace

有限元方法核心命名空间，包含完整的有限元求解器实现。

## 概述

FEM命名空间提供了完整的有限元求解器实现，支持多种物理场问题的数值模拟。

## 子命名空间

- [Core](core/README.md) - 核心有限元功能
- [IO](io/README.md) - 输入输出功能
- [Kernels](kernels/README.md) - 物理内核实现
- [Materials](materials/README.md) - 材料属性定义
- [Mesh](mesh/README.md) - 网格数据结构
- [Physics](physics/README.md) - 物理场实现
- [BCs](bcs/README.md) - 边界条件实现

## 核心设计原则

1. **模块化架构**：按功能划分子命名空间
2. **面向接口编程**：通过抽象基类提供统一接口
3. **可扩展性**：支持添加新的物理场和单元类型
4. **多物理场耦合**：支持多个物理场的耦合求解
5. **边界条件抽象**：统一的边界条件处理框架

## 使用示例

```cpp
// 创建网格
auto mesh = FEM::Mesh::create_uniform_2d_mesh(1.0, 1.0, 10, 10);

// 创建材料
auto material = std::make_unique<FEM::Material>();
material->setProperty("thermal_conductivity", 400.0);

// 创建物理场
auto physics = std::make_unique<FEM::HeatTransfer<2>>();
physics->addKernel(std::make_unique<FEM::HeatDiffusionKernel<2, 4>>(*material));

// 添加边界条件
physics->addBoundaryCondition(
    std::make_unique<FEM::DirichletBC<2>>("left_boundary", 300.0)
);
physics->addBoundaryCondition(
    std::make_unique<FEM::NeumannBC<2>>("right_boundary", 0.0)
);

// 创建并求解问题
FEM::Problem<2> problem(std::move(mesh), std::move(physics));
problem.assemble();
problem.solve();

// 获取结果
const auto& solution = problem.getSolution();
```