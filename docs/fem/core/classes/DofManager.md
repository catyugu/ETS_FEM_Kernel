# DofManager 类

## 描述

`DofManager` 类负责管理有限元问题中的自由度（Degrees of Freedom）。它为网格中的每个节点分配自由度索引，并提供获取单元自由度、节点自由度等信息的接口。该类还支持稀疏模式预计算功能，以提高矩阵组装的性能。

## 类定义

```cpp
class DofManager
```

## 构造函数

### explicit DofManager(const Mesh& mesh)

构造函数，使用给定的网格初始化自由度管理器。

**参数:**
- `mesh` - 网格对象的引用

## 成员函数

### void buildDofMap(int dofs_per_node)

构建自由度映射表。

**参数:**
- `dofs_per_node` - 每个节点的自由度数量

### std::vector<int> getElementDofs(const Element& element) const

获取一个单元所有节点的自由度索引。

**参数:**
- `element` - 单元对象

**返回值:**
- 包含单元所有自由度索引的向量

### int getNodeDof(int node_id, int component) const

获取指定节点的特定自由度索引。

**参数:**
- `node_id` - 节点ID
- `component` - 自由度分量

**返回值:**
- 自由度索引

### size_t getNumDofs() const

获取总的自由度数量。

**返回值:**
- 总自由度数

### std::vector<std::pair<int, int>> computeSparsityPattern(const Mesh& mesh) const

计算稀疏模式，为矩阵预分配做准备。

**参数:**
- `mesh` - 网格对象

**返回值:**
- 包含所有非零元素位置的索引对向量

## 示例用法

```cpp
// 创建自由度管理器
FEM::DofManager dof_manager(mesh);

// 为标量问题（如热传导）构建自由度映射
dof_manager.buildDofMap(1);

// 获取单元的自由度
auto element_dofs = dof_manager.getElementDofs(element);

// 获取特定节点的自由度
int node_dof = dof_manager.getNodeDof(node_id, 0);

// 计算稀疏模式以优化矩阵组装
auto sparsity_pattern = dof_manager.computeSparsityPattern(mesh);
```

## 注意事项

1. 在调用其他方法之前，必须先调用 `buildDofMap` 方法
2. `computeSparsityPattern` 方法用于性能优化，可在矩阵组装前预计算稀疏模式
3. 自由度索引从0开始
4. 对于向量问题（如弹性力学），每个节点可能有多个自由度