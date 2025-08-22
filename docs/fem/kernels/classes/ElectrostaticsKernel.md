# ElectrostaticsKernel 类

## 描述

`ElectrostaticsKernel` 类实现了静电场问题的单元计算逻辑。它继承自 [Kernel](Kernel.md) 基类，用于计算二维或三维静电场问题中单个单元的局部刚度矩阵。该内核使用电导率材料属性来计算电势分布。

## 类签名

```cpp
template<int TDim, int TNumNodes, typename TScalar = double>
class ElectrostaticsKernel : public Kernel<TDim, TNumNodes, TScalar> {
public:
    explicit ElectrostaticsKernel(const Material& material);
    
    Eigen::Matrix<TScalar, TNumNodes, TNumNodes>
    compute_element_matrix(const Element& element) override;
};
```

**模板参数:**
- `TDim` - 问题的空间维度
- `TNumNodes` - 单元节点数量
- `TScalar` - 标量类型，默认为double，也可支持std::complex<double>等类型

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
Eigen::Matrix<TScalar, TNumNodes, TNumNodes>
compute_element_matrix(const Element& element) override;
```

**描述**: 计算单元的局部刚度矩阵。该方法使用有限元方法计算静电场问题的局部矩阵，基于电导率材料属性。

**参数**:
- `element` - 要计算的单元对象

**返回值**: 单元局部刚度矩阵

**计算公式**: 
K_e = ∫(B^T * D * B) dV

其中 B 是梯度算子矩阵，D 是材料属性矩阵。

## 示例用法

```cpp
#include "fem/kernels/ElectrostaticsKernel.hpp"

// 创建材料并设置电导率
FEM::Material copper("Copper");
copper.setProperty("electrical_conductivity", 5.96e7); // S/m

// 创建静电场内核
auto electrostatics_kernel = std::make_unique<FEM::ElectrostaticsKernel<3>>(copper);

// 将内核添加到问题中
problem.addKernel(std::move(electrostatics_kernel));
```

## 注意事项

1. 该类使用模板参数 `TDim` 表示问题的维度
2. 材料必须包含"electrical_conductivity"属性，单位为西门子每米(S/m)
3. 该内核适用于标量静电场问题
4. 计算假设电导率在单元内为常数
5. 使用 [FEValues](../../core/classes/FEValues.md) 类的现代迭代器接口进行有限元值计算

## 依赖关系

- [Kernel](Kernel.md) - 基类
- [Material](../../materials/classes/Material.md) - 材料属性
- [FEValues](../../core/classes/FEValues.md) - 有限元值计算
- [Element](../../mesh/classes/Element.md) - 单元数据结构
- Eigen - 矩阵运算库