# Quadrature 类

## 描述

`Quadrature` 类提供数值积分点和权重的计算方法，用于有限元分析中的数值积分。该类支持一维线单元、二维四边形单元和三维六面体单元的高斯积分点计算。数值积分是有限元方法中计算单元矩阵和向量的关键步骤。

## 类定义

```cpp
class Quadrature
```

## 数据结构

### QuadraturePoint

表示一个积分点的数据结构。

**成员:**
- `Eigen::VectorXd point` - 积分点坐标
- `double weight` - 积分点权重

## 成员函数

### static std::vector<QuadraturePoint> getLineQuadrature(int order)

获取一维线单元的高斯积分点。

**参数:**
- `order` - 积分阶次（目前仅支持1阶，即2点高斯积分）

**返回值:**
- 积分点和权重的向量

**异常:**
- `std::invalid_argument` - 当阶次大于1时抛出

### static std::vector<QuadraturePoint> getQuadrilateralQuadrature(int order)

获取二维四边形单元的高斯积分点。

**参数:**
- `order` - 积分阶次（目前仅支持1阶，即2x2点高斯积分）

**返回值:**
- 积分点和权重的向量

**异常:**
- `std::invalid_argument` - 当阶次大于1时抛出

### static std::vector<QuadraturePoint> getHexahedronQuadrature(int order)

获取三维六面体单元的高斯积分点。

**参数:**
- `order` - 积分阶次（目前仅支持1阶，即2x2x2点高斯积分）

**返回值:**
- 积分点和权重的向量

**异常:**
- `std::invalid_argument` - 当阶次大于1时抛出

## 示例用法

```cpp
// 获取线单元积分点
auto line_quad = FEM::Utils::Quadrature::getLineQuadrature(1);
std::cout << "Line quadrature points: " << line_quad.size() << std::endl;
for (const auto& qp : line_quad) {
    std::cout << "Point: " << qp.point.transpose() << ", Weight: " << qp.weight << std::endl;
}

// 获取四边形单元积分点
auto quad_quad = FEM::Utils::Quadrature::getQuadrilateralQuadrature(1);
std::cout << "Quadrilateral quadrature points: " << quad_quad.size() << std::endl;

// 获取六面体单元积分点
auto hex_quad = FEM::Utils::Quadrature::getHexahedronQuadrature(1);
std::cout << "Hexahedron quadrature points: " << hex_quad.size() << std::endl;
```

## 实现细节

目前所有单元类型仅支持1阶积分规则：

1. **线单元（Line Element）**：
   - 2点高斯积分
   - 积分点：±1/√3
   - 权重：1.0

2. **四边形单元（Quadrilateral Element）**：
   - 2x2点高斯积分（共4个点）
   - 积分点：(±1/√3, ±1/√3) 的组合
   - 权重：1.0

3. **六面体单元（Hexahedron Element）**：
   - 2x2x2点高斯积分（共8个点）
   - 积分点：(±1/√3, ±1/√3, ±1/√3) 的组合
   - 权重：1.0

## 依赖关系

- Eigen - 向量运算库
- STL - 向量、异常处理等标准库组件
- 数学库 - 平方根函数