# Mesh 类

## 描述

`Mesh` 类表示有限元分析中的网格，负责管理所有的节点和单元。它提供了添加节点和单元、获取节点和单元集合以及根据ID查找节点的功能。此外，还提供了创建规则网格的静态工厂方法。

## 类定义

```cpp
class Mesh
```

## 析构函数

### ~Mesh()

析构函数，负责释放所有节点和单元的内存。

## 成员函数

### void addNode(Node* node)

添加节点到网格中。

**参数:**
- `node` - 要添加的节点指针

### void addElement(Element* element)

添加单元到网格中。

**参数:**
- `element` - 要添加的单元指针

### const std::vector<Node*>& getNodes() const

获取网格中所有节点的引用。

**返回值:**
- 包含所有节点指针的向量的常量引用

### const std::vector<Element*>& getElements() const

获取网格中所有单元的引用。

**返回值:**
- 包含所有单元指针的向量的常量引用

### Node* getNodeById(int id) const

根据ID获取节点。

**参数:**
- `id` - 节点ID

**返回值:**
- 指定ID的节点指针，如果未找到则返回nullptr

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

// 添加节点
FEM::Node* node1 = new FEM::Node(1, {0.0, 0.0});
FEM::Node* node2 = new FEM::Node(2, {1.0, 0.0});
mesh->addNode(node1);
mesh->addNode(node2);

// 添加单元
FEM::LineElement* element = new FEM::LineElement(1, {node1, node2});
mesh->addElement(element);

// 获取网格信息
const auto& nodes = mesh->getNodes();
const auto& elements = mesh->getElements();

// 使用静态工厂方法创建规则网格
auto mesh_1d = FEM::Mesh::create_uniform_1d_mesh(1.0, 10);  // 长度为1，10个单元的一维网格
auto mesh_2d = FEM::Mesh::create_uniform_2d_mesh(1.0, 1.0, 10, 10);  // 1x1大小，10x10单元的二维网格
```

## 实现细节

`Mesh` 类内部使用两个向量分别存储节点和单元指针，并使用一个映射表 [node_map_](file:///E:/code/cpp/ETS_FEM_Kernel/fem/mesh/Mesh.hpp#L27) 来加速根据ID查找节点的操作。析构函数负责释放所有节点和单元的内存，避免内存泄漏。

静态工厂方法提供了创建规则网格的便捷方式，可以快速生成用于测试和简单分析的网格。

## 依赖关系

- [Node](file:///E:/code/cpp/ETS_FEM_Kernel/fem/mesh/Node.hpp#L11-L26) - 节点类
- [Element](file:///E:/code/cpp/ETS_FEM_Kernel/fem/mesh/Element.hpp#L28-L77) - 单元基类及各种具体单元类
- STL - 向量、映射、智能指针等标准库组件