# AnalysisType 枚举

## 描述

`AnalysisType` 枚举定义了有限元内核所支持的分析类型。这个枚举用于指导 [FEValues](FEValues.md) 类构建正确的 B 矩阵，以适应不同类型的物理问题。

## 枚举定义

```cpp
enum class AnalysisType {
    SCALAR_DIFFUSION,   // 标量扩散 (热传导、静电势)
    VECTOR_CURL,        // 矢量旋度 (磁场)
    // 未来可以添加 STRUCTURAL, FLUIDS, etc.
};
```

## 枚举值说明

### SCALAR_DIFFUSION

标量扩散类型，适用于以下物理问题：
- 热传导问题
- 静电势问题

对于这类问题，B 矩阵是形函数的梯度矩阵。

### VECTOR_CURL

矢量旋度类型，适用于以下物理问题：
- 磁场问题

对于这类问题，B 矩阵是基于旋度算子构建的。

## 使用示例

```cpp
// 在 FEValues 构造函数中使用
FEValues fe_values(element, 2, AnalysisType::SCALAR_DIFFUSION);

// 在 ElectrostaticsKernel 中使用
FEValues fe_values(element, 1, AnalysisType::SCALAR_DIFFUSION);
```

## 实现细节

该枚举类型在 [FEValues](FEValues.md) 类中被使用，以根据不同的物理问题类型构建相应的 B 矩阵。B 矩阵是有限元计算中的关键组成部分，它将形函数的导数与物理问题的本构关系相结合。

## 依赖关系

- [FEValues](FEValues.md) - 使用此枚举来构建 B 矩阵