# PhysicsField 类

## 描述

`PhysicsField` 是所有物理场问题的抽象基类。它定义了物理场问题必须实现的接口，包括组装全局矩阵和向量、应用边界条件等方法。通过继承这个基类，可以实现各种具体的物理场问题，如热传导、静电场等。

与之前的版本相比，该类现在支持多物理场，需要定义变量名称并管理自由度。

## 类签名

```cpp
template<int TDim, typename TScalar = double>
class PhysicsField {
public:
    virtual ~PhysicsField() = default;
    
    virtual const std::string& getVariableName() const = 0;
    
    virtual void defineVariables(DofManager& dof_manager) const = 0;
    
    virtual void assemble_volume(const Mesh& mesh, const DofManager& dof_manager,
                         std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) = 0;
                         
    virtual void applyNaturalBCs(const Geometry& geometry, const DofManager& dof_manager,
                                        std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global);
                                        
    virtual std::string getName() const = 0;
};
```

**模板参数:**
- `TDim` - 问题的空间维度
- `TScalar` - 标量类型，默认为double，也可支持std::complex<double>等类型

## 方法说明

### getVariableName

```cpp
virtual const std::string& getVariableName() const = 0;
```

**描述**: 纯虚函数，由派生类实现。返回物理场变量的名称。

**返回值**: 物理场变量名称的字符串常量引用

### defineVariables

```cpp
virtual void defineVariables(DofManager& dof_manager) const = 0;
```

**描述**: 纯虚函数，由派生类实现。负责在自由度管理器中定义物理场变量。

**参数**:
- `dof_manager` - 自由度管理器引用

### assemble_volume

```cpp
virtual void assemble_volume(const Mesh& mesh, const DofManager& dof_manager,
                     std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) = 0;
```

**描述**: 纯虚函数，由派生类实现。负责组装全局刚度矩阵和载荷向量。与之前版本不同，现在使用Triplet列表而不是直接操作稀疏矩阵，以提高组装效率。

**参数**:
- `mesh` - 网格对象
- `dof_manager` - 自由度管理器
- `triplet_list` - 用于存储矩阵元素的Triplet列表（输出）
- `F_global` - 全局载荷向量（输出）

### applyNaturalBCs

```cpp
virtual void applyNaturalBCs(const Geometry& geometry, const DofManager& dof_manager,
                                    std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global);
```

**描述**: 应用自然边界条件（Neumann和Cauchy边界条件）。默认实现为空，具体物理场可以重写此方法。与之前版本不同，现在使用Triplet列表而不是直接操作稀疏矩阵，以提高组装效率。

**参数**:
- `geometry` - 几何对象（包含网格和边界定义）
- `dof_manager` - 自由度管理器
- `triplet_list` - 用于存储矩阵元素的Triplet列表
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
auto geometry = FEM::Mesh::create_uniform_1d_mesh(1.0, 10);
auto problem = std::make_unique<FEM::Problem<2>>(std::move(geometry), std::move(physics));
```

## 实现细节

与之前的版本相比，`PhysicsField` 现在需要实现两个新的纯虚函数：

1. `getVariableName` - 返回物理场变量的名称，用于在自由度管理器中查找正确的自由度索引
2. `defineVariables` - 在自由度管理器中定义物理场变量

这些变化支持了多物理场功能，每个物理场都需要明确声明其变量名称和自由度需求。

## 注意事项

1. 所有具体的物理场类都必须继承自`PhysicsField`并实现所有纯虚函数
2. `assemble`方法是有限元计算的核心，负责将局部单元矩阵组装成全局矩阵
3. `getVariableName`方法用于标识物理场变量类型，便于在自由度管理器中查找
4. 为了提高性能，现在使用Triplet列表而不是直接操作稀疏矩阵
5. `applyNaturalBCs`方法现在接受Geometry对象而不是Mesh对象
6. 物理场类必须在`defineVariables`方法中调用`dof_manager.addVariable()`来注册其变量

## 依赖关系

- [Mesh](../../mesh/classes/Mesh.md) - 网格类
- [Geometry](../../mesh/classes/Geometry.md) - 几何类
- [DofManager](../../core/classes/DofManager.md) - 自由度管理器
- Eigen - 稀疏矩阵运算库