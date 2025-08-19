# Electrostatics 类

## 描述

`Electrostatics` 类实现了静电场问题的有限元求解。它继承自 [PhysicsField](PhysicsField.md) 抽象基类，用于计算在给定边界条件下的电势分布。该类使用内核机制来处理不同类型的单元和计算任务。

## 类签名

```cpp
template<int TDim>
class Electrostatics : public PhysicsField<TDim> {
public:
    template<typename KernelType>
    void addKernel(std::unique_ptr<KernelType> kernel);
    
    void assemble(const Mesh& mesh, const DofManager& dof_manager,
                  Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) override;
                  
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
    std::make_unique<FEM::ElectrostaticsKernel<2, 4>>(material)
);
```

### assemble

```cpp
void assemble(const Mesh& mesh, const DofManager& dof_manager,
              Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) override;
```

**描述**: 实现基类的纯虚函数，负责组装静电场问题的全局刚度矩阵和载荷向量。

**参数**:
- `mesh` - 网格对象
- `dof_manager` - 自由度管理器
- `K_global` - 全局刚度矩阵（输出）
- `F_global` - 全局载荷向量（输出）

### getName

```cpp
std::string getName() const override;
```

**描述**: 返回物理场的名称。

**返回值**: 字符串 "Electrostatics"

## 示例用法

```cpp
#include "fem/physics/Electrostatics.hpp"
#include "fem/kernels/ElectrostaticsKernel.hpp"

// 创建静电场物理问题
auto electrostatics_physics = std::make_unique<FEM::Electrostatics<2>>();

// 添加计算内核
electrostatics_physics->addKernel(
    std::make_unique<FEM::ElectrostaticsKernel<2, 4>>(material)
);

// 创建问题实例
auto problem = std::make_unique<FEM::Problem<2>>(std::move(mesh), std::move(electrostatics_physics));

// 设置边界条件
problem->addDirichletBC(0, 10.0);  // 10V
problem->addDirichletBC(10, 0.0);  // 0V (地)

// 求解
problem->assemble();
problem->applyBCs();
problem->solve();
```

## 注意事项

1. 该类使用模板参数 `TDim` 表示问题的维度（1D、2D或3D）
2. 使用 [KernelWrapper](../../kernels/classes/KernelWrappers.md) 机制来管理不同类型的内核
3. 需要与 [ElectrostaticsKernel](../../kernels/classes/ElectrostaticsKernel.md) 配合使用以获得正确的物理计算
4. 电势的单位是伏特(V)