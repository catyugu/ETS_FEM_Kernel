# Node 类

## 描述

`Node` 类表示有限元网格中的节点，存储节点的唯一标识符和空间坐标。节点是有限元网格的基本组成元素之一，用于定义单元的几何形状和插值函数。

## 类定义

```cpp
class Node
```

## 构造函数

### Node(int id, const std::vector<double>& coords)

构造函数，使用给定的ID和坐标初始化节点对象。

**参数:**
- `id` - 节点的唯一标识符
- `coords` - 节点坐标的向量，可以是1D、2D或3D

## 成员函数

### int getId() const

获取节点ID。

**返回值:**
- 节点的唯一标识符

### const std::vector<double>& getCoords() const

获取节点坐标。

**返回值:**
- 节点坐标的常量引用

## 示例用法

```cpp
// 创建2D节点
FEM::Node node_2d(1, {1.0, 2.0});

// 获取节点ID和坐标
int id = node_2d.getId();
const auto& coords = node_2d.getCoords();
std::cout << "Node " << id << " at (" << coords[0] << ", " << coords[1] << ")" << std::endl;

// 创建3D节点
FEM::Node node_3d(2, {1.0, 2.0, 3.0});
```

## 实现细节

`Node` 类是一个简单的数据容器类，存储节点的ID和坐标信息。坐标使用 `std::vector<double>` 存储，这使得节点可以表示1D、2D或3D空间中的点。

## 依赖关系

- STL - 向量等标准库组件