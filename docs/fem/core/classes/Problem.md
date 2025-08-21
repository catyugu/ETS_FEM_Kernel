# Problem 类

## 描述

`Problem` 类是有限元问题的主控制器类，负责协调整个求解过程。它管理网格、物理场、自由度、全局矩阵和向量，并提供组装、施加边界条件、求解和输出结果的接口。这是一个模板类，模板参数表示问题的维度。

与之前版本相比，该类现在支持多物理场耦合，可以同时处理多个物理场问题。

## 类定义

```cpp
template<int TDim, typename TScalar = double>
class Problem
```

**模板参数:**
- `TDim` - 问题的空间维度
- `TScalar` - 标量类型，默认为double，也可支持std::complex<double>等类型

## 构造函数

### Problem(std::unique_ptr<Mesh> mesh, std::unique_ptr<PhysicsField<TDim, TScalar>> physics)

构造函数，初始化问题对象。

**参数:**
- `mesh` - 网格对象的智能指针
- `physics` - 物理场对象的智能指针（继承自PhysicsField抽象类）

### Problem(std::unique_ptr<Mesh> mesh, std::unique_ptr<PhysicsField<TDim, TScalar>> physics, SolverType solver_type)

构造函数，初始化问题对象并指定求解器类型。

**参数:**
- `mesh` - 网格对象的智能指针
- `physics` - 物理场对象的智能指针（继承自PhysicsField抽象类）
- `solver_type` - 求解器类型（SparseLU或ConjugateGradient）

### Problem(std::unique_ptr<Mesh> mesh, std::vector<std::unique_ptr<PhysicsField<TDim, TScalar>>> physics_fields, SolverType solver_type)

新增构造函数，支持多物理场耦合。

**参数:**
- `mesh` - 网格对象的智能指针
- `physics_fields` - 物理场对象的智能指针向量
- `solver_type` - 求解器类型（SparseLU或ConjugateGradient）

## 成员函数

### void assemble()

组装全局刚度矩阵和载荷向量。该方法现在支持稀疏模式预计算，以提高矩阵组装的性能。

对于多物理场问题，该方法会依次组装每个物理场的贡献。

### void addDirichletBC(int node_id, double value)

添加 Dirichlet 边界条件。

**参数:**
- `node_id` - 节点ID
- `value` - 边界条件值

### void applyBCs()

应用边界条件到全局系统。

### void solve()

求解线性系统。根据构造时指定的求解器类型，使用相应的求解算法。


### const Mesh& getMesh() const

获取网格对象的引用。

### const Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& getSolution() const

获取解向量的常量引用。

### const PhysicsField<TDim, TScalar>& getPhysicsField(size_t index = 0) const

获取物理场对象的常量引用。

**参数:**
- `index` - 物理场索引，默认为0

**异常:**
- `std::out_of_range` - 当索引超出范围时抛出

### size_t getNumPhysicsFields() const

获取物理场数量。

**返回值:**
- 物理场数量

### const DofManager& getDofManager() const

获取自由度管理器的引用。

## 示例用法

```cpp
// 创建热传导问题
auto heat_physics = std::make_unique<FEM::HeatTransfer<2>>();
heat_physics->addKernel(
    std::make_unique<FEM::HeatDiffusionKernel<2, 4>>(material)
);

// 使用默认求解器创建单物理场问题
auto problem = std::make_unique<FEM::Problem<2>>(std::move(mesh), std::move(heat_physics));

// 或者指定求解器类型
auto problem_cg = std::make_unique<FEM::Problem<2>>(
    std::move(mesh), 
    std::move(heat_physics), 
    FEM::SolverType::ConjugateGradient
);

// 创建多物理场问题
std::vector<std::unique_ptr<FEM::PhysicsField<2>>> physics_fields;
physics_fields.push_back(std::make_unique<FEM::HeatTransfer<2>>());
physics_fields.push_back(std::make_unique<FEM::Electrostatics<2>>());
auto multiphysics_problem = std::make_unique<FEM::Problem<2>>(std::move(mesh), std::move(physics_fields));
```