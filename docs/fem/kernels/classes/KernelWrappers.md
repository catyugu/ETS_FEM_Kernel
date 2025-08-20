# KernelWrappers 类

## 描述

`KernelWrappers` 文件包含内核抽象接口 [IKernel](#ikernel) 和模板化的内核包装器 [KernelWrapper](#kernelwrapper)。这些类提供了一种统一的方式来管理和调用不同类型的计算内核，使得物理场类可以支持多种单元类型而无需了解具体的内核实现细节。

## 类签名

### IKernel

```cpp
template<int TDim>
class IKernel {
public:
    virtual ~IKernel() = default;
    virtual void assemble_element(const Element& element, Eigen::SparseMatrix<double>& K_global, const DofManager& dof_manager) = 0;
};
```

### KernelWrapper

```cpp
template<int TDim, int TNumNodes>
class KernelWrapper : public IKernel<TDim> {
public:
    explicit KernelWrapper(std::unique_ptr<Kernel<TDim, TNumNodes>> kernel);
    
    void assemble_element(const Element& element, Eigen::SparseMatrix<double>& K_global, const DofManager& dof_manager) override;
};
```

## IKernel 类

### 描述

[IKernel](#ikernel) 是所有内核的抽象接口。它定义了一个纯虚函数 `assemble_element`，所有具体的内核包装器都必须实现该函数。

### 方法说明

#### assemble_element

```cpp
virtual void assemble_element(const Element& element, Eigen::SparseMatrix<double>& K_global, const DofManager& dof_manager) = 0;
```

**描述**: 纯虚函数，由派生类实现。负责将单个单元的局部矩阵组装到全局矩阵中。

**参数**:
- `element` - 要组装的单元
- `K_global` - 全局刚度矩阵（输出）
- `dof_manager` - 自由度管理器

## KernelWrapper 类

### 描述

[KernelWrapper](#kernelwrapper) 是一个模板化的内核包装器，它实现了 [IKernel](#ikernel) 接口。该包装器将具体的内核实例包装起来，提供统一的接口供物理场类使用。

### 构造函数

#### KernelWrapper

```cpp
explicit KernelWrapper(std::unique_ptr<Kernel<TDim, TNumNodes>> kernel);
```

**描述**: 构造一个内核包装器实例。

**参数**:
- `kernel` - 指向具体内核实例的智能指针

### 方法说明

#### assemble_element

```cpp
void assemble_element(const Element& element, Eigen::SparseMatrix<double>& K_global, const DofManager& dof_manager) override;
```

**描述**: 实现 [IKernel](#ikernel) 接口的函数。负责计算单元局部矩阵并将其组装到全局矩阵中。

**参数**:
- `element` - 要组装的单元
- `K_global` - 全局刚度矩阵（输出）
- `dof_manager` - 自由度管理器

**工作流程**:
1. 检查单元节点数是否与内核匹配
2. 调用内核的 `compute_element_matrix` 方法计算局部矩阵
3. 获取单元的自由度索引
4. 将局部矩阵的元素添加到全局矩阵的相应位置

## 示例用法

```cpp
// 在物理场类中使用内核包装器
template<typename KernelType>
void HeatTransfer::addKernel(std::unique_ptr<KernelType> kernel) {
    // 创建包装器，将派生类指针安全地转换为基类接口指针
    kernels_.push_back(std::make_unique<KernelWrapper<TDim, KernelType::NumNodes>>(std::move(kernel)));
}
```

## 设计模式

该实现使用了以下设计模式：

1. **适配器模式**: [KernelWrapper](#kernelwrapper) 将具体的内核实例适配为统一的 [IKernel](#ikernel) 接口
2. **工厂模式**: `addKernel` 方法充当工厂，创建适当的内核包装器
3. **多态性**: 通过 [IKernel](#ikernel) 接口，物理场类可以处理任何类型的内核

## 注意事项

1. [KernelWrapper](#kernelwrapper) 会自动检查单元节点数是否与内核匹配，不匹配的单元会被忽略
2. 该机制支持混合网格，即同一物理场可以使用多种不同类型的单元
3. 内核的所有权通过智能指针管理，确保内存安全

## 依赖关系

- [Kernel](Kernel.md) - 内核基类
- [Element](../../mesh/classes/Element.md) - 单元数据结构
- [DofManager](../../core/classes/DofManager.md) - 自由度管理器
- Eigen - 稀疏矩阵运算库