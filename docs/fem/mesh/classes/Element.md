# Element 类

## 描述

`Element` 类是有限元网格中单元的基类，表示由一组节点组成的几何实体。单元是有限元方法的基本计算单元，用于离散化求解域。该类定义了单元的通用接口，具体的单元类型通过派生类实现。

## 类定义

```cpp
class Element
```

## 枚举类型

### ElementType

单元类型枚举，定义了支持的单元类型：

- `Line` - 线单元
- `Triangle` - 三角形单元
- `Quadrilateral` - 四边形单元
- `Tetrahedron` - 四面体单元
- `Hexahedron` - 六面体单元

## 构造函数

### Element(int id, const std::vector<Node*>& nodes)

构造函数，使用给定的ID和节点数组初始化单元对象。

**参数:**
- `id` - 单元的唯一标识符
- `nodes` - 构成单元的节点指针数组

## 成员函数

### virtual ~Element() = default

虚析构函数，确保通过基类指针删除派生类对象时能正确调用派生类的析构函数。

### int getId() const

获取单元ID。

**返回值:**
- 单元的唯一标识符

### const std::vector<Node*>& getNodes() const

获取单元节点。

**返回值:**
- 构成单元的节点指针数组的常量引用

### virtual int getNumNodes() const = 0

纯虚函数，获取单元节点数量。由派生类实现。

**返回值:**
- 单元节点数量

### virtual ElementType getType() const = 0

纯虚函数，获取单元类型。由派生类实现。

**返回值:**
- 单元类型枚举值

## 派生类

### LineElement

线单元类，表示由2个节点组成的线段单元。

### TriElement

三角形单元类，表示由3个节点组成的三角形单元。

### QuadElement

四边形单元类，表示由4个节点组成的四边形单元。

### HexaElement

六面体单元类，表示由8个节点组成的六面体单元。

## 示例用法

```cpp
// 假设已经有节点对象
FEM::Node* node1 = new FEM::Node(1, {0.0, 0.0});
FEM::Node* node2 = new FEM::Node(2, {1.0, 0.0});
FEM::Node* node3 = new FEM::Node(3, {0.0, 1.0});

// 创建三角形单元
std::vector<FEM::Node*> nodes = {node1, node2, node3};
FEM::TriElement triangle(1, nodes);

// 获取单元信息
int id = triangle.getId();
int num_nodes = triangle.getNumNodes();
FEM::ElementType type = triangle.getType();
const auto& element_nodes = triangle.getNodes();
```

## 实现细节

`Element` 类是一个抽象基类，定义了有限元单元的通用接口。具体的单元类型通过派生类实现，每个派生类定义了特定的节点数量和单元类型。

这种设计允许有限元程序处理不同类型的单元，同时保持接口的一致性。

## 依赖关系

- [Node](file:///E:/code/cpp/ETS_FEM_Kernel/fem/mesh/Node.hpp#L11-L26) - 节点类
- STL - 向量等标准库组件