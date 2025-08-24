# DofManager 类

## 描述

`DofManager` 类负责管理有限元网格中各个几何实体（节点、边、面、体）上的自由度。它为每个自由度分配全局唯一的索引，并提供计算稀疏矩阵模式的功能。

与之前版本相比，该类现在支持基于变量的自由度管理，可以同时处理多个物理场的自由度，为多物理场耦合问题提供了支持。该设计支持多种自由度类型（节点、边、面、体自由度），为使用边缘元等高级单元类型提供了支持。

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

### void addVariable(const std::string& name, DofType type, int components = 1)

添加一个自由度变量。

**参数:**
- `name` - 变量名称（例如"Voltage"或"Temperature"）
- `type` - 自由度类型
- `components` - 每个实体的分量数（例如矢量为2或3，默认为1）

### void finalize()

根据已添加的变量构建所有自由度映射。

### void buildDofMap(int dofs_per_entity, DofType dof_type = DofType::NODE) [[deprecated]]

构建自由度映射（旧版，内部将创建一个名为"default"的变量）。

**参数:**
- `dofs_per_entity` - 每个几何实体上的自由度数量
- `dof_type` - 自由度类型（默认为节点自由度）

**注意:** 此函数已被标记为废弃，推荐使用`addVariable`和`finalize`接口。

### int getNodeDof(const std::string& var_name, int node_id, int component = 0) const

获取指定变量在节点上的自由度索引。

**参数:**
- `var_name` - 变量名称
- `node_id` - 节点ID
- `component` - 自由度分量（对于矢量问题，如二维问题中的x和y分量）

**返回值:**
- 全局自由度索引，如果未找到则返回-1

### int getEdgeDof(const std::string& var_name, int edge_id, int component = 0) const

获取指定变量在边上的自由度索引。

**参数:**
- `var_name` - 变量名称
- `edge_id` - 边ID
- `component` - 自由度分量

**返回值:**
- 全局自由度索引，如果未找到则返回-1

### int getFaceDof(const std::string& var_name, int face_id, int component = 0) const

获取指定变量在面上的自由度索引。

**参数:**
- `var_name` - 变量名称
- `face_id` - 面ID
- `component` - 自由度分量

**返回值:**
- 全局自由度索引，如果未找到则返回-1

### int getVolumeDof(const std::string& var_name, int volume_id, int component = 0) const

获取指定变量在体上的自由度索引。

**参数:**
- `var_name` - 变量名称
- `volume_id` - 体ID
- `component` - 自由度分量

**返回值:**
- 全局自由度索引，如果未找到则返回-1

### size_t getNumDofs() const

获取总自由度数。

**返回值:**
- 所有变量的自由度总数

### size_t getNumDofs(const std::string& var_name) const

获取特定变量的自由度数。

**参数:**
- `var_name` - 变量名称

**返回值:**
- 指定变量的自由度数

### std::vector<std::pair<int, int>> computeSparsityPattern(const Mesh& mesh) const

计算稀疏矩阵的稀疏模式。

**参数:**
- `mesh` - 网格对象

**返回值:**
- 非零元素位置的pair向量，表示稀疏模式

## 使用示例

```cpp
// 创建自由度管理器
FEM::DofManager dof_manager(mesh);

// 为热传导问题添加温度变量
dof_manager.addVariable("Temperature", FEM::DofType::NODE);

// 为静电场问题添加电势变量
dof_manager.addVariable("Voltage", FEM::DofType::NODE);

// 构建自由度映射
dof_manager.finalize();

// 获取节点自由度索引
int temp_dof = dof_manager.getNodeDof("Temperature", node_id);
int volt_dof = dof_manager.getNodeDof("Voltage", node_id);
```

## 实现细节

`DofManager` 现在使用基于变量的自由度管理机制。每个物理场变量都有自己的自由度映射，这使得多物理场耦合变得更加容易管理。当需要获取某个节点上特定变量的自由度索引时，必须提供变量名称。

这种设计的主要优势包括：
1. 支持多物理场耦合
2. 清晰的变量标识
3. 更好的代码可读性和可维护性
4. 避免不同物理场之间的自由度混淆

## 注意事项

- 在调用 `getNodeDof` 等方法之前，必须先调用 `addVariable` 和 `finalize`
- 变量名称必须唯一
- 如果请求的变量不存在，方法将返回-1