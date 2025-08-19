# LinearSolver 类

## 描述

`LinearSolver` 类提供线性方程组求解功能，用于求解有限元方法中产生的稀疏线性系统 Ax = b。该类封装了 Eigen 库的稀疏 LU 分解求解器，为有限元计算提供高效的线性求解能力。

## 类定义

```cpp
class LinearSolver
```

## 成员函数

### Eigen::VectorXd solve(const Eigen::SparseMatrix<double>& A, const Eigen::VectorXd& b) const

使用 Eigen 的稀疏 LU 分解求解线性系统 Ax = b。

**参数:**
- `A` - 稀疏矩阵（系数矩阵）
- `b` - 向量（右端项）

**返回值:**
- `Eigen::VectorXd` - 求解得到的解向量 x

**异常:**
- `std::runtime_error` - 当矩阵分解失败或求解失败时抛出

## 示例用法

```cpp
// 假设已经有了系数矩阵A和右端项b
Eigen::SparseMatrix<double> A;
Eigen::VectorXd b;

// 创建线性求解器
FEM::LinearSolver solver;

try {
    // 求解线性系统
    Eigen::VectorXd x = solver.solve(A, b);
    
    // 使用解向量x进行后续处理...
} catch (const std::runtime_error& e) {
    // 处理求解失败的情况
    std::cerr << "Linear solve failed: " << e.what() << std::endl;
}
```

## 实现细节

`LinearSolver` 类使用 Eigen 库的 `Eigen::SparseLU` 求解器来执行稀疏线性系统的求解。求解过程分为两个步骤：
1. 对系数矩阵 A 进行 LU 分解
2. 使用分解后的矩阵求解线性系统 Ax = b

求解成功后，会通过 [Utils::Logger](file:///E:/code/cpp/ETS_FEM_Kernel/utils/SimpleLogger.hpp#L15-L61) 记录日志信息。

## 依赖关系

- Eigen 3.4.0 - 稀疏矩阵运算和线性求解
- [utils/SimpleLogger.hpp](file:///E:/code/cpp/ETS_FEM_Kernel/utils/SimpleLogger.hpp) - 日志记录功能