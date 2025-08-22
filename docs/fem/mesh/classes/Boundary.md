# Boundary 类

## 描述

`Boundary` 类用于定义一个命名的边界。这个类将包含边界的名称以及构成该边界的低维单元（例如，对于3D六面体单元的边界，其边界单元是2D的四边形单元）。它提供了一种清晰的方式来管理和访问边界信息，将边界语义与网格几何信息分离。

## 类定义

```cpp
class Boundary
```

## 构造函数

### Boundary(const std::string& name)

构造函数，使用给定的名称创建边界定义。

**参数:**
- `name` - 边界名称

## 成员函数

### void addElement(std::unique_ptr<Element> element)

向边界定义中添加单元。

**参数:**
- `element` - 要添加的单元智能指针

### const std::string& getName() const

获取边界名称。

**返回值:**
- 边界名称的常量引用

### const std::vector<std::unique_ptr<Element>>& getElements() const

获取边界上所有单元的引用。

**返回值:**
- 包含所有边界单元智能指针的向量的常量引用

### std::vector<int> getUniqueNodeIds() const

获取边界上所有唯一的节点ID。

**返回值:**
- 包含所有唯一节点ID的向量

## 示例用法

```cpp
// 创建边界定义
auto boundary = std::make_unique<FEM::Boundary>("left");

// 添加边界单元
auto node = mesh->getNodeById(0);
if (node) {
    boundary->addElement(std::make_unique<FEM::PointElement>(0, std::vector<FEM::Node*>{node}));
}

// 获取边界信息
const std::string& name = boundary->getName();
const auto& elements = boundary->getElements();
std::vector<int> node_ids = boundary->getUniqueNodeIds();
```

## 实现细节

`Boundary` 类通过存储边界单元和提供辅助函数来获取唯一节点ID，简化了边界条件的施加过程。它与 `Geometry` 类配合使用，实现了网格几何信息与边界语义定义的分离。

## 依赖关系

- [Element](Element.md) - 单元类
- STL - 智能指针、字符串、向量、集合等标准库组件