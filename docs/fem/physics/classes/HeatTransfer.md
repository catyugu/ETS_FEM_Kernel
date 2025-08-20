# HeatTransfer 类

## 描述

`HeatTransfer` 类表示热传导物理问题，负责管理热传导分析的内核并执行全局矩阵组装。该类使用模板设计，支持不同维度的热传导问题。通过 [IKernel](../../kernels/classes/KernelWrappers.md) 抽象接口和 [KernelWrapper](../../kernels/classes/KernelWrappers.md) 包装器，可以支持不同节点数的单元类型。

## 类定义

```cpp
template<int TDim>
class HeatTransfer
```

**模板参数:**
- `TDim` - 问题的空间维度

## 成员函数

### template<typename KernelType> void addKernel(std::unique_ptr<KernelType> kernel)

添加内核到热传导问题中。

**模板参数:**
- `KernelType` - 内核类型

**参数:**
- `kernel` - 内核对象的智能指针

### void assemble(const Mesh& mesh, const DofManager& dof_manager, Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global)

组装全局刚度矩阵和载荷向量。

**参数:**
- `mesh` - 网格对象的常量引用
- `dof_manager` - 自由度管理器的常量引用
- `K_global` - 全局刚度矩阵的引用
- `F_global` - 全局载荷向量的引用

### std::string getName() const

获取物理场名称。

**返回值:**
- 字符串 "HeatTransfer"

## 示例用法

```cpp
// 创建热传导问题对象
auto heat_transfer = std::make_unique<FEM::HeatTransfer<2>>();

// 创建并添加热传导内核
auto kernel = std::make_unique<FEM::HeatDiffusionKernel<2, 3>>(material);
heat_transfer->addKernel(std::move(kernel));

// 组装全局矩阵
auto mesh = std::make_unique<FEM::Mesh>();
FEM::DofManager dof_manager(*mesh);
dof_manager.buildDofMap(1);

Eigen::SparseMatrix<double> K_global;
Eigen::VectorXd F_global;
heat_transfer->assemble(*mesh, dof_manager, K_global, F_global);
```

## 实现细节

`HeatTransfer` 类通过抽象接口和模板包装器的设计模式解决了不同单元类型支持的问题。[IKernel](../../kernels/classes/KernelWrappers.md) 抽象接口隐藏了单元节点数量的模板参数，而 [KernelWrapper](../../kernels/classes/KernelWrappers.md) 模板类则将具体的内核实现包装成统一接口。

组装过程中，遍历所有单元和内核，对于节点数匹配的单元-内核组合执行单元矩阵计算和全局矩阵组装。

## 依赖关系

- [Kernel](../../kernels/classes/Kernel.md) - 内核基类
- [Mesh](../../mesh/classes/Mesh.md) - 网格类
- [DofManager](../../core/classes/DofManager.md) - 自由度管理器
- Eigen - 稀疏矩阵运算库
