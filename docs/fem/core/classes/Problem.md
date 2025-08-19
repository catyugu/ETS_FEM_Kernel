# Problem 类

## 描述

`Problem` 类是有限元问题的主控制器类，负责协调整个求解过程。它管理网格、物理场、自由度、全局矩阵和向量，并提供组装、施加边界条件、求解和输出结果的接口。这是一个模板类，模板参数表示问题的维度。

## 类定义

```cpp
template<int TDim>
class Problem
```

## 构造函数

### Problem(std::unique_ptr<Mesh> mesh, std::unique_ptr<PhysicsField<TDim>> physics)

构造函数，初始化问题对象。

**参数:**
- `mesh` - 网格对象的智能指针
- `physics` - 物理场对象的智能指针（继承自PhysicsField抽象类）

### Problem(std::unique_ptr<Mesh> mesh, std::unique_ptr<PhysicsField<TDim>> physics, SolverType solver_type)

构造函数，初始化问题对象并指定求解器类型。

**参数:**
- `mesh` - 网格对象的智能指针
- `physics` - 物理场对象的智能指针（继承自PhysicsField抽象类）
- `solver_type` - 求解器类型（SparseLU或ConjugateGradient）

## 成员函数

### void assemble()

组装全局刚度矩阵和载荷向量。该方法现在支持稀疏模式预计算，以提高矩阵组装的性能。

### void addDirichletBC(int node_id, double value)

添加 Dirichlet 边界条件。

**参数:**
- `node_id` - 节点ID
- `value` - 边界条件值

### void applyBCs()

应用边界条件到全局系统。

### void solve()

求解线性系统。根据构造时指定的求解器类型，使用相应的求解算法。

### void printResults() const

打印计算结果到标准输出。

### const Mesh& getMesh() const

获取网格对象的引用。

### const Eigen::VectorXd& getSolution() const

获取解向量的引用。

### const PhysicsField<TDim>& getPhysicsField() const

获取物理场对象的引用。

### const DofManager& getDofManager() const

获取自由度管理器的引用。

## 示例用法

```cpp
// 创建热传导问题
auto heat_physics = std::make_unique<FEM::HeatTransfer<2>>();
heat_physics->addKernel(
    std::make_unique<FEM::HeatDiffusionKernel<2, 4>>(material)
);

// 使用默认求解器创建问题
auto problem = std::make_unique<FEM::Problem<2>>(std::move(mesh), std::move(heat_physics));

// 或者指定求解器类型
auto problem_cg = std::make_unique<FEM::Problem<2>>(
    std::move(mesh), 
    std::move(heat_physics), 
    FEM::SolverType::ConjugateGradient
);
```

## 注意事项

1. Problem类现在使用抽象的`PhysicsField`类而不是具体的`HeatTransfer`类，支持多种物理场
2. 新增了对稀疏模式预计算的支持，以提高大型问题的组装性能
3. 支持多种求解器类型，包括直接求解器（SparseLU）和迭代求解器（ConjugateGradient）
4. 该类是模板类，模板参数`TDim`表示问题的维度（1D、2D或3D）