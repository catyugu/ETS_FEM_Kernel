# HeatDiffusionKernel 类

## 描述

`HeatDiffusionKernel` 类是热传导问题的物理内核实现，继承自 [Kernel](Kernel.md) 基类。该类负责计算热传导问题的单元刚度矩阵，是有限元热分析的核心计算组件。

## 类定义

```cpp
template<int TDim, int TNumNodes>
class HeatDiffusionKernel : public Kernel<TDim, TNumNodes>
```

**模板参数:**
- `TDim` - 问题的空间维度
- `TNumNodes` - 单元节点数量

## 构造函数

### explicit HeatDiffusionKernel(const Material& material)

构造函数，使用给定的材料属性初始化热传导内核。

**参数:**
- `material` - 材料属性对象的常量引用

## 成员函数

### Eigen::Matrix<double, TNumNodes, TNumNodes> compute_element_matrix(const Element& element) override

计算热传导问题的单元刚度矩阵。

**参数:**
- `element` - 要计算单元矩阵的单元对象

**返回值:**
- 热传导问题的单元刚度矩阵，大小为 [TNumNodes](file:///E:/code/cpp/ETS_FEM_Kernel/fem/kernels/HeatDiffusionKernel.hpp#L17) x [TNumNodes](file:///E:/code/cpp/ETS_FEM_Kernel/fem/kernels/HeatDiffusionKernel.hpp#L17)

## 示例用法

```cpp
// 创建材料属性
FEM::Material material;

// 创建热传导内核
FEM::HeatDiffusionKernel<2, 3> kernel(material);

// 假设有一个单元对象
FEM::Element& element = ...;

// 计算单元刚度矩阵
auto element_matrix = kernel.compute_element_matrix(element);
```

## 实现细节

`HeatDiffusionKernel` 类实现了热传导问题的有限元计算。其核心计算基于以下公式：

K_elem += B^T * D * B * dV

其中：
- B 是梯度矩阵（应变-位移矩阵或标量问题的梯度算子）
- D 是材料属性矩阵（对于热传导问题，是一个标量，即热导率）
- dV 是体积微元（通过雅可比行列式和积分权重计算）

该实现使用 [FEValues](../../core/classes/FEValues.md) 类来计算形函数梯度和几何信息，并从材料属性中获取热导率。

## 依赖关系

- [Kernel](Kernel.md) - 基类
- [Material](../../materials/classes/Material.md) - 材料属性
- [FEValues](../../core/classes/FEValues.md) - 有限元值计算
- [Element](../../mesh/classes/Element.md) - 单元数据结构
- Eigen - 矩阵运算库