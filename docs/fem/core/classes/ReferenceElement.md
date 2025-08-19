# ReferenceElement 类

## 描述

`ReferenceElement` 类是一个缓存管理器，负责预计算和缓存参考单元上的形函数值、导数以及积分点信息。它通过缓存机制避免重复计算，提高有限元计算的效率。

在有限元方法中，计算通常在参考单元上进行，然后通过坐标变换映射到实际单元。`ReferenceElement` 类负责提供这些参考单元上的预计算数据。

## 类定义

```cpp
class ReferenceElement
```

## 成员函数

### static const ReferenceElementData& get(ElementType type, int order)

获取指定类型和阶次单元的缓存数据。

**参数:**
- `type` - 单元类型（如线单元、四边形单元、六面体单元）
- `order` - 积分阶次

**返回值:**
- 指定单元类型和阶次的 [ReferenceElementData](file:///E:/code/cpp/ETS_FEM_Kernel/fem/core/ReferenceElement.hpp#L18-L23) 结构体的常量引用

如果指定的单元类型和阶次的数据尚未缓存，则会先进行预计算并缓存。

## 相关数据结构

### ReferenceElementData

预计算并缓存的数据结构体。

**成员:**
- `std::vector<Utils::QuadraturePoint> q_points` - 积分点信息
- `std::vector<Eigen::VectorXd> N_values` - 各积分点上的形函数值
- `std::vector<Eigen::MatrixXd> dN_dxi_values` - 各积分点上的形函数导数

## 实现细节

`ReferenceElement` 类使用静态函数和静态成员变量实现单例模式的缓存管理。它维护一个映射表，键为单元类型和积分阶次的组合，值为预计算的 [ReferenceElementData](file:///E:/code/cpp/ETS_FEM_Kernel/fem/core/ReferenceElement.hpp#L18-L23) 数据。

当请求特定类型和阶次的数据时，如果数据已在缓存中，则直接返回；否则，调用 [precompute](file:///E:/code/cpp/ETS_FEM_Kernel/fem/core/ReferenceElement.hpp#L33-L59) 函数进行预计算，并将结果存储在缓存中。

支持的单元类型包括：
- 线单元（ElementType::Line）
- 四边形单元（ElementType::Quadrilateral）
- 六面体单元（ElementType::Hexahedron）

## 依赖关系

- [ElementType](file:///E:/code/cpp/ETS_FEM_Kernel/fem/mesh/Element.hpp#L12-L16) - 单元类型枚举
- [Utils::Quadrature](file:///E:/code/cpp/ETS_FEM_Kernel/utils/Quadrature.hpp#L15-L57) - 积分规则
- [Utils::ShapeFunctions](file:///E:/code/cpp/ETS_FEM_Kernel/utils/ShapeFunctions.hpp#L15-L71) - 形函数计算
- Eigen - 向量和矩阵运算