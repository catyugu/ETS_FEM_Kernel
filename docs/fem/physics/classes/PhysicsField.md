# PhysicsField 类

## 描述

`PhysicsField` 是所有物理场问题的抽象基类。它定义了物理场问题必须实现的接口，包括组装全局矩阵和向量、应用边界条件等方法。通过继承这个基类，可以实现各种具体的物理场问题，如热传导、静电场等。

## 类签名

```cpp
template<int TDim>
class PhysicsField {
public:
    virtual ~PhysicsField() = default;
    
    virtual void assemble(const Mesh& mesh, const DofManager& dof_manager,
                         Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) = 0;
                         
    virtual void applyBoundaryConditions(const Mesh& mesh, const DofManager& dof_manager,
                                        Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global);
                                        
    virtual std::string getName() const = 0;
};
```

## 方法说明

### assemble

```cpp
virtual void assemble(const Mesh& mesh, const DofManager& dof_manager,
                     Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) = 0;
```

**描述**: 纯虚函数，由派生类实现。负责组装全局刚度矩阵和载荷向量。

**参数**:
- `mesh` - 网格对象
- `dof_manager` - 自由度管理器
- `K_global` - 全局刚度矩阵（输出）
- `F_global` - 全局载荷向量（输出）

### applyBoundaryConditions

```cpp
virtual void applyBoundaryConditions(const Mesh& mesh, const DofManager& dof_manager,
                                    Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global);
```

**描述**: 应用边界条件。默认实现为空，具体物理场可以重写此方法。

**参数**:
- `mesh` - 网格对象
- `dof_manager` - 自由度管理器
- `K_global` - 全局刚度矩阵
- `F_global` - 全局载荷向量

### getName

```cpp
virtual std::string getName() const = 0;
```

**描述**: 纯虚函数，由派生类实现。返回物理场的名称。

**返回值**: 物理场名称的字符串

## 示例用法

```cpp
// 创建具体的物理场实例
auto physics = std::make_unique<FEM::HeatTransfer<2>>();
// 或
auto physics = std::make_unique<FEM::Electrostatics<2>>();

// 在Problem类中使用
auto problem = std::make_unique<FEM::Problem<2>>(std::move(mesh), std::move(physics));
```

## 注意事项

1. 所有具体的物理场类都必须继承自`PhysicsField`并实现所有纯虚函数
2. `assemble`方法是有限元计算的核心，负责将局部单元矩阵组装成全局矩阵
3. `getName`方法用于标识物理场类型，便于调试和日志记录

## 依赖关系

- [Mesh](../../mesh/classes/Mesh.md) - 网格类
- [DofManager](../../core/classes/DofManager.md) - 自由度管理器
- Eigen - 稀疏矩阵运算库