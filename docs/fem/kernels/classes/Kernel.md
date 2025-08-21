# Kernel 类

## 描述

`Kernel` 类是所有物理内核的抽象基类，为不同物理问题的有限元计算提供统一接口。它是一个模板类，定义了计算单元矩阵的接口方法，具体实现由派生类完成。

## 类定义

```cpp
template<int TDim, int TNumNodes_, typename TScalar = double>
class Kernel
```

**模板参数:**
- `TDim` - 问题的空间维度
- `TNumNodes_` - 单元节点数量
- `TScalar` - 标量类型，默认为double，也可支持std::complex<double>等类型

## 成员函数

### virtual ~Kernel() = default

虚析构函数，确保通过基类指针删除派生类对象时能正确调用派生类的析构函数。

### virtual Eigen::Matrix<TScalar, TNumNodes_, TNumNodes_> compute_element_matrix(const Element& element) = 0

纯虚函数，计算单元矩阵。具体实现由派生类提供。

**参数:**
- `element` - 要计算单元矩阵的单元对象

**返回值:**
- 单元矩阵，大小为 `TNumNodes_`x `TNumNodes_`

## 静态成员

### static constexpr int NumNodes

静态常量成员，表示单元节点数量。

## 实现细节

`Kernel` 类是典型的抽象基类，使用模板参数定义了单元矩阵的维度。通过纯虚函数 [compute_element_matrix](file:///E:/code/cpp/ETS_FEM_Kernel/fem/kernels/Kernel.hpp#L14-L14) 定义了接口，要求所有派生类必须实现该方法以提供特定物理问题的单元矩阵计算。

这种设计遵循了面向对象的开闭原则（对扩展开放，对修改封闭），可以方便地添加新的物理问题内核而无需修改现有代码。

## 依赖关系

- [Element](../../mesh/classes/Element.md) - 单元数据结构
- Eigen - 矩阵运算库