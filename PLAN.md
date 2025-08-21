### **重构计划：实现彻底的泛型设计**

本次重构的核心思想是：**让 `Kernel` 和 `FEValues` 等计算类在运行时从 `Element` 对象中动态获取所需信息，而不是通过模板参数在编译时写死。**

#### **第0步：准备工作与思想转变**

* **目标**：从“为一种单元编程”转变为“为任意单元编程”。
* **分支**：在开始重构前，强烈建议您创建一个新的 Git 分支，例如 `feature/generic-elements`。这将确保您的主干代码在重构期间保持稳定。
* **备份测试**：确保您当前的测试是全部通过的。这些测试将成为重构后验证系统正确性的“安全网”。

#### **第1步：增强 `Element` 类，使其成为信息中心**

`Element` 对象需要提供足够的信息，让其他模块了解其拓扑结构。

**修改 `fem/mesh/Element.hpp`**

```cpp
// file: fem/mesh/Element.hpp
#pragma once
#include <vector>
#include <memory>
#include "Node.hpp"

namespace FEM {
    // 定义一个枚举来表示不同的单元几何类型
    enum class ElementType {
        Line2,      // 2节点线单元
        Triangle3,  // 3节点三角形单元
        Quad4,      // 4节点四边形单元
        Tetra4,     // 4节点四面体单元
        Hexa8,      // 8节点六面体单元
        // ... 未来可以添加高阶单元，如 Line3, Triangle6 等
    };

    class Element {
    public:
        // 构造函数需要知道自己的类型
        Element(int id, ElementType type, const std::vector<std::shared_ptr<Node>>& nodes);

        // ... 保留现有方法 ...
        int getId() const { return id_; }
        const std::vector<std::shared_ptr<Node>>& getNodes() const { return nodes_; }

        // --- 新增核心方法 ---
        ElementType getType() const { return type_; }
        int getNumNodes() const { return nodes_.size(); } // 直接从节点列表获取

    private:
        int id_;
        ElementType type_; // 新增成员变量
        std::vector<std::shared_ptr<Node>> nodes_;
    };
}
```

* **任务**：
  1.  在 `Element.hpp` 中添加 `ElementType` 枚举。
  2.  为 `Element` 类增加 `type_` 成员变量和 `getType()` `getNumNodes()` 方法。
  3.  更新 `Mesh` 类中创建 `Element` 的相关代码，使其能够正确设置 `ElementType`。

#### **第2步：解耦 `Kernel` 与节点数 `TNumNodes_`**

这是最关键的一步。`Kernel` 将不再“知道”它为多少个节点的单元工作，它会在每次计算时查询 `Element` 对象。

**修改 `fem/kernels/Kernel.hpp`**

```cpp
// file: fem/kernels/Kernel.hpp
#pragma once
#include <Eigen/Dense>
#include "../mesh/Element.hpp"
#include <complex>

namespace FEM {
    // 移除 TNumNodes_ 模板参数
    template<int TDim, typename TScalar = double>
    class Kernel {
    public:
        // 返回类型变为动态大小的矩阵
        using MatrixType = Eigen::Matrix<TScalar, Eigen::Dynamic, Eigen::Dynamic>;

        virtual ~Kernel() = default;

        // 接口保持不变，但其实现将依赖 element.getNumNodes()
        virtual MatrixType compute_element_matrix(const Element& element) = 0;
    };
}
```

**修改所有具体的 `Kernel` 实现** (以 `HeatDiffusionKernel` 为例)

```cpp
// file: fem/kernels/HeatDiffusionKernel.hpp
// 移除 TNumNodes_ 模板参数
template<int TDim, typename TScalar = double>
class HeatDiffusionKernel : public Kernel<TDim, TScalar> {
public:
    using MatrixType = typename Kernel<TDim, TScalar>::MatrixType;

    // ... 构造函数 ...

    MatrixType compute_element_matrix(const Element& element) override {
        // 1. 在运行时获取节点数
        const int num_nodes = element.getNumNodes();

        FEValues fe_values(element, 1); // FEValues 也需要改造 (见第3步)
        MatrixType K_elem = MatrixType::Zero(num_nodes, num_nodes); // 动态创建矩阵

        // ...
        for (size_t q = 0; q < fe_values.n_quad_points(); ++q) {
            // ...
            for (size_t i = 0; i < num_nodes; ++i) { // 循环边界现在是动态的
                for (size_t j = 0; j < num_nodes; ++j) {
                    // ...
                }
            }
        }
        return K_elem;
    }
    // ...
};
```

* **任务**：
  1.  修改 `Kernel.hpp` 的基类定义。
  2.  修改所有继承自 `Kernel` 的类 (`HeatDiffusionKernel`, `HeatCapacityKernel`, `ElectrostaticsKernel` 等)，移除 `TNumNodes_` 模板参数，并更新 `compute_element_matrix` 的内部实现，使其矩阵大小和循环边界动态化。

#### **第3步：改造 `FEValues` 以支持动态配置**

`FEValues` (以及 `FEFaceValues`) 封装了形函数和积分计算，它也必须从编译时绑定改为运行时配置。

**修改 `fem/core/FEValues.hpp`**

```cpp
// file: fem/core/FEValues.hpp

class FEValues {
public:
    // 构造函数现在直接接收 Element 对象
    FEValues(const Element& element, int quadrature_order) {
        // 根据 element.getType() 和 quadrature_order
        // 1. 获取积分点和权重 (从 Quadrature 库)
        // 2. 获取参考单元的形函数值和梯度 (从 ShapeFunctions 库)
        // 3. 动态分配存储空间
        //    shape_values_.resize(element.getNumNodes(), num_quad_points);
        //    shape_grads_.resize(element.getNumNodes(), num_quad_points);
        // ...
    }

    // ... reinit, JxW, shape_value 等方法保持不变 ...

private:
    // 成员变量存储大小是动态的
    Eigen::MatrixXd shape_values_;
    std::vector<Eigen::MatrixXd> shape_grads_;
    // ...
};
```

* **任务**：
  1.  重构 `FEValues` 的构造函数。它需要一个工厂或者一个大型的 `switch` 语句，根据传入的 `element.getType()` 来初始化正确的形函数和积分规则。
  2.  将其内部存储（如形函数值的矩阵）从固定大小（基于模板参数）改为动态大小的 `Eigen` 矩阵或 `std::vector`。

#### **第4步：更新 `PhysicsField` 以管理通用的 `Kernel`**

现在 `Kernel` 已经泛化，`PhysicsField` 的定义也变得更加简单和通用。

**修改 `fem/physics/PhysicsField.hpp`**

```cpp
// file: fem/physics/PhysicsField.hpp

namespace FEM {
    template<int TDim, typename TScalar = double>
    class PhysicsField {
    public:
        // KernelBase 不再需要 TNumNodes_，这解决了最初的问题！
        using KernelBase = Kernel<TDim, TScalar>;

        void addKernel(std::unique_ptr<KernelBase> kernel) {
            kernels_.push_back(std::move(kernel));
        }

        // assemble_volume 内部实现几乎不变，因为 K_elem 已经是动态矩阵，
        // 其尺寸会在每次调用 compute_element_matrix 时自动匹配当前单元。
        void assemble_volume(...) const { ... }

    private:
        std::vector<std::unique_ptr<KernelBase>> kernels_;
        // ...
    };
}
```

* **任务**：
  1.  在 `PhysicsField.hpp` 中，更新 `KernelBase` 的 `using` 声明，移除硬编码的节点数 `2`。这是整个重构计划中水到渠成的一步。

#### **第5步：更新测试用例并验证**

这是确保重构成功的最后一步，也是最重要的一步。

* **任务**:
  1.  **修复现有测试**：您在 `PLAN.md` 中设计的 `tests/test_frequency_domain_heat.cpp` 以及其他测试现在都无法编译。您需要更新它们，以匹配新的 `Kernel` 构造方式（不再需要 `TNumNodes_` 模板参数）。
  2.  **创建新测试**：
    * 创建一个新的测试文件，例如 `tests/test_mixed_elements.cpp`。
    * 在这个测试中，手动创建一个包含不同类型单元的 `Mesh` 对象（例如，一个由两个 `Triangle3` 和一个 `Quad4` 组成的2D网格）。
    * 为这个混合网格设置一个简单的物理问题（例如，热传导），并定义一个已知的边界条件。
    * 求解并验证结果的合理性。这个测试的通过将是您重构成功的最有力证明。

### **重构后的收益**

完成以上步骤后，您的 FEM 内核将达到一个新的高度：

* **真正的通用性**：能够求解任何支持的单元类型，甚至是混合网格，无需重新编译代码。
* **高度可扩展**：未来若要支持新的单元类型（如二次单元、棱柱单元），您只需：
  1.  在 `ElementType` 枚举中添加新类型。
  2.  为新单元提供相应的形函数和积分规则（更新 `FEValues` 的工厂逻辑）。
      内核的其他部分（`Problem`, `PhysicsField`, `Kernel`）将无需任何改动。
* **架构清晰**：职责划分更加明确，`Element` 负责描述几何，`Kernel` 负责描述物理，`FEValues` 负责有限元空间的映射。

这个重构计划是一项较大的工程，但它将为您的项目奠定坚实的商业化基础。建议您按部就班，每完成一步都编译并运行测试，以确保问题被及时发现和解决。