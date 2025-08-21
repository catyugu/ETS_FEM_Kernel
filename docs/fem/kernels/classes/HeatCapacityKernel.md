# HeatCapacityKernel 类

## 描述

`HeatCapacityKernel` 类是频域热传导问题的物理内核实现，继承自 [Kernel](Kernel.md) 基类。该类负责计算热传导问题中与热容相关的单元矩阵，用于频域分析。

## 类定义

```cpp
template<int TDim, int TNumNodes_, typename TScalar = double>
class HeatCapacityKernel : public Kernel<TDim, TNumNodes_, TScalar>
```

**模板参数:**
- `TDim` - 问题的空间维度
- `TNumNodes_` - 单元节点数量
- `TScalar` - 标量类型，默认为double，也可支持std::complex<double>等类型

## 构造函数

### HeatCapacityKernel(const Material& material, double omega)

构造函数，使用给定的材料属性和角频率初始化热容内核。

**参数:**
- `material` - 材料属性对象的常量引用
- `omega` - 角频率

## 成员函数

### Eigen::Matrix<TScalar, TNumNodes_, TNumNodes_> compute_element_matrix(const Element& element) override

计算热容问题的单元质量矩阵。

**参数:**
- `element` - 要计算单元矩阵的单元对象

**返回值:**
- 热容问题的单元质量矩阵，大小为 [TNumNodes_](file:///E:/code/cpp/ETS_FEM_Kernel/fem/kernels/HeatCapacityKernel.hpp#L9) x [TNumNodes_](file:///E:/code/cpp/ETS_FEM_Kernel/fem/kernels/HeatCapacityKernel.hpp#L9)

## 实现细节

该内核计算频域热传导问题中的质量矩阵项，其形式为：

M_e = j * ω * ρ * c * ∫(N^T * N) dV

其中：
- j 是虚数单位
- ω 是角频率
- ρ 是密度
- c 是比热容
- N 是形函数矩阵

## 示例用法

```cpp
// 创建材料属性
FEM::Material material;
material.setProperty("density", 1.0);
material.setProperty("specific_heat", 1.0);

// 创建热容内核
FEM::HeatCapacityKernel<2, 3, std::complex<double>> kernel(material, 2.0 * M_PI);

// 假设有一个单元对象
FEM::Element& element = ...;
```