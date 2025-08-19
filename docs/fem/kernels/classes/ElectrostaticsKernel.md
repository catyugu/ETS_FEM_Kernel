# ElectrostaticsKernel 类

## 描述

`ElectrostaticsKernel` 类实现了静电场问题的单元计算逻辑。它继承自 [Kernel](Kernel.md) 基类，用于计算二维或三维静电场问题中单个单元的局部刚度矩阵。该内核使用电导率材料属性来计算电势分布。

## 类签名

```cpp
template<int TDim, int TNumNodes>
class ElectrostaticsKernel : public Kernel<TDim, TNumNodes> {
public:
    explicit ElectrostaticsKernel(const Material& material);
    
    Eigen::Matrix<double, TNumNodes, TNumNodes>
    compute_element_matrix(const Element& element) override;
};
```

## 构造函数

### ElectrostaticsKernel

```cpp
explicit ElectrostaticsKernel(const Material& material);
```

**描述**: 构造一个静电场内核实例。

**参数**:
- `material` - 材料对象，必须包含"electrical_conductivity"属性

**异常**: 如果材料不包含"electrical_conductivity"属性，将在计算时抛出异常。

## 方法说明

### compute_element_matrix

```cpp
Eigen::Matrix<double, TNumNodes, TNumNodes>
compute_element_matrix(const Element& element) override;
```

**描述**: 计算单元的局部刚度矩阵。该方法使用有限元方法计算静电场问题的局部矩阵，基于电导率材料属性。

**参数**:
- `element` - 要计算的单元对象

**返回值**: 单元局部刚度矩阵

**计算公式**: 
K_e = ∫(B^T * σ * B) dV

其中:
- B 是应变-位移矩阵（在标量问题中是形函数梯度）
- σ 是电导率
- 积分在单元域内进行

## 示例用法

```cpp
#include "fem/kernels/ElectrostaticsKernel.hpp"

// 创建材料并设置电导率
FEM::Material copper("Copper");
copper.setProperty("electrical_conductivity", 5.96e7); // S/m

// 创建静电场内核（2D，4节点单元）
auto kernel = std::make_unique<FEM::ElectrostaticsKernel<2, 4>>(copper);

// 在物理场中使用内核
auto electrostatics = std::make_unique<FEM::Electrostatics<2>>();
electrostatics->addKernel(std::move(kernel));
```

## 注意事项

1. 该类使用模板参数 `TDim` 表示问题的维度，`TNumNodes` 表示单元节点数
2. 材料必须包含"electrical_conductivity"属性，单位为西门子每米(S/m)
3. 该内核适用于标量静电场问题
4. 计算假设电导率在单元内为常数