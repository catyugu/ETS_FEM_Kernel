# LinearSolver 类

## 描述

`LinearSolver` 类提供了线性方程组的求解功能。它封装了不同的求解算法，包括直接求解器和迭代求解器，可以根据问题的特点选择合适的求解方法。

## 类定义

```cpp
class LinearSolver
```

## 枚举类型

### SolverType

求解器类型枚举。

```cpp
enum class SolverType {
    SparseLU,              // 稀疏LU分解（直接求解器）
    ConjugateGradient     // 共轭梯度法（迭代求解器）
};
```

## 构造函数

### explicit LinearSolver(SolverType type = SolverType::SparseLU)

构造函数，指定求解器类型。

**参数:**
- `type` - 求解器类型，默认为SparseLU

## 成员函数

### Eigen::VectorXd solve(const Eigen::SparseMatrix<double>& A, const Eigen::VectorXd& b) const

使用指定的求解算法求解线性系统 Ax = b。

**参数:**
- `A` - 稀疏矩阵（系数矩阵）
- `b` - 向量（右端项）

**返回值:**
- 解向量 x

**异常:**
- 如果求解失败，抛出 `std::runtime_error` 异常

## 求解器说明

### SparseLU（默认）

使用Eigen库的稀疏LU分解方法。这是一种直接求解器，适用于中小型问题，能够提供精确解。

特点：
- 精度高
- 对问题规模有一定限制
- 内存消耗较大
- 适用于需要高精度解的问题

### ConjugateGradient

使用共轭梯度法的迭代求解器。适用于大型对称正定系统。

特点：
- 内存效率高
- 适用于大型问题
- 解是近似的（取决于收敛容差）
- 对矩阵特性有要求（对称正定）

## 示例用法

```cpp
// 使用默认的SparseLU求解器
FEM::LinearSolver solver;
Eigen::VectorXd x = solver.solve(A, b);

// 使用共轭梯度求解器
FEM::LinearSolver cg_solver(FEM::SolverType::ConjugateGradient);
Eigen::VectorXd x = cg_solver.solve(A, b);
```

## 注意事项

1. 默认使用SparseLU直接求解器
2. 对于大型问题，可以考虑使用ConjugateGradient迭代求解器
3. 迭代求解器的收敛性和精度可以通过设置容差和最大迭代次数来控制
4. 求解失败时会抛出异常，需要适当处理