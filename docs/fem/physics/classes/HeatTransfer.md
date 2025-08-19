# HeatTransfer 类

## 描述

`HeatTransfer` 类表示热传导物理问题，负责管理热传导分析的内核并执行全局矩阵组装。该类使用模板设计，支持不同维度的热传导问题。通过 [IKernel](file:///E:/code/cpp/ETS_FEM_Kernel/fem/physics/HeatTransfer.hpp#L14-L19) 抽象接口和 [KernelWrapper](file:///E:/code/cpp/ETS_FEM_Kernel/fem/physics/HeatTransfer.hpp#L22-L41) 包装器，可以支持不同节点数的单元类型。

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

### void assemble(const Mesh& mesh, const DofManager& dof_manager, Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& /*F_global*/)

组装全局刚度矩阵。

**参数:**
- `mesh` - 网格对象的常量引用
- `dof_manager` - 自由度管理器的常量引用
- `K_global` - 全局刚度矩阵的引用
- `F_global` - 全局载荷向量的引用（目前未使用）

## 相关类

### IKernel 类

热传导内核的抽象接口类。

```cpp
template<int TDim>
class IKernel
```

**成员函数:**
- `virtual ~IKernel() = default` - 虚析构函数
- `virtual void assemble_element(const Element& element, Eigen::SparseMatrix<double>& K_global, const DofManager& dof_manager) = 0` - 纯虚函数，组装单元矩阵

### KernelWrapper 类

内核包装器类，实现 [IKernel](file:///E:/code/cpp/ETS_FEM_Kernel/fem/physics/HeatTransfer.hpp#L14-L19) 接口。

```cpp
template<int TDim, int TNumNodes>
class KernelWrapper : public IKernel<TDim>
```

**模板参数:**
- `TDim` - 问题的空间维度
- `TNumNodes` - 单元节点数量

**成员函数:**
- `explicit KernelWrapper(std::unique_ptr<Kernel<TDim, TNumNodes>> kernel)` - 构造函数
- `void assemble_element(const Element& element, Eigen::SparseMatrix<double>& K_global, const DofManager& dof_manager) override` - 实现接口的单元矩阵组装方法

## 示例用法

```cpp
// 创建热传导问题对象
FEM::HeatTransfer<2> heat_transfer;

// 创建并添加热传导内核
auto kernel = std::make_unique<FEM::HeatDiffusionKernel<2, 3>>(material);
heat_transfer.addKernel(std::move(kernel));

// 组装全局矩阵
Eigen::SparseMatrix<double> K_global;
Eigen::VectorXd F_global;
heat_transfer.assemble(mesh, dof_manager, K_global, F_global);
```

## 实现细节

`HeatTransfer` 类通过抽象接口和模板包装器的设计模式解决了不同单元类型支持的问题。[IKernel](file:///E:/code/cpp/ETS_FEM_Kernel/fem/physics/HeatTransfer.hpp#L14-L19) 抽象接口隐藏了单元节点数量的模板参数，而 [KernelWrapper](file:///E:/code/cpp/ETS_FEM_Kernel/fem/physics/HeatTransfer.hpp#L22-L41) 模板类则将具体的内核实现包装成统一接口。

组装过程中，遍历所有单元和内核，对于节点数匹配的单元-内核组合执行单元矩阵计算和全局矩阵组装。

## 依赖关系

- [Kernel](file:///E:/code/cpp/ETS_FEM_Kernel/fem/kernels/Kernel.hpp#L15-L26) - 内核基类
- [Mesh](file:///E:/code/cpp/ETS_FEM_Kernel/fem/mesh/Mesh.hpp#L17-L45) - 网格类
- [DofManager](file:///E:/code/cpp/ETS_FEM_Kernel/fem/core/DofManager.hpp#L19-L61) - 自由度管理器
- Eigen - 稀疏矩阵运算库