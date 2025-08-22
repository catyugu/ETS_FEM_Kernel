# FEM Core 模块

核心模块包含有限元方法的核心组件，这些组件协同工作，提供有限元问题的完整求解流程。

## 模块组成

### 主要类

- [Problem](classes/Problem.md) - 有限元问题主控制器类，协调整个求解过程
- [DofManager](classes/DofManager.md) - 自由度管理器，处理节点、边、面和体自由度
- [BoundaryCondition](classes/BoundaryCondition.md) - 边界条件抽象基类
- [LinearSolver](classes/LinearSolver.md) - 线性求解器接口
- [FEValues](classes/FEValues.md) - 有限元值计算类（已整合面单元功能）
- [ReferenceElement](classes/ReferenceElement.md) - 参考单元类
- [QuadraturePoint](classes/QuadraturePoint.md) - 积分点类（FEValues的内部组件）
- [AnalysisTypes](classes/AnalysisTypes.md) - 分析类型枚举

### 功能特点

1. **统一的有限元值计算接口**：
   - 使用统一的 [FEValues](classes/FEValues.md) 类处理体单元和面单元
   - 引入现代C++迭代器接口，支持范围for循环
   - 提供更安全、更直观的API

2. **性能优化**：
   - 通过 [ReferenceElement](classes/ReferenceElement.md) 类缓存参考单元上的形函数值、导数以及积分点信息，避免重复计算
   - 使用高效的矩阵运算库Eigen进行数值计算

3. **模块化设计**：
   - 各个类职责明确，耦合度低
   - 易于扩展和维护

### 类关系图

```
           +----------------+
           |   Problem      |
           +----------------+
                  |
        +-------------------+
        |   DofManager      |
        +-------------------+
                  |
        +-------------------+     +---------------------+
        |  LinearSolver     |<----|  Eigen::SparseMatrix|
        +-------------------+     +---------------------+
                  |
        +-------------------+
        |   FEValues        |
        +-------------------+
           |        |
+-----------------+ +------------------+
| ReferenceElement| | QuadraturePoint  |
+-----------------+ +------------------+
```

### 使用示例

```cpp
// 创建问题对象
FEM::Problem problem;

// 设置网格和边界条件
// ...

// 组装系统矩阵
problem.assemble();

// 求解
problem.solve();

// 输出结果
problem.exportResults("results.vtu");
```

### 性能优化

1. **避免重复计算**：
   - 使用 [ReferenceElement](classes/ReferenceElement.md) 缓存机制避免重复计算参考单元信息
   - 预计算并存储所有积分点的形函数值和导数

2. **高效的数据结构**：
   - 使用Eigen库进行矩阵运算
   - 采用Triplet列表方式组装稀疏矩阵，提高组装效率

3. **内存优化**：
   - 通过 [QuadraturePoint](classes/QuadraturePoint.md) 类提供对积分点数据的引用访问，避免不必要的数据复制