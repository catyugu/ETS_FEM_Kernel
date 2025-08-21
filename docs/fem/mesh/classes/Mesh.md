# Mesh 类

## 描述

`Mesh` 类表示有限元分析中的网格，负责管理所有的节点和单元。它提供了添加节点和单元、获取节点和单元集合以及根据ID查找节点的功能。此外，还提供了创建规则网格的静态工厂方法。

## 类定义

```cpp
class Mesh
```

## 析构函数

### ~Mesh()

析构函数，负责释放所有节点和单元的内存。使用默认析构函数，因为智能指针会自动管理内存。

## 成员函数

### void addNode(std::unique_ptr<Node> node)

添加节点到网格中。

**参数:**
- `node` - 要添加的节点智能指针

### void addElement(std::unique_ptr<Element> element)

添加单元到网格中。

**参数:**
- `element` - 要添加的单元智能指针

### const std::vector<std::unique_ptr<Node>>& getNodes() const

获取网格中所有节点的引用。

**返回值:**
- 包含所有节点智能指针的向量的常量引用

### const std::vector<std::unique_ptr<Element>>& getElements() const

获取网格中所有单元的引用。

**返回值:**
- 包含所有单元智能指针的向量的常量引用

### Node* getNodeById(int id) const

根据ID获取节点。

**参数:**
- `id` - 节点ID

**返回值:**
- 指定ID的节点指针，如果未找到则返回空指针

## 静态工厂方法

### static std::unique_ptr<Mesh> create_uniform_1d_mesh(double length, int num_elements)

创建一维规则网格。

**参数:**
- `length` - 网格长度
- `num_elements` - 单元数量

**返回值:**
- 创建的网格对象的智能指针

### static std::unique_ptr<Mesh> create_uniform_2d_mesh(double width, double height, int nx, int ny)

创建二维规则网格。

**参数:**
- `width` - 网格宽度
- `height` - 网格高度
- `nx` - X方向单元数量
- `ny` - Y方向单元数量

**返回值:**
- 创建的网格对象的智能指针

### static std::unique_ptr<Mesh> create_uniform_3d_mesh(double width, double height, double depth, int nx, int ny, int nz)

创建三维规则网格。

**参数:**
- `width` - 网格宽度
- `height` - 网格高度
- `depth` - 网格深度
- `nx` - X方向单元数量
- `ny` - Y方向单元数量
- `nz` - Z方向单元数量

**返回值:**
- 创建的网格对象的智能指针

## 示例用法

```cpp
// 创建网格对象
auto mesh = std::make_unique<FEM::Mesh>();

// 添加节点和单元
auto node1 = std::make_unique<FEM::Node>(1, std::vector<double>{0.0, 0.0});
auto node2 = std::make_unique<FEM::Node>(2, std::vector<double>{1.0, 0.0});
auto node3 = std::make_unique<FEM::Node>(3, std::vector<double>{0.0, 1.0});

mesh->addNode(std::move(node1));
mesh->addNode(std::move(node2));
mesh->addNode(std::move(node3));

std::vector<FEM::Node*> nodes = {node1.get(), node2.get(), node3.get()};
auto element = std::make_unique<FEM::TriElement>(1, nodes);
mesh->addElement(std::move(element));

// 创建规则网格
auto uniform_mesh = FEM::Mesh::create_uniform_2d_mesh(1.0, 1.0, 10, 10);
```

## 实现细节

`Mesh` 类使用智能指针管理节点和单元的内存，避免了手动内存管理的问题。通过`getNodeById`方法可以方便地根据ID查找节点，这在施加边界条件等操作中非常有用。

## 依赖关系

- [Node](Node.md) - 节点类
- [Element](Element.md) - 单元类
- STL - 向量、智能指针等标准库组件