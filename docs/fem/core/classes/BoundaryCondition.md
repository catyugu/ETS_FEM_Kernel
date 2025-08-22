# BoundaryCondition

边界条件抽象基类，为所有具体边界条件类型提供统一接口。

## 类签名

```cpp
template<int TDim, typename TScalar = double>
class BoundaryCondition
```

**模板参数:**
- `TDim` - 问题的空间维度
- `TScalar` - 标量类型，默认为double，也可支持std::complex<double>等类型

## 概述

`BoundaryCondition`是所有边界条件类型的抽象基类。它定义了边界条件的基本接口，包括应用边界条件的方法和获取边界条件类型的方法。

## 构造函数

```cpp
protected:
    BoundaryCondition(const std::string& boundary_name)
```

### 参数

- `boundary_name` - 边界名称，用于标识网格中的特定边界

## 方法

### apply

应用边界条件到全局系统矩阵和载荷向量。与之前版本不同，现在使用Triplet列表而不是直接操作稀疏矩阵，以提高组装效率。

```cpp
virtual void apply(const Geometry& geometry, const DofManager& dof_manager,
                   std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const = 0;
```

### getType

获取边界条件类型。

```cpp
virtual BCType getType() const = 0;
```

### getBoundaryName

获取边界名称。

```cpp
const std::string& getBoundaryName() const
```

## 示例用法

由于BoundaryCondition是抽象类，不能直接实例化。应使用具体的边界条件实现类：

```cpp
// 使用DirichletBC
auto bc = std::make_unique<DirichletBC<3>>("inlet", 100.0);

// 添加到物理场
physics->addBoundaryCondition(std::move(bc));
```

## 注意事项

- 所有具体的边界条件类都必须继承自BoundaryCondition
- apply方法的实现应根据边界条件类型修改全局矩阵和向量
- 为了提高性能，现在使用Triplet列表而不是直接操作稀疏矩阵
- Dirichlet边界条件在apply方法中通常为空实现，因为它们由Problem类统一处理
- apply方法现在接受Geometry对象而不是Mesh对象