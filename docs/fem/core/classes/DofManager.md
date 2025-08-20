# DofManager 类

## 描述

`DofManager` 类负责管理有限元网格中各个几何实体（节点、边、面、体）上的自由度。它为每个自由度分配全局唯一的索引，并提供计算稀疏矩阵模式的功能。

与之前版本相比，该类现在支持多种自由度类型，不仅支持传统的节点自由度，还支持边、面和体自由度，为使用边缘元等高级单元类型提供了支持。

## 类定义

```cpp
class DofManager
```

## 枚举类型

### DofType

自由度类型枚举，定义了支持的自由度类型：

- `NODE` - 节点自由度
- `EDGE` - 边自由度
- `FACE` - 面自由度
- `VOLUME` - 体自由度

## 构造函数

### explicit DofManager(const Mesh& mesh)

构造函数，使用给定的网格初始化自由度管理器。

**参数:**
- `mesh` - 网格对象的常量引用

## 成员函数

### void buildDofMap(int dofs_per_entity, DofType dof_type = DofType::NODE)

构建自由度映射。

**参数:**
- `dofs_per_entity` - 每个几何实体上的自由度数量
- `dof_type` - 自由度类型（默认为节点自由度）

### int getNodeDof(int node_id, int dof_component = 0) const

获取节点自由度索引。

**参数:**
- `node_id` - 节点ID
- `dof_component` - 自由度分量（对于矢量问题，如二维问题中的x和y分量）

**返回值:**
- 全局自由度索引，如果未找到则返回-1

### int getEdgeDof(int edge_id, int dof_component = 0) const

获取边自由度索引。

**参数:**
- `edge_id` - 边ID
- `dof_component` - 自由度分量

**返回值:**
- 全局自由度索引，如果未找到则返回-1

### int getFaceDof(int face_id, int dof_component = 0) const

获取面自由度索引。

**参数:**
- `face_id` - 面ID
- `dof_component` - 自由度分量

**返回值:**
- 全局自由度索引，如果未找到则返回-1

### int getVolumeDof(int volume_id, int dof_component = 0) const

获取体自由度索引。

**参数:**
- `volume_id` - 体ID
- `dof_component` - 自由度分量

**返回值:**
- 全局自由度索引，如果未找到则返回-1

### size_t getNumDofs() const

获取自由度总数。

**返回值:**
- 自由度总数

### std::vector<std::pair<int, int>> computeSparsityPattern(const Mesh& mesh) const

计算稀疏模式，用于预分配稀疏矩阵的存储空间。

**参数:**
- `mesh` - 网格对象

**返回值:**
- 稀疏模式（非零元素位置的集合）

## 实现细节

`DofManager` 类通过为每个几何实体分配连续的自由度索引来管理自由度。它支持多种自由度类型：

1. **节点自由度 (DofType::NODE)** - 传统的有限元自由度，定义在网格节点上
2. **边自由度 (DofType::EDGE)** - 定义在网格边上，用于边缘元等
3. **面自由度 (DofType::FACE)** - 定义在网格面上
4. **体自由度 (DofType::VOLUME)** - 定义在网格体单元内

对于矢量问题（如弹性力学），每个几何实体可能有多个自由度分量。例如，在二维问题中，每个节点可能有x和y两个方向的位移分量。

稀疏模式计算功能通过遍历所有单元并确定哪些自由度之间存在耦合关系来工作。这允许在组装全局矩阵之前预分配正确的存储空间，从而提高性能。

## 示例用法

```cpp
// 假设已经有一个网格对象
FEM::Mesh mesh = ...;

// 创建自由度管理器
FEM::DofManager dof_manager(mesh);

// 为标量问题构建节点自由度映射（每个节点1个自由度）
dof_manager.buildDofMap(1, FEM::DofType::NODE);

// 为二维矢量问题构建节点自由度映射（每个节点2个自由度）
dof_manager.buildDofMap(2, FEM::DofType::NODE);

// 获取特定节点的自由度索引
int dof_index = dof_manager.getNodeDof(5, 0); // 节点5的第0个自由度分量

// 计算稀疏模式
auto sparsity_pattern = dof_manager.computeSparsityPattern(mesh);
```

## 依赖关系

- [Mesh](../../mesh/classes/Mesh.md) - 网格类
- Eigen - 稀疏矩阵库