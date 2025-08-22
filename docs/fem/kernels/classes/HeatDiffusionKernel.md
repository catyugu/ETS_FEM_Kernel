# HeatDiffusionKernel 类

## 描述

`HeatDiffusionKernel` 类是热传导问题的物理内核实现，继承自 [Kernel](Kernel.md) 基类。该类负责计算热传导问题的单元刚度矩阵，是有限元热分析的核心计算组件。

与之前版本相比，该类现在负责构建B矩阵（应变-位移矩阵或标量问题的梯度算子），而不是依赖 [FEValues](../../core/classes/FEValues.md) 类提供。同时，该类现在支持动态节点数的单元。

## 类定义

```cpp
template<int TDim, typename TScalar = double>
class HeatDiffusionKernel : public Kernel<TDim, TScalar>

**模板参数:**
- `TDim` - 问题的空间维度
- `TScalar` - 标量类型，默认为 `double`，也可支持 `std::complex<double>` 等类型

## 构造函数

### explicit HeatDiffusionKernel(const Material& material)

构造函数，使用给定的材料属性初始化热传导内核。

**参数:**
- `material` - 材料属性对象的常量引用

## 成员函数

### Eigen::Matrix<TScalar, Eigen::Dynamic, Eigen::Dynamic> compute_element_matrix(const Element& element) override

计算热传导问题的单元刚度矩阵。

**参数:**
- `element` - 要计算单元矩阵的单元对象

**返回值:**
- 热传导问题的单元刚度矩阵，大小为动态矩阵

## 示例用法

```cpp
#include "fem/kernels/HeatDiffusionKernel.hpp"

// 创建材料对象并设置热传导系数
FEM::Material material;
material.addProperty("thermal_conductivity", 25.0); // W/(m·K)

// 创建热传导内核
auto heat_kernel = std::make_unique<FEM::HeatDiffusionKernel<3, double>>(material);

// 将内核添加到问题中
problem.addKernel(std::move(heat_kernel));
```

## 实现细节

在 `compute_element_matrix` 方法中，该类执行以下步骤：

1. 创建 [FEValues](../../core/classes/FEValues.md) 对象以计算单元上的形函数梯度和几何信息
2. 使用范围for循环遍历所有积分点：
   - 获取形函数梯度（B矩阵）和雅可比信息
   - 获取材料属性（热传导系数）
   - 计算局部传导矩阵贡献：K_elem += B^T * D * B * JxW
3. 返回累积的单元传导矩阵

数学上，热传导问题的弱形式导致以下单元矩阵计算：
```
K_elem = ∫(∇N)^T * k * ∇N dV
```

其中 N 是形函数，k 是热传导系数，∇N 是形函数梯度（即B矩阵）。

## 注意事项

- 该类使用 [FEValues](../../core/classes/FEValues.md) 的现代迭代器接口，避免了手动管理积分点状态
- 材料对象必须包含"thermal_conductivity"属性
- 该内核适用于标量热传导问题

## 依赖关系

- [Kernel](Kernel.md) - 基类
- [Material](../../materials/classes/Material.md) - 材料属性
- [Element](../../mesh/classes/Element.md) - 单元数据结构
- [FEValues](../../core/classes/FEValues.md) - 有限元值计算
- Eigen - 矩阵运算库