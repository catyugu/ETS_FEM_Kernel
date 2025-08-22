# Geometry 类

## 描述

`Geometry` 类作为顶层容器，持有核心的 `Mesh` 对象以及所有 `BoundaryDefinition` 对象。它将网格的几何/拓扑信息与边界的语义定义分离开来，提供更清晰的接口。系统的其他部分（如 `Problem` 类）将主要与 `Geometry` 类交互，而不是直接操作 `Mesh` 的边界。

## 类定义

```cpp
class Geometry
```

## 构造函数

### Geometry(std::unique_ptr<Mesh> mesh)

构造函数，使用给定的网格创建几何对象。

**参数:**
- `mesh` - 网格对象的智能指针

## 成员函数

### Mesh& getMesh()

获取网格对象的引用。

**返回值:**
- 网格对象的引用

### const Mesh& getMesh() const

获取网格对象的常量引用。

**返回值:**
- 网格对象的常量引用

### void addBoundary(std::unique_ptr<BoundaryDefinition> boundary)

添加边界定义到几何对象中。

**参数:**
- `boundary` - 边界定义对象的智能指针

### const BoundaryDefinition& getBoundary(const std::string& name) const

根据名称获取边界定义。

**参数:**
- `name` - 边界名称

**返回值:**
- 指定名称的边界定义对象的常量引用

**异常:**
- `std::runtime_error` - 当找不到指定名称的边界时抛出

## 示例用法

```cpp
// 创建网格
auto mesh = std::make_unique<FEM::Mesh>();

// 添加节点和单元
// ... 添加节点和单元 ...

// 创建几何对象
auto geometry = std::make_unique<FEM::Geometry>(std::move(mesh));

// 添加边界定义
auto left_boundary = std::make_unique<FEM::BoundaryDefinition>("left");
// ... 添加边界单元 ...
geometry->addBoundary(std::move(left_boundary));

// 获取网格
auto& mesh_ref = geometry->getMesh();

// 获取边界定义
const auto& boundary = geometry->getBoundary("left");
```

## 实现细节

`Geometry` 类通过将网格和边界定义封装在一起，实现了网格几何信息与边界语义定义的分离。这种设计使得网格生成和边界定义可以独立进行，提高了代码的模块化程度和可维护性。

## 依赖关系

- [Mesh](Mesh.md) - 网格类
- [BoundaryDefinition](BoundaryDefinition.md) - 边界定义类
- STL - 智能指针、字符串、映射等标准库组件