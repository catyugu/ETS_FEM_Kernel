# ShapeFunctions 类

## 描述

`ShapeFunctions` 类提供有限元分析中各种单元类型的形函数及其导数的计算方法。形函数是有限元方法的核心概念，用于在单元内部插值场变量。该类支持一维线单元、二维四边形单元和三维六面体单元的形函数计算。

## 类定义

```cpp
class ShapeFunctions
```

## 成员函数

### static void getLineShapeFunctions(int order, double xi, Eigen::VectorXd& N)

计算一维线单元的形函数值。

**参数:**
- `order` - 形函数阶次（目前仅支持1阶）
- `xi` - 局部坐标
- `N` - 输出的形函数值向量

**异常:**
- `std::invalid_argument` - 当阶次大于1时抛出

### static void getLineShapeFunctionDerivatives(int order, double xi, Eigen::MatrixXd& dN_dxi)

计算一维线单元形函数的导数。

**参数:**
- `order` - 形函数阶次（目前仅支持1阶）
- `xi` - 局部坐标
- `dN_dxi` - 输出的形函数导数矩阵

**异常:**
- `std::invalid_argument` - 当阶次大于1时抛出

### static void getQuadShapeFunctions(int order, double xi, double eta, Eigen::VectorXd& N)

计算二维四边形单元的形函数值。

**参数:**
- `order` - 形函数阶次（目前仅支持1阶）
- `xi` - 局部坐标xi方向
- `eta` - 局部坐标eta方向
- `N` - 输出的形函数值向量

**异常:**
- `std::invalid_argument` - 当阶次大于1时抛出

### static void getQuadShapeFunctionDerivatives(int order, double xi, double eta, Eigen::MatrixXd& dN_dxi)

计算二维四边形单元形函数的导数。

**参数:**
- `order` - 形函数阶次（目前仅支持1阶）
- `xi` - 局部坐标xi方向
- `eta` - 局部坐标eta方向
- `dN_dxi` - 输出的形函数导数矩阵（第一行是xi方向导数，第二行是eta方向导数）

**异常:**
- `std::invalid_argument` - 当阶次大于1时抛出

### static void getHexShapeFunctions(int order, double xi, double eta, double zeta, Eigen::VectorXd& N)

计算三维六面体单元的形函数值。

**参数:**
- `order` - 形函数阶次（目前仅支持1阶）
- `xi` - 局部坐标xi方向
- `eta` - 局部坐标eta方向
- `zeta` - 局部坐标zeta方向
- `N` - 输出的形函数值向量

**异常:**
- `std::invalid_argument` - 当阶次大于1时抛出

### static void getHexShapeFunctionDerivatives(int order, double xi, double eta, double zeta, Eigen::MatrixXd& dN_dxi)

计算三维六面体单元形函数的导数。

**参数:**
- `order` - 形函数阶次（目前仅支持1阶）
- `xi` - 局部坐标xi方向
- `eta` - 局部坐标eta方向
- `zeta` - 局部坐标zeta方向
- `dN_dxi` - 输出的形函数导数矩阵（第一行是xi方向导数，第二行是eta方向导数，第三行是zeta方向导数）

**异常:**
- `std::invalid_argument` - 当阶次大于1时抛出

## 示例用法

```cpp
// 计算线单元形函数
Eigen::VectorXd N_line;
FEM::Utils::ShapeFunctions::getLineShapeFunctions(1, 0.5, N_line);
std::cout << "Line shape functions: " << N_line.transpose() << std::endl;

// 计算四边形单元形函数
Eigen::VectorXd N_quad;
FEM::Utils::ShapeFunctions::getQuadShapeFunctions(1, 0.5, 0.5, N_quad);
std::cout << "Quad shape functions: " << N_quad.transpose() << std::endl;

// 计算六面体单元形函数导数
Eigen::MatrixXd dN_dxi_hex;
FEM::Utils::ShapeFunctions::getHexShapeFunctionDerivatives(1, 0.5, 0.5, 0.5, dN_dxi_hex);
std::cout << "Hex shape function derivatives:\n" << dN_dxi_hex << std::endl;
```

## 实现细节

目前所有单元类型仅支持1阶形函数：

1. **线单元（Line Element）**：
   - 2个节点
   - 形函数：N1 = (1-ξ)/2, N2 = (1+ξ)/2

2. **四边形单元（Quadrilateral Element）**：
   - 4个节点
   - 双线性形函数

3. **六面体单元（Hexahedron Element）**：
   - 8个节点
   - 三线性形函数

## 依赖关系

- Eigen - 向量和矩阵运算库
- STL - 异常处理