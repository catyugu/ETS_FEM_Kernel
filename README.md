# ETS_FEM_Kernel

ETS_FEM_Kernel 是一个用 C++ 编写的有限元方法（FEM）计算内核，专门用于求解工程和物理问题，如热传导和静电场等。该项目提供了一个模块化、可扩展的有限元求解器核心，支持材料建模、网格处理、多种物理问题定义和线性求解。

## 目录结构

```
.
├── app/                 # 主程序入口
├── docs/                # 项目文档
├── fem/                 # 核心有限元模块
│   ├── bcs/             # 边界条件模块
│   ├── core/            # 核心计算类
│   ├── io/              # 输入输出模块
│   ├── kernels/         # 物理内核
│   ├── materials/       # 材料属性
│   ├── mesh/            # 网格结构
│   └── physics/         # 物理问题接口
├── share/               # 第三方库（Eigen）
├── tests/               # 单元测试
└── utils/               # 工具类
```

## 功能特性

- **网格管理**: 支持节点、单元的定义与操作，使用智能指针进行内存管理
- **材料建模**: 提供基础材料属性定义
- **物理问题定义**: 支持热传导、静电场等多种物理问题建模
- **有限元计算**: 包括自由度管理、参考单元、形函数、积分规则等
- **输入输出**: 支持模型导入与结果导出
- **线性求解**: 集成多种线性方程组求解接口（直接求解器和迭代求解器）
- **性能优化**: 稀疏模式预计算、多种求解器选项

## 技术架构

### 核心组件

1. **Mesh模块**: 定义有限元网格的基本数据结构，包括节点(Node)和单元(Element)，使用智能指针进行内存管理
2. **Materials模块**: 管理材料属性，支持常数和函数形式的材料属性
3. **Core模块**: 核心计算功能，包括自由度管理、有限元值计算和线性求解器
4. **Kernels模块**: 物理内核实现，如热传导内核、静电场内核等
5. **Physics模块**: 物理问题定义接口，支持多物理场扩展
6. **IO模块**: 数据导入导出功能
7. **Utils模块**: 工具类，包括形函数、积分规则、日志、性能分析等

### 设计模式

- **模块化架构**: 按功能划分多个模块，便于维护和扩展
- **模板方法模式**: 在数值计算中使用模板以提高性能
- **抽象工厂模式**: 通过抽象物理场类支持多种物理问题
- **适配器模式**: 使用内核包装器统一管理不同类型的计算内核
- **单例模式**: 在日志记录器、性能分析器等工具类中使用
- **RAII**: 使用智能指针自动管理内存资源

## 技术选型

- **编程语言**: C++17
- **构建系统**: CMake 3.16+
- **核心库**: Eigen 3.4.0（内嵌）
- **测试框架**: GoogleTest v1.14.0

## 安装与构建

### 环境要求

- C++17 编译器（支持 CMake）
- CMake 3.16+

### 构建步骤

```bash
# 克隆项目仓库
git clone <repository-url>
cd ETS_FEM_Kernel

# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 构建项目
cmake --build .
```

### 构建选项

- **Debug 构建**:
  ```bash
  mkdir -p cmake-build-debug && cd cmake-build-debug
  cmake -DCMAKE_BUILD_TYPE=Debug ..
  cmake --build .
  ```

- **Release 构建**:
  ```bash
  mkdir -p cmake-build-release && cd cmake-build-release
  cmake -DCMAKE_BUILD_TYPE=Release ..
  cmake --build .
  ```

## 使用示例

```cpp
#include "fem/core/Problem.hpp"
#include "fem/physics/HeatTransfer.hpp"
#include "fem/physics/Electrostatics.hpp"
#include "fem/materials/Material.hpp"
#include "fem/kernels/HeatDiffusionKernel.hpp"
#include "fem/kernels/ElectrostaticsKernel.hpp"

int main() {
    // 创建网格
    auto mesh = FEM::Mesh::create_uniform_2d_mesh(1.0, 1.0, 10, 10);
    
    // 创建材料
    FEM::Material material("Copper");
    material.setProperty("thermal_conductivity", 401.0);
    
    // 创建物理场（热传导示例）
    auto physics = std::make_unique<FEM::HeatTransfer<2>>();
    auto kernel = std::make_unique<FEM::HeatDiffusionKernel<2, 4>>(material);
    physics->addKernel(std::move(kernel));
    
    // 创建问题（使用共轭梯度求解器示例）
    FEM::Problem<2> problem(std::move(mesh), std::move(physics), FEM::SolverType::ConjugateGradient);
    
    // 组装系统
    problem.assemble();
    
    // 添加边界条件
    problem.addDirichletBC(0, 300.0);
    problem.addDirichletBC(10, 400.0);
    
    // 应用边界条件并求解
    problem.applyBCs();
    problem.solve();
    
    // 输出结果
    problem.printResults();
    
    return 0;
}
```

## 文档

项目的完整文档位于 [docs](./docs) 目录中，包括：

- [API文档](./docs/SUMMARY.md) - 所有类和函数的详细说明
- [命名空间文档](./docs/fem/README.md) - 各个命名空间的概述和类列表
- [类文档](./docs/fem/core/classes) - 每个类的详细说明

## 测试

项目使用 GoogleTest 进行单元测试。运行测试：

```bash
cd build
ctest
```

或者直接运行测试可执行文件：

```bash
./fem_tests
```
