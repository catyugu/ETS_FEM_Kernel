# FEFaceValues

面单元值计算类，用于在边界面上计算形函数值、导数和雅可比信息。

## 类签名

```cpp
class FEFaceValues
```

## 概述

`FEFaceValues`类专为边界面上的数值积分设计，提供形函数值、导数和雅可比信息的计算功能。它扩展了FEValues类的功能，专门处理边界条件计算中的面积分。

## 构造函数

```cpp
FEFaceValues(const Element& elem, int order, AnalysisType analysis_type)
```

### 参数

- `elem` - 面单元引用
- `order` - 积分阶次
- `analysis_type` - 分析类型

## 方法

### reinit

重新初始化指定积分点的计算数据。

```cpp
void reinit(int q_index)
```

### n_quad_points

获取积分点数量。

```cpp
size_t n_quad_points() const
```

### N

获取当前积分点的形函数值。

```cpp
const Eigen::VectorXd& N() const
```

### dN_dx

获取当前积分点的形函数导数。

```cpp
const Eigen::MatrixXd& dN_dx() const
```

### JxW

获取当前积分点的雅可比行列式与权重的乘积。

```cpp
double JxW() const
```

## 示例用法

```cpp
#include "fem/core/FEFaceValues.hpp"

// 在边界单元上创建FEFaceValues对象
FEFaceValues fe_face_values(face_element, 1, AnalysisType::SCALAR_DIFFUSION);

// 遍历所有积分点
for (size_t q = 0; q < fe_face_values.n_quad_points(); ++q) {
    fe_face_values.reinit(q);
    
    // 获取形函数值
    const auto& N = fe_face_values.N();
    
    // 计算积分贡献
    F_elem_bc += N * value_ * fe_face_values.JxW();
}
```

## 注意事项

- 该类专为边界条件计算设计
- 需要与边界单元配合使用
- 形函数和导数的计算考虑了边界单元的几何特性