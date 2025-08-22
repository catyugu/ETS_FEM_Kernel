# HeatCapacityKernel 类

热容计算内核类，用于计算热传导问题的单元质量矩阵。

## 类签名

```cpp
template<int TDim, typename TScalar = double>
class HeatCapacityKernel : public Kernel<TDim, TScalar>
```

## 概述

`HeatCapacityKernel` 类实现热传导问题的热容计算内核。该类负责计算单元级别的质量矩阵，通过在单元上进行数值积分来获得热容项的贡献。在频域分析中，该矩阵还会乘以j*ω因子。

该类使用 [FEValues](../../core/classes/FEValues.md) 类来计算单元上的形函数值和几何信息，并通过现代C++迭代器接口遍历积分点。

## 构造函数

```cpp
explicit HeatCapacityKernel(const Material& material, double omega = 0.0)
```

### 参数

- `material` - 包含密度和比热等属性的材料对象
- `omega` - 频率参数，用于频域分析（默认为0.0，表示时域分析）

## 方法

### compute_element_matrix

计算单元质量矩阵。

```cpp
MatrixType compute_element_matrix(const Element& element) override
```

**参数:**
- `element` - 当前计算的单元

**返回值:**
- 单元质量矩阵

## 示例用法

```cpp
#include "fem/kernels/HeatCapacityKernel.hpp"

// 创建材料对象并设置密度和比热
FEM::Material material;
material.addProperty("density", 7800.0);        // kg/m^3 (钢的密度)
material.addProperty("specific_heat", 500.0);   // J/(kg·K) (钢的比热)

// 创建热容内核（时域分析）
auto heat_capacity_kernel = std::make_unique<FEM::HeatCapacityKernel<3, double>>(material);

// 或者创建用于频域分析的热容内核（频率为100 Hz）
auto frequency_heat_capacity_kernel = std::make_unique<FEM::HeatCapacityKernel<3, std::complex<double>>>(material, 2 * M_PI * 100);

// 将内核添加到问题中
problem.addKernel(std::move(heat_capacity_kernel));
```

## 实现细节

在 `compute_element_matrix` 方法中，该类执行以下步骤：

1. 创建 [FEValues](../../core/classes/FEValues.md) 对象以计算单元上的形函数值和几何信息
2. 使用范围for循环遍历所有积分点：
   - 获取形函数值（N向量）和雅可比信息
   - 获取材料属性（密度和比热）
   - 计算局部质量矩阵贡献：C_elem += factor * N * N^T * JxW
3. 返回累积的单元质量矩阵

数学上，热容问题的弱形式导致以下单元矩阵计算：
```
C_elem = ∫N^T * ρ * cp * N dV
```

其中 N 是形函数，ρ 是密度，cp 是比热。

在频域分析中，该矩阵会乘以j*ω因子：
```
C_elem = j * ω * ∫N^T * ρ * cp * N dV
```

## 注意事项

- 该类使用 [FEValues](../../core/classes/FEValues.md) 的现代迭代器接口，避免了手动管理积分点状态
- 材料对象必须包含"density"和"specific_heat"属性
- 该内核适用于标量热传导问题
- 在频域分析中，TScalar模板参数应为std::complex<double>或类似复数类型