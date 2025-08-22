# HeatTransfer 类

## 描述

`HeatTransfer` 类表示热传导物理问题，负责管理热传导分析的内核并执行全局矩阵组装。该类使用模板设计，支持不同维度的热传导问题。通过 [IKernel](../../kernels/classes/KernelWrappers.md) 抽象接口和 [KernelWrapper](../../kernels/classes/KernelWrappers.md) 包装器，可以支持不同节点数的单元类型。

与之前版本相比，该类现在实现了单元类型过滤机制，确保只有适当类型的单元参与域内组装。

## 类定义

```cpp
template<int TDim, typename TScalar = double>
class HeatTransfer : public PhysicsField<TDim, TScalar>
```

**模板参数:**
- `TDim` - 问题的空间维度
- `TScalar` - 标量类型，默认为 `double`，也可支持 `std::complex<double>` 等类型

## 成员函数

### template<typename KernelType> void addKernel(std::unique_ptr<KernelType> kernel)

添加内核到热传导问题中。

**模板参数:**
- `KernelType` - 内核类型

**参数:**
- `kernel` - 内核对象的智能指针

### void assemble_volume(const Mesh& mesh, const DofManager& dof_manager, Eigen::SparseMatrix<TScalar>& K_global, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global)

组装全局刚度矩阵和载荷向量。

**参数:**
- `mesh` - 网格对象的常量引用
- `dof_manager` - 自由度管理器的常量引用
- `K_global` - 全局刚度矩阵的引用
- `F_global` - 全局载荷向量的引用

与之前版本相比，该方法现在会过滤单元类型，只对适当类型的单元进行组装：
- 一维问题：只组装线单元
- 二维问题：只组装三角形和四边形单元
- 三维问题：只组装四面体和六面体单元

### bool shouldAssembleElement(const Element& element, int problem_dim) const

判断单元是否应该参与组装。

**参数:**
- `element` - 单元对象的常量引用
- `problem_dim` - 问题维度

**返回值:**
- 布尔值，指示单元是否应该参与组装

### std::string getName() const

获取物理场名称。

**返回值:**
- 字符串 "HeatTransfer"

## 实现细节

与之前版本相比，该类的主要变化包括：

1. 实现了单元类型过滤机制，确保只有适当类型的单元参与域内组装
2. 添加了 `shouldAssembleElement` 方法，用于判断单元是否应该参与组装
3. 移除了硬编码的节点数2

这些改进使得物理场可以处理混合网格，即同时包含不同类型和节点数的单元，并确保只有适当类型的单元参与计算。

## 示例用法

```cpp
// 创建热传导问题对象
auto heat_transfer = std::make_unique<FEM::HeatTransfer<2>>();
```

## 依赖关系

- [PhysicsField](PhysicsField.md) - 基类
- [Kernel](../../kernels/classes/Kernel.md) - 内核基类
- [IKernel](../../kernels/classes/KernelWrappers.md) - 内核接口
- [KernelWrapper](../../kernels/classes/KernelWrappers.md) - 内核包装器
- [Mesh](../../mesh/classes/Mesh.md) - 网格
- [DofManager](../../core/classes/DofManager.md) - 自由度管理器