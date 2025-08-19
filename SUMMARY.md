# ETS_FEM_Kernel 文档索引

## 概述

ETS_FEM_Kernel 是一个有限元方法（FEM）计算内核的 C++ 实现，用于求解工程和物理问题，如热传导。该项目提供了一个模块化、可扩展的有限元求解器核心。

## 命名空间文档

### [fem](docs/fem/README.md)

核心有限元模块。

#### [fem::core](docs/fem/core/README.md)

核心计算类，包括自由度管理、有限元值计算、线性求解器等。

##### 类列表
- [DofManager](docs/fem/core/classes/DofManager.md) - 自由度管理器
- [FEValues](docs/fem/core/classes/FEValues.md) - 有限元值计算器
- [LinearSolver](docs/fem/core/classes/LinearSolver.md) - 线性求解器
- [Problem](docs/fem/core/classes/Problem.md) - 问题定义基类
- [ReferenceElement](docs/fem/core/classes/ReferenceElement.md) - 参考单元类

#### [fem::io](docs/fem/io/README.md)

输入输出模块，负责模型导入和结果导出。

##### 类列表
- [Importer](docs/fem/io/classes/Importer.md) - 数据导入器
- [Exporter](docs/fem/io/classes/Exporter.md) - 数据导出器

#### [fem::kernels](docs/fem/kernels/README.md)

物理内核，实现不同物理问题的计算逻辑。

##### 类列表
- [Kernel](docs/fem/kernels/classes/Kernel.md) - 内核基类
- [HeatDiffusionKernel](docs/fem/kernels/classes/HeatDiffusionKernel.md) - 热传导内核

#### [fem::materials](docs/fem/materials/README.md)

材料属性定义和管理。

##### 类列表
- [Material](docs/fem/materials/classes/Material.md) - 材料类
- [MaterialProperty](docs/fem/materials/classes/MaterialProperty.md) - 材料属性类

#### [fem::mesh](docs/fem/mesh/README.md)

网格结构定义，包括节点、单元等。

##### 类列表
- [Node](docs/fem/mesh/classes/Node.md) - 节点类
- [Element](docs/fem/mesh/classes/Element.md) - 单元类
- [Mesh](docs/fem/mesh/classes/Mesh.md) - 网格类

#### [fem::physics](docs/fem/physics/README.md)

物理问题接口定义。

##### 类列表
- [HeatTransfer](docs/fem/physics/classes/HeatTransfer.md) - 热传导问题类

### [utils](docs/utils/README.md)

工具类，如形函数、积分规则、日志等。

#### 类列表
- [ShapeFunctions](docs/utils/classes/ShapeFunctions.md) - 形函数类
- [Quadrature](docs/utils/classes/Quadrature.md) - 积分规则类
- [InterpolationUtilities](docs/utils/classes/InterpolationUtilities.md) - 插值工具类
- [SimpleLogger](docs/utils/classes/SimpleLogger.md) - 简单日志类

## 使用示例

以下是一个简单的使用示例，展示如何使用 ETS_FEM_Kernel 求解热传导问题：

```cpp
#include "fem/core/Problem.hpp"
#include "fem/physics/HeatTransfer.hpp"
#include "fem/materials/Material.hpp"
#include "fem/kernels/HeatDiffusionKernel.hpp"
#include "fem/io/Exporter.hpp"

int main() {
    // 创建网格
    auto mesh = FEM::Mesh::create_uniform_2d_mesh(1.0, 1.0, 10, 10);
    
    // 创建材料
    FEM::Material material("Copper");
    material.setProperty("thermal_conductivity", 401.0);
    
    // 创建物理场
    auto physics = std::make_unique<FEM::HeatTransfer<2>>();
    auto kernel = std::make_unique<FEM::HeatDiffusionKernel<2, 4>>(material);
    physics->addKernel(std::move(kernel));
    
    // 创建问题
    FEM::Problem<2> problem(std::move(mesh), std::move(physics));
    
    // 组装系统
    problem.assemble();
    
    // 添加边界条件
    problem.addDirichletBC(0, 300.0);  // 左下角节点温度为300K
    problem.addDirichletBC(10, 400.0); // 右下角节点温度为400K
    
    // 应用边界条件并求解
    problem.applyBCs();
    problem.solve();
    
    // 输出结果
    problem.printResults();
    
    // 导出为VTK文件
    FEM::IO::Exporter::write_vtk("temperature_results.vtk", problem);
    
    return 0;
}
```

## 依赖关系

- Eigen 3.4.0 - 高效矩阵运算库
- GoogleTest v1.14.0 - 单元测试框架（仅用于测试）

## 许可证

该项目采用 MIT 许可证。