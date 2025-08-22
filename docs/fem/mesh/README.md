# FEM Mesh 模块

## 概述

Mesh 模块负责有限元分析中的网格管理，包括节点、单元以及几何拓扑结构的表示。它提供了创建和操作有限元网格的基本功能。

## 类和功能

### [Edge](classes/Mesh.md#Edge)

表示网格中的边，包含边的唯一标识符和组成边的两个节点ID。

### [Face](classes/Mesh.md#Face)

表示网格中的面，包含面的唯一标识符和组成面的节点ID列表。

### [Mesh](classes/Mesh.md)

网格类，负责管理有限元分析中的网格。它提供了添加节点和单元、获取节点和单元集合以及根据ID查找节点的功能。

该类还负责管理网格的几何拓扑结构，包括边和面的识别与存储。通过`buildTopology`方法，可以识别并存储网格中所有唯一的边和面。这为自由度管理器等组件提供了支持，使它们能够直接使用这些几何实体，而无需自己计算。

Mesh 类还提供了创建规则网格的静态工厂方法，包括一维、二维和三维规则网格。

### [Node](classes/Node.md)

节点类，表示网格中的节点，包含节点ID和坐标信息。

### [Element](classes/Element.md)

单元类，表示网格中的单元，是各种具体单元类型的基类。

### [Geometry](classes/Geometry.md)

几何类，作为顶层容器持有核心的Mesh对象以及所有BoundaryDefinition对象。它将网格的几何/拓扑信息与边界的语义定义分离开来，提供更清晰的接口。

### [BoundaryDefinition](classes/BoundaryDefinition.md)

边界定义类，用于定义一个命名的边界及其包含的边界单元。该类包含边界的名称以及构成该边界的低维单元。

## 依赖关系

Mesh 模块是整个有限元框架的基础，被 core 模块和 physics 模块所依赖。它不依赖于其他模块，仅使用标准库组件。

## 设计原则

1. **可扩展性**：支持多种单元类型
2. **统一接口**：所有单元类型继承自统一基类
3. **边界支持**：支持边界单元的定义和管理
4. **工厂模式**：提供统一网格生成接口
5. **职责分离**：将网格几何信息与边界语义定义分离

## 边界元素支持

Mesh类现在通过Geometry和BoundaryDefinition类管理边界元素：

```cpp
// 创建几何对象
auto geometry = FEM::Mesh::create_uniform_1d_mesh(1.0, 10);

// 获取边界定义
const auto& boundary = geometry->getBoundary("left");

// 获取边界单元
const auto& boundary_elements = boundary.getElements();

// 获取边界节点
const auto& boundary_nodes = boundary.getUniqueNodeIds();
```

## 使用示例

```cpp
// 创建几何对象（包含网格和边界定义）
auto geometry = FEM::Mesh::create_uniform_2d_mesh(1.0, 1.0, 10, 10);

// 访问网格
auto& mesh = geometry->getMesh();

// 访问边界定义
const auto& left_boundary = geometry->getBoundary("left");
const auto& right_boundary = geometry->getBoundary("right");

// 添加自定义边界定义
auto custom_boundary = std::make_unique<FEM::BoundaryDefinition>("custom");
// ... 添加边界单元 ...
geometry->addBoundary(std::move(custom_boundary));
```