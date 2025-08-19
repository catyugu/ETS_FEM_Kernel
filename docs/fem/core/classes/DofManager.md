# DofManager 类

## 描述

`DofManager`（自由度管理器）类负责管理有限元网格中节点的自由度（Degrees of Freedom, DOF）。它为每个节点分配唯一的自由度索引，并提供查询特定节点或单元自由度索引的功能。

在有限元方法中，自由度是问题求解的基本未知量。例如，在热传导问题中，每个节点的温度值就是一个自由度；在结构力学问题中，每个节点可能有多个自由度（如x、y、z方向的位移）。

## 类定义

```cpp
class DofManager
```

## 构造函数

### DofManager(const Mesh& mesh)

构造函数，使用给定的网格初始化自由度管理器。

**参数:**
- `mesh` - 用于初始化自由度管理器的网格引用

## 成员函数

### void buildDofMap(int dofs_per_node)

构建自由度映射表。

**参数:**
- `dofs_per_node` - 每个节点的自由度数量（例如，热传导问题为1）

**异常:**
- `std::invalid_argument` - 当[dofs_per_node](file:///E:/code/cpp/ETS_FEM_Kernel/fem/core/DofManager.cpp#L14)小于等于0时抛出

### std::vector<int> getElementDofs(const Element& element) const

获取指定单元所有节点的自由度索引。

**参数:**
- `element` - 要获取自由度索引的单元

**返回值:**
- 包含该单元所有节点自由度索引的向量

**异常:**
- `std::out_of_range` - 当节点ID不存在于映射表中时抛出

### int getNodeDof(int node_id, int component) const

获取指定节点的特定自由度索引。

**参数:**
- `node_id` - 节点ID
- `component` - 自由度分量（对于每个节点的第几个自由度）

**返回值:**
- 指定节点特定自由度的全局索引

**异常:**
- `std::out_of_range` - 当[component](file:///E:/code/cpp/ETS_FEM_Kernel/utils/ShapeFunctions.hpp#L38-L38)超出范围或节点ID不存在于映射表中时抛出

### size_t getNumDofs() const

获取总的自由度数量。

**返回值:**
- 网格中所有节点的自由度总数

## 示例用法

```cpp
// 假设已经有一个网格对象mesh
FEM::DofManager dof_manager(mesh);

// 为每个节点分配1个自由度（例如热传导问题）
dof_manager.buildDofMap(1);

// 获取总自由度数
size_t total_dofs = dof_manager.getNumDofs();

// 获取特定单元的自由度
auto element = mesh.getElements()[0];
std::vector<int> element_dofs = dof_manager.getElementDofs(element);
```

## 实现细节

`DofManager`内部使用一个`std::map<int, int>`来存储节点ID到其第一个自由度索引的映射。对于每个节点，其自由度索引是连续的，从其第一个自由度开始，根据每个节点的自由度数量进行计算。