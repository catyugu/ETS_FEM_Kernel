# Electrostatics 类

## 描述

`Electrostatics` 类实现了静电场问题的有限元求解。它继承自 [PhysicsField](PhysicsField.md) 抽象基类，用于计算在给定边界条件下的电势分布。该类使用内核机制来处理不同类型的单元和计算任务。

与之前版本相比，该类现在实现了单元类型过滤机制，确保只有适当类型的单元参与域内组装。

## 类签名

```cpp
template<int TDim, typename TScalar = double>
class Electrostatics : public PhysicsField<TDim, TScalar> {
public:
    template<typename KernelType>
    void addKernel(std::unique_ptr<KernelType> kernel);
    
    void assemble_volume(const Mesh& mesh, const DofManager& dof_manager,
                  Eigen::SparseMatrix<TScalar>& K_global, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) override;
                  
    bool shouldAssembleElement(const Element& element, int problem_dim) const;
    
    std::string getName() const override;
};
```

## 方法说明

### addKernel

```cpp
template<typename KernelType>
void addKernel(std::unique_ptr<KernelType> kernel);
```

**描述**: 添加一个计算内核到物理场中。内核负责实现特定单元类型的计算逻辑。

**参数**:
- `kernel` - 指向内核对象的智能指针

**示例**:
```cpp
auto electrostatics = std::make_unique<FEM::Electrostatics<2>>();
electrostatics->addKernel(
    std::make_unique<FEM::ElectrostaticsKernel<2>>(material)
);
```

### assemble_volume

```cpp
void assemble_volume(const Mesh& mesh, const DofManager& dof_manager,
              Eigen::SparseMatrix<TScalar>& K_global, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) override;
```

**描述**: 组装全局刚度矩阵和载荷向量。与之前版本相比，该方法现在会过滤单元类型，只对适当类型的单元进行组装。

**参数**:
- `mesh` - 网格对象的常量引用
- `dof_manager` - 自由度管理器的常量引用
- `K_global` - 全局刚度矩阵的引用
- `F_global` - 全局载荷向量的引用

与之前版本相比，该方法现在会过滤单元类型，只对适当类型的单元进行组装：
- 一维问题：只组装线单元
- 二维问题：只组装三角形和四边形单元
- 三维问题：只组装四面体和六面体单元

### shouldAssembleElement

```cpp
bool shouldAssembleElement(const Element& element, int problem_dim) const;
```

**描述**: 判断单元是否应该参与组装。

**参数**:
- `element` - 单元对象的常量引用
- `problem_dim` - 问题维度

**返回值**:
- 布尔值，指示单元是否应该参与组装

### getName

```cpp
std::string getName() const override;
```

**描述**: 获取物理场名称。

**返回值**:
- 字符串 "Electrostatics"

## 实现细节

与之前版本相比，该类的主要变化包括：

1. 实现了单元类型过滤机制，确保只有适当类型的单元参与域内组装
2. 添加了 [shouldAssembleElement](file:///E:/code/cpp/ETS_FEM_Kernel/fem/physics/Electrostatics.hpp#L58-L75) 方法，用于判断单元是否应该参与组装
3. 移除了硬编码的节点数

这些改进使得物理场可以处理混合网格，即同时包含不同类型和节点数的单元，并确保只有适当类型的单元参与计算。

## 依赖关系

- [PhysicsField](PhysicsField.md) - 基类
- [Kernel](../../kernels/classes/Kernel.md) - 内核基类
- [IKernel](../../kernels/classes/KernelWrappers.md) - 内核接口
- [KernelWrapper](../../kernels/classes/KernelWrappers.md) - 内核包装器
- [Mesh](../../mesh/classes/Mesh.md) - 网格
- [DofManager](../../core/classes/DofManager.md) - 自由度管理器