# Problem 类

## 描述

`Problem` 类是有限元问题的主控制器类，负责协调整个求解过程。它管理网格、物理场、自由度、全局矩阵和向量，并提供组装、施加边界条件、求解和输出结果的接口。这是一个模板类，模板参数表示问题的维度。

## 类定义

```cpp
template<int TDim>
class Problem
```

## 构造函数

### Problem(std::unique_ptr<Mesh> mesh, std::unique_ptr<HeatTransfer<TDim>> physics)

构造函数，初始化问题对象。

**参数:**
- `mesh` - 网格对象的智能指针
- `physics` - 物理场对象的智能指针

## 成员函数

### void assemble()

组装全局刚度矩阵和载荷向量。

### void addDirichletBC(int node_id, double value)

添加 Dirichlet 边界条件。

**参数:**
- `node_id` - 节点ID
- `value` - 边界条件值

### void applyBCs()

应用边界条件到全局系统。

### void solve()

求解线性系统。

### void printResults() const

打印计算结果到标准输出。

### const Mesh& getMesh() const

获取网格对象的引用。

**返回值:**
- 网格对象的常量引用

### const Eigen::VectorXd& getSolution() const

获取解向量的引用。

**返回值:**
- 解向量的常量引用

### const HeatTransfer<TDim>& getPhysicsField() const

获取物理场对象的引用。

**返回值:**
- 物理场对象的常量引用

### const DofManager& getDofManager() const

获取自由度管理器的引用。

**返回值:**
- 自由度管理器的常量引用

## 示例用法

```cpp
// 创建网格和物理场对象
auto mesh = std::make_unique<FEM::Mesh>();
auto physics = std::make_unique<FEM::HeatTransfer<2>>();

// 创建问题对象
FEM::Problem<2> problem(std::move(mesh), std::move(physics));

// 组装系统
problem.assemble();

// 添加边界条件
problem.addDirichletBC(0, 300.0); // 节点0温度为300K

// 应用边界条件
problem.applyBCs();

// 求解
problem.solve();

// 输出结果
problem.printResults();
```

## 实现细节

`Problem` 类是有限元求解流程的核心协调者，它：
1. 管理问题的所有数据组件（网格、物理场、自由度等）
2. 控制求解流程（组装、边界条件、求解）
3. 提供结果访问接口

该类使用模板参数 [TDim](file:///E:/code/cpp/ETS_FEM_Kernel/fem/core/Problem.hpp#L27) 表示问题的空间维度，目前主要用于热传导问题。

## 依赖关系

- [Mesh](file:///E:/code/cpp/ETS_FEM_Kernel/fem/mesh/Mesh.hpp#L17-L45) - 网格数据结构
- [HeatTransfer](file:///E:/code/cpp/ETS_FEM_Kernel/fem/physics/HeatTransfer.hpp#L22-L66) - 热传导物理场
- [DofManager](file:///E:/code/cpp/ETS_FEM_Kernel/fem/core/DofManager.hpp#L19-L61) - 自由度管理器
- [LinearSolver](file:///E:/code/cpp/ETS_FEM_Kernel/fem/core/LinearSolver.hpp#L16-L54) - 线性求解器
- Eigen - 稀疏矩阵和向量运算