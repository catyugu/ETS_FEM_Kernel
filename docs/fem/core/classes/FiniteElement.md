# FiniteElement 类

## 描述

`FiniteElement` 类是有限单元的抽象基类，封装了特定单元类型的形函数、积分规则和插值逻辑。它是连接单元几何与物理计算的桥梁，提供了一种模块化的方式来处理不同类型的有限单元。

该类的设计目的是提高代码的可扩展性和可维护性，使添加新的单元类型变得更加容易。

## 类定义

```cpp
class FiniteElement
```

## 成员函数

### virtual ~FiniteElement() = default

虚析构函数，确保通过基类指针删除派生类对象时能正确调用派生类的析构函数。

### virtual ElementType getType() const = 0

纯虚函数，获取单元类型。

**返回值:**
- 单元类型枚举值

### virtual int getNumNodes() const = 0

纯虚函数，获取单元节点数量。

**返回值:**
- 节点数量

### virtual size_t getNumQuadPoints() const = 0

纯虚函数，获取积分点数量。

**返回值:**
- 积分点数量

### virtual const Eigen::VectorXd& getShapeFunctions(size_t q_index) const = 0

纯虚函数，获取指定积分点的形函数值。

**参数:**
- `q_index` - 积分点索引

**返回值:**
- 形函数值向量

### virtual const Eigen::MatrixXd& getShapeFunctionDerivatives(size_t q_index) const = 0

纯虚函数，获取指定积分点的形函数导数（参考坐标系）。

**参数:**
- `q_index` - 积分点索引

**返回值:**
- 形函数导数矩阵

### virtual double getQuadWeight(size_t q_index) const = 0

纯虚函数，获取指定积分点的积分权重。

**参数:**
- `q_index` - 积分点索引

**返回值:**
- 积分权重

### static std::unique_ptr<FiniteElement> create(ElementType type, int order)

创建指定类型和阶次的有限单元。

**参数:**
- `type` - 单元类型
- `order` - 积分阶次

**返回值:**
- 有限单元指针

## 派生类

### FiniteElementImpl

模板化的有限单元实现类，提供了具体单元类型的实现。

## 实现细节

`FiniteElement` 类通过抽象接口定义了有限单元所需的基本功能，包括：
1. 获取单元类型和节点数量
2. 获取积分点数量和相关信息
3. 获取形函数值和导数
4. 获取积分权重

具体实现通过模板类 `FiniteElementImpl` 完成，该类支持以下单元类型：
- 线单元 (ElementType::Line)
- 四边形单元 (ElementType::Quadrilateral)
- 四面体单元 (ElementType::Tetrahedron)
- 六面体单元 (ElementType::Hexahedron)

## 示例用法

```cpp
// 创建一个四边形单元，积分阶次为1
auto fe = FEM::FiniteElement::create(FEM::ElementType::Quadrilateral, 1);

// 获取单元信息
std::cout << "单元类型: " << static_cast<int>(fe->getType()) << std::endl;
std::cout << "节点数: " << fe->getNumNodes() << std::endl;
std::cout << "积分点数: " << fe->getNumQuadPoints() << std::endl;

// 获取第一个积分点的信息
const auto& N = fe->getShapeFunctions(0);
const auto& dN_dxi = fe->getShapeFunctionDerivatives(0);
double weight = fe->getQuadWeight(0);
```

## 依赖关系

- [ElementType](../../mesh/classes/Element.md) - 单元类型枚举
- [Quadrature](../../../utils/classes/Quadrature.md) - 积分规则
- [ShapeFunctions](../../../utils/classes/ShapeFunctions.md) - 形函数计算
- Eigen - 向量和矩阵运算