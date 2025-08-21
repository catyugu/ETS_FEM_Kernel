# FEM::Mesh Namespace

网格命名空间，包含有限元网格相关的类和功能。

## 概述

Mesh命名空间提供了有限元网格的基本数据结构和操作功能，包括节点、单元的定义和管理。

## 类列表

### 基础类
- [Node](classes/Node.md) - 网格节点类
- [Element](classes/Element.md) - 单元基类

### 具体单元类型
- PointElement - 点单元
- LineElement - 线单元
- TriElement - 三角形单元
- QuadElement - 四边形单元
- TetraElement - 四面体单元
- HexaElement - 六面体单元

### 网格管理
- [Mesh](classes/Mesh.md) - 网格管理类

## 设计原则

1. **可扩展性**：支持多种单元类型
2. **统一接口**：所有单元类型继承自统一基类
3. **边界支持**：支持边界单元的定义和管理
4. **工厂模式**：提供统一网格生成接口

## 边界元素支持

Mesh类现在支持边界元素的管理，包括：

```cpp
// 添加边界单元
mesh->addBoundaryElement("boundary_name", std::make_unique<LineElement>(...));

// 获取边界单元
const auto& boundary_elements = mesh->getBoundaryElements("boundary_name");

// 获取边界节点
const auto& boundary_nodes = mesh->getBoundaryNodes("boundary_name");
```

## 使用示例

```cpp
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