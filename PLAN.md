好的，为了引入频域研究，我们需要将代码的核心部分泛型化，使其不再局限于使用 `double` 作为标量类型，而是能够支持 `std::complex<double>`。这是一个重要的重构，因为它触及了从矩阵、向量到线性求解器和物理内核的多个层面。

本次重构将遵循以下核心思想：

1.  **引入模板参数**：在 `Problem`、`PhysicsField`、`Kernel` 等核心类中引入一个模板参数 `typename TScalar`，用于代表计算中标量的类型。
2.  **更新数据结构**：将所有 `Eigen::VectorXd` 和 `Eigen::SparseMatrix<double>` 替换为基于 `TScalar` 的泛型版本。
3.  **实现频域物理**：创建一个新的 `Kernel` 来表示频域下的物理现象（例如，热容或电容效应）。
4.  **建立基准测试**：创建一个具有解析解的频域基准测试，以验证重构的正确性。

-----

### **重构步骤详解**

#### **第1步：泛型化 `Kernel`**

`Kernel` 是计算的核心，首先需要支持不同标量类型的输出。

**修改 `fem/kernels/Kernel.hpp`**

```cpp
// file: fem/kernels/Kernel.hpp
#pragma once
#include <Eigen/Dense>
#include "../mesh/Element.hpp"
#include <complex> // 包含 complex 头文件

namespace FEM {
    // 增加 TScalar 模板参数，并默认为 double 以保持向后兼容
    template<int TDim, int TNumNodes_, typename TScalar = double>
    class Kernel {
    public:
        static constexpr int NumNodes = TNumNodes_;
        using MatrixType = Eigen::Matrix<TScalar, TNumNodes_, TNumNodes_>; // 使用 using 别名

        virtual ~Kernel() = default;
        
        // 返回值类型变为 MatrixType
        virtual MatrixType compute_element_matrix(const Element& element) = 0;
    };
}
```

* **修改** `HeatDiffusionKernel.hpp` 和 `ElectrostaticsKernel.hpp` 以继承自新的 `Kernel` 模板。例如：
  ```cpp
  template<int TDim, int TNumNodes_, typename TScalar = double>
  class HeatDiffusionKernel : public Kernel<TDim, TNumNodes_, TScalar> { ... };
  ```

#### **第2步：创建新的频域 `Kernel`**

为了处理频域问题 `∇·(k∇T) + jωρc * T = 0`，我们需要一个 `Kernel` 来计算 `jωρc * T` 这一项的贡献，它会形成一个质量矩阵乘以 `jω`。

**新增 `fem/kernels/HeatCapacityKernel.hpp`**

```cpp
// file: fem/kernels/HeatCapacityKernel.hpp
#pragma once
#include "Kernel.hpp"
#include "../materials/Material.hpp"
#include "../core/FEValues.hpp"
#include <complex>

namespace FEM {
    template<int TDim, int TNumNodes_, typename TScalar = double>
    class HeatCapacityKernel : public Kernel<TDim, TNumNodes_, TScalar> {
    public:
        using MatrixType = typename Kernel<TDim, TNumNodes_, TScalar>::MatrixType;

        HeatCapacityKernel(const Material& material, double omega) : material_(material), omega_(omega) {}

        MatrixType compute_element_matrix(const Element& element) override {
            FEValues fe_values(element, 1);
            MatrixType M_elem = MatrixType::Zero();

            const double rho = material_.getProperty("density").getValue();
            const double c = material_.getProperty("specific_heat").getValue();
            
            // 频域项: j * omega * rho * c
            const TScalar coeff = TScalar(0.0, 1.0) * omega_ * rho * c;

            for (size_t q = 0; q < fe_values.n_quad_points(); ++q) {
                fe_values.reinit(q);
                double JxW = fe_values.JxW();
                for (size_t i = 0; i < TNumNodes_; ++i) {
                    for (size_t j = 0; j < TNumNodes_; ++j) {
                        // M_ij = ∫(coeff * Ni * Nj) dV
                        M_elem(i, j) += coeff * fe_values.shape_value(i, q) * fe_values.shape_value(j, q) * JxW;
                    }
                }
            }
            return M_elem;
        }

    private:
        const Material& material_;
        double omega_; // 角频率
    };
}
```

#### **第3步：泛型化 `PhysicsField`**

`PhysicsField` 需要能够管理和组装由泛型 `Kernel` 生成的矩阵。

**修改 `fem/physics/PhysicsField.hpp`**

```cpp
// file: fem/physics/PhysicsField.hpp
#pragma once
// ... includes
#include "../kernels/Kernel.hpp"

namespace FEM {
    template<int TDim, typename TScalar = double> // 添加 TScalar 模板参数
    class PhysicsField {
    public:
        // ...
        // Kernel 的类型现在是泛型的
        using KernelBase = Kernel<TDim, 2, TScalar>; // 假设2节点单元, 实际应更通用
        
        void addKernel(std::unique_ptr<KernelBase> kernel) {
            kernels_.push_back(std::move(kernel));
        }

        void assemble_volume(const Mesh& mesh, const DofManager& dof_manager,
                             Eigen::SparseMatrix<TScalar>& K_global, // 矩阵和向量类型泛型化
                             Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const {
            // ... 内部实现不变，但调用的 kernel->compute_element_matrix() 返回的是 TScalar 矩阵
        }
        
        // ... 其他方法类似修改 ...

    private:
        std::vector<std::unique_ptr<KernelBase>> kernels_;
        // ...
    };

    // 修改 HeatTransfer.hpp 使其继承自泛型化的 PhysicsField
    template<int TDim, typename TScalar = double>
    class HeatTransfer : public PhysicsField<TDim, TScalar> { ... };
}
```

**注意**: `KernelBase` 的节点数在这里被硬编码为2，一个更完整的重构需要处理混合单元和不同节点数的情况，但这超出了本次重构的范围。

#### **第4步：泛型化 `Problem` 和 `LinearSolver`**

`Problem` 是最高层的协调者，必须被泛型化。

**修改 `fem/core/Problem.hpp`**

```cpp
// file: fem/core/Problem.hpp
#pragma once
#include <Eigen/Sparse>
#include <complex>
// ... 其他 includes

namespace FEM {
    template<int TDim, typename TScalar = double> // 添加 TScalar 模板参数
    class Problem {
    public:
        // ... 构造函数等 ...
        Problem(std::unique_ptr<Mesh> mesh, std::unique_ptr<PhysicsField<TDim, TScalar>> physics, ...) { ... }

        // ...
        const Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& getSolution() const { return U_solution_; }

    private:
        // ...
        void initializeSystem() {
            size_t num_dofs = dof_manager_->getNumDofs();
            K_global_.resize(num_dofs, num_dofs);
            F_global_.resize(num_dofs);
            U_solution_.resize(num_dofs);
            F_global_.setZero();
        }

        // --- 核心数据结构泛型化 ---
        Eigen::SparseMatrix<TScalar> K_global_;
        Eigen::Matrix<TScalar, Eigen::Dynamic, 1> F_global_;
        Eigen::Matrix<TScalar, Eigen::Dynamic, 1> U_solution_;
    };
}
```

**修改 `fem/core/LinearSolver.hpp`**

Eigen 的求解器本身就是模板化的，所以修改起来很直接。

```cpp
// file: fem/core/LinearSolver.hpp
#include <Eigen/Sparse>
#include <complex>

namespace FEM {
    // ...
    class LinearSolver {
    public:
        // ...
        template<typename TScalar> // 将 solve 方法模板化
        Eigen::Matrix<TScalar, Eigen::Dynamic, 1> solve(const Eigen::SparseMatrix<TScalar>& A, 
                                                        const Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& b) {
            // ...
            if (solver_type_ == SolverType::SparseLU) {
                Eigen::SparseLU<Eigen::SparseMatrix<TScalar>> solver;
                // ...
            } else if (solver_type_ == SolverType::ConjugateGradient) {
                Eigen::ConjugateGradient<Eigen::SparseMatrix<TScalar>, Eigen::Lower | Eigen::Upper> solver;
                // ...
            }
            // ...
        }
    };
}
```

#### **第5步：引入频域热传导基准测试**

我们将测试一维杆中的热波问题。其控制方程为 `d/dx(-k * dT/dx) + jωρc * T = 0`。

**问题设定**:

* 长度 `L=1m` 的杆。
* 左端边界条件 `T(0) = 1`。
* 右端边界条件 `T(L) = 0`。
* 材料属性：`k=1`, `ρ=1`, `c=1`。
* 角频率 `ω=2π`。

**解析解**:
解的形式为 `T(x) = C1 * exp(γ*x) + C2 * exp(-γ*x)`，其中 `γ = sqrt(jωρc/k) = (1+j)*sqrt(ω/2)`。
通过边界条件可以求得 `C1` 和 `C2`，最终得到 `T(x) = (exp(-γ*x) - exp(γ*(x-2L))) / (1 - exp(-2γL))`。

**新增测试文件 `tests/test_frequency_domain_heat.cpp`**

```cpp
// file: tests/test_frequency_domain_heat.cpp
#include <gtest/gtest.h>
#include "fem/core/Problem.hpp"
#include "fem/materials/Material.hpp"
#include "fem/kernels/HeatDiffusionKernel.hpp"
#include "fem/kernels/HeatCapacityKernel.hpp" // 引入新Kernel
#include "fem/physics/HeatTransfer.hpp"
#include "fem/bcs/DirichletBC.hpp"
#include <complex>

// 定义标量类型为复数
using ComplexScalar = std::complex<double>;

TEST(FrequencyDomainTest, 1DHeatWave) {
    constexpr int dim = 1;
    const double length = 1.0;
    const int num_elements = 50; // 使用足够多的单元以保证精度
    const double omega = 2.0 * M_PI;

    // 1. 创建网格
    auto mesh = FEM::Mesh::create_uniform_1d_mesh(length, num_elements);

    // 2. 定义材料 (增加密度和比热)
    FEM::Material material("TestMaterial");
    material.setProperty("thermal_conductivity", 1.0);
    material.setProperty("density", 1.0);
    material.setProperty("specific_heat", 1.0);

    // 3. 创建物理场 (使用复数类型)
    auto heat_physics = std::make_unique<FEM::HeatTransfer<dim, ComplexScalar>>();

    // 4. 添加稳态和频域 Kernels
    heat_physics->addKernel(
        std::make_unique<FEM::HeatDiffusionKernel<dim, 2, ComplexScalar>>(material)
    );
    heat_physics->addKernel(
        std::make_unique<FEM::HeatCapacityKernel<dim, 2, ComplexScalar>>(material, omega)
    );

    // 5. 添加边界条件
    heat_physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim, ComplexScalar>>("left", ComplexScalar(1.0, 0.0))
    );
    heat_physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim, ComplexScalar>>("right", ComplexScalar(0.0, 0.0))
    );
    
    // 6. 创建 Problem (使用复数类型)
    auto problem = std::make_unique<FEM::Problem<dim, ComplexScalar>>(std::move(mesh), std::move(heat_physics));
    
    // 7. 求解
    problem->assemble();
    problem->solve();

    // 8. 验证结果
    const auto& solution = problem->getSolution();
    const auto& result_mesh = problem->getMesh();
    
    // 解析解
    const ComplexScalar j(0.0, 1.0);
    const ComplexScalar gamma = std::sqrt(j * omega); // k=rho=c=1
    auto analytical_solution = [&](double x) {
        return (std::exp(-gamma * x) - std::exp(gamma * (x - 2.0 * length))) / (1.0 - std::exp(-2.0 * gamma * length));
    };

    double max_error = 0.0;
    for (const auto& node_ptr : result_mesh.getNodes()) {
        const auto& node = *node_ptr;
        int dof_index = problem->getDofManager().getNodeDof(node.getId(), 0);
        ComplexScalar fem_val = solution(dof_index);
        ComplexScalar analytical_val = analytical_solution(node.getX());
        
        double error = std::abs(fem_val - analytical_val);
        if (error > max_error) {
            max_error = error;
        }
    }
    
    // 期望误差在一个较小的范围内
    EXPECT_LT(max_error, 1e-4);
}
```

最后，将 `tests/test_frequency_domain_heat.cpp` 添加到 `CMakeLists.txt` 的测试目标中。

-----

### **重构后的优势**

完成以上步骤后，您的求解器内核将获得巨大的灵活性：

* **支持多领域分析**：代码库现在可以无缝处理稳态问题（`TScalar = double`）和频域问题（`TScalar = std::complex<double>`）。
* **代码复用性高**：相同的 `HeatDiffusionKernel` 和 `Problem` 逻辑可以同时服务于两种不同类型的分析，只需在实例化时指定不同的模板参数。
* **易于扩展**：未来若要支持其他需要复数运算的物理场（如电磁波），只需创建新的复数 `Kernel` 并将其添加到物理场中即可，框架已完全支持。
* **健壮性**：通过解析解基准测试，我们确保了这次大规模重构的正确性，为后续开发提供了信心。