# Element 类

`Element` 类是有限元网格中单元的基类，表示由一组节点组成的几何实体。单元是有限元方法的基本计算单元，用于离散化求解域。该类定义了单元的通用接口，具体的单元类型通过派生类实现。

## 类定义

```cpp
class Element
```

## 枚举类型

### ElementType

单元类型枚举，定义了支持的单元类型：

- `Point` - 点单元
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

获取单元节点数量的纯虚函数，由派生类实现。

**返回值:**
- 单元包含的节点数量

### int getNodeId(size_t index) const

获取指定索引的节点ID。

**参数:**
- `index` - 节点索引

**返回值:**
- 指定索引节点的ID

**异常:**
- `std::out_of_range` - 当索引超出范围时抛出

### virtual ElementType getType() const = 0

获取单元类型的纯虚函数，由派生类实现。

**返回值:**
- 单元类型枚举值

## 派生类

- `PointElement` - 点单元，包含1个节点
- `LineElement` - 线单元，包含2个节点
- `TriElement` - 三角形单元，包含3个节点
- `QuadElement` - 四边形单元，包含4个节点
- `TetraElement` - 四面体单元，包含4个节点
- `HexaElement` - 六面体单元，包含8个节点

## 示例用法

```cpp
// 创建节点
auto node1 = std::make_unique<FEM::Node>(1, std::vector<double>{0.0, 0.0});
auto node2 = std::make_unique<FEM::Node>(2, std::vector<double>{1.0, 0.0});
auto node3 = std::make_unique<FEM::Node>(3, std::vector<double>{0.0, 1.0});

// 创建三角形单元
std::vector<FEM::Node*> nodes = {node1.get(), node2.get(), node3.get()};
auto element = std::make_unique<FEM::TriElement>(1, nodes);

// 获取单元信息
int id = triangle->getId();
int num_nodes = triangle->getNumNodes();
FEM::ElementType type = triangle->getType();
const auto& element_nodes = triangle->getNodes();
```

## 实现细节

`Element` 类是一个抽象基类，定义了有限元单元的通用接口。具体的单元类型通过派生类实现，每个派生类定义了特定的节点数量和单元类型。

这种设计允许有限元程序处理不同类型的单元，同时保持接口的一致性。

## 依赖关系

- [Node](Node.md) - 节点类
- STL - 向量、异常等标准库组件