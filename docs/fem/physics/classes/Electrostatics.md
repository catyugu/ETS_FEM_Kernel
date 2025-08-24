# Electrostatics 类

## 描述

`Electrostatics` 类实现了静电场问题的有限元求解。它继承自 [PhysicsField](PhysicsField.md) 抽象基类，用于计算在给定边界条件下的电势分布。该类使用内核机制来处理不同类型的单元和计算任务。

与之前版本相比，该类现在实现了单元类型过滤机制，确保只有适当类型的单元参与域内组装，并支持多物理场功能。

## 类签名

```cpp
template<int TDim, typename TScalar = double>
class Electrostatics : public PhysicsField<TDim, TScalar> {
public:
    Electrostatics();
    
    template<typename KernelType>
    void addKernel(std::unique_ptr<KernelType> kernel);
    
    const std::string& getVariableName() const override;
    
    void defineVariables(DofManager& dof_manager) const override;
    
    void assemble_volume(const Mesh& mesh, const DofManager& dof_manager,
                  std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) override;
                  
    bool shouldAssembleElement(const Element& element, int problem_dim) const;
    
    std::string getName() const override;
};
```

## 构造函数

### Electrostatics

```cpp
Electrostatics();
```

**描述**: 构造一个 Electrostatics 对象。构造函数初始化变量名称为 "Voltage"。

## 方法说明

### getVariableName

```cpp
const std::string& getVariableName() const override;
```

**描述**: 获取物理场变量名称。

**返回值**:
- 字符串 "Voltage" 的常量引用

### defineVariables

```cpp
void defineVariables(DofManager& dof_manager) const override;
```

**描述**: 在自由度管理器中定义电势变量。

**参数**:
- `dof_manager` - 自由度管理器引用

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
              std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) override;
```

**描述**: 组装全局刚度矩阵和载荷向量。与之前版本相比，该方法现在会过滤单元类型，只对适当类型的单元进行组装，并使用Triplet列表提高性能。

**参数**:
- `mesh` - 网格对象的常量引用
- `dof_manager` - 自由度管理器的常量引用
- `triplet_list` - 稀疏矩阵的Triplet列表（输出）
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
2. 添加了 `shouldAssembleElement` 方法，用于判断单元是否应该参与组装
3. 移除了硬编码的节点数
4. 实现了 `getVariableName` 和 `defineVariables` 方法以支持多物理场
5. 使用Triplet列表而不是直接操作稀疏矩阵以提高性能

## 示例用法

```cpp
#include "fem/physics/Electrostatics.hpp"

// 创建静电场物理实例
auto electrostatics = std::make_unique<FEM::Electrostatics<2>>();

// 添加材料和内核
auto material = std::make_unique<FEM::Material>();
material->setProperty("permittivity", 8.854e-12); // 真空介电常数

electrostatics->addKernel(
    std::make_unique<FEM::ElectrostaticsKernel<2>>(std::move(material))
);

// 添加边界条件
electrostatics->addBoundaryCondition(
    std::make_unique<FEM::DirichletBC<2>>("Voltage", "electrode_1", 10.0)
);
electrostatics->addBoundaryCondition(
    std::make_unique<FEM::DirichletBC<2>>("Voltage", "electrode_2", 0.0)
);

// 创建问题并求解
auto geometry = FEM::Mesh::create_uniform_2d_mesh(1.0, 1.0, 10, 10);
auto problem = std::make_unique<FEM::Problem<2>>(std::move(geometry), std::move(electrostatics));
problem->assemble();
problem->applyBCs();
problem->solve();
```