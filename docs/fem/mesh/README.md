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

## 依赖关系

Mesh 模块是整个有限元框架的基础，被 core 模块和 physics 模块所依赖。它不依赖于其他模块，仅使用标准库组件。

## 设计原则

1. **可扩展性**：支持多种单元类型
2. **统一接口**：所有单元类型继承自统一基类
3. **边界支持**：支持边界单元的定义和管理
4. **工厂模式**：提供统一网格生成接口

## 边界元素支持

Mesh类现在支持边界元素的管理，包括：

```
// 添加边界单元
mesh->addBoundaryElement("boundary_name", std::make_unique<LineElement>(...));

// 获取边界单元
const auto& boundary_elements = mesh->getBoundaryElements("boundary_name");

// 获取边界节点
const auto& boundary_nodes = mesh->getBoundaryNodes("boundary_name");
```

## 使用示例

```
// 创建网格
auto mesh = std::make_unique<Mesh>();

// 添加节点和单元
mesh->addNode(std::make_unique<Node>(0, std::vector<double>{0.0, 0.0}));
mesh->addNode(std::make_unique<Node>(1, std::vector<double>{1.0, 0.0}));
mesh->addNode(std::make_unique<Node>(2, std::vector<double>{1.0, 1.0}));
mesh->addNode(std::make_unique<Node>(3, std::vector<double>{0.0, 1.0}));

std::vector<FEM::Node*> nodes = {mesh->getNodes()[0].get(), mesh->getNodes()[1].get(), 
                                 mesh->getNodes()[2].get(), mesh->getNodes()[3].get()};
mesh->addElement(std::make_unique<QuadElement>(0, nodes));

// 添加边界信息
std::vector<FEM::Node*> boundary_nodes = {mesh->getNodes()[0].get(), mesh->getNodes()[1].get()};
mesh->addBoundaryElement("bottom", std::make_unique<LineElement>(0, boundary_nodes));
```