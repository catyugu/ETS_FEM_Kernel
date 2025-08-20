
### **重构目标**

将目前硬编码在 `Problem` 类中的边界条件逻辑，重构为一个通用的、面向对象的边界条件框架。此框架将统一处理狄利克雷（Dirichlet）、诺伊曼（Neumann）和柯西（Cauchy）边界条件，提高代码的可扩展性和可维护性。

### **核心设计思想**

1.  **抽象化**：创建一个通用的 `BoundaryCondition` 基类，所有具体边界条件都继承自它。
2.  **权责分离**：
    * `PhysicsField` 类负责**持有和管理**与其相关的边界条件集合。
    * `Mesh` 类负责提供**几何信息**，例如根据名称查询边界上的节点或单元。
    * `Problem` 类作为**协调器**，负责触发组装流程和在求解前统一施加狄利克雷约束。
3.  **统一接口**：所有边界条件都通过 `PhysicsField` 的 `addBoundaryCondition` 方法添加，使得用户代码（如 `main.cpp`）更加简洁和一致。

-----

### **重构执行步骤**

#### **第 1 步：创建新的目录和文件**

1.  在 `fem` 目录下创建一个新目录 `bcs`，用于存放所有具体的边界条件实现。
2.  **创建新文件** `fem/core/BoundaryCondition.hpp`：定义边界条件基类。
3.  **创建新文件** `fem/bcs/DirichletBC.hpp`：定义狄利克雷边界条件。
4.  **创建新文件** `fem/bcs/NeumannBC.hpp`：定义诺伊曼边界条件。
5.  **创建新文件** `fem/bcs/CauchyBC.hpp`：定义柯西边界条件。

#### **第 2 步：填充新文件内容**

##### **2.1 `fem/core/BoundaryCondition.hpp`**

```cpp
#pragma once

#include <Eigen/Sparse>
#include <string>
#include <vector>
#include "../mesh/Mesh.hpp"
#include "DofManager.hpp"

namespace FEM {

    // 边界条件类型的枚举
    enum class BCType {
        Dirichlet,
        Neumann,
        Cauchy
    };

    // 边界条件抽象基类
    template<int TDim>
    class BoundaryCondition {
    public:
        virtual ~BoundaryCondition() = default;

        // 应用边界条件。对于Neumann和Cauchy，此方法会修改K和F。
        // 对于Dirichlet，此方法为空，因为它的应用逻辑是特殊的。
        virtual void apply(const Mesh& mesh, const DofManager& dof_manager,
                           Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) const = 0;
                           
        virtual BCType getType() const = 0;
        
        const std::string& getBoundaryName() const { return boundary_name_; }

    protected:
        BoundaryCondition(const std::string& boundary_name) : boundary_name_(boundary_name) {}
        std::string boundary_name_;
    };
}
```

##### **2.2 `fem/bcs/DirichletBC.hpp`**

```cpp
#pragma once

#include "../core/BoundaryCondition.hpp"

namespace FEM {
    template<int TDim>
    class DirichletBC : public BoundaryCondition<TDim> {
    public:
        DirichletBC(const std::string& boundary_name, double value)
            : BoundaryCondition<TDim>(boundary_name), value_(value) {}

        // 留空，由 Problem 类统一处理
        void apply(const Mesh& mesh, const DofManager& dof_manager,
                   Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) const override {}
        
        BCType getType() const override { return BCType::Dirichlet; }

        double getValue() const { return value_; }

    private:
        double value_;
    };
}
```

##### **2.3 `fem/bcs/NeumannBC.hpp`**

```cpp
#pragma once

#include "../core/BoundaryCondition.hpp"
#include "../core/FEValues.hpp" // 假设 FEFaceValues 在此或相关文件中定义

namespace FEM {
    template<int TDim>
    class NeumannBC : public BoundaryCondition<TDim> {
    public:
        NeumannBC(const std::string& boundary_name, double value)
            : BoundaryCondition<TDim>(boundary_name), value_(value) {}

        void apply(const Mesh& mesh, const DofManager& dof_manager,
                   Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) const override {
            
            const auto& boundary_elements = mesh.getBoundaryElements(this->boundary_name_);

            for (const auto& elem_ptr : boundary_elements) {
                const Element& face_element = *elem_ptr;
                FEFaceValues fe_face_values(face_element, 1, AnalysisType::SCALAR_DIFFUSION);
                Eigen::VectorXd F_elem_bc = Eigen::VectorXd::Zero(face_element.getNumNodes());

                for (size_t q = 0; q < fe_face_values.n_quad_points(); ++q) {
                    fe_face_values.reinit(q);
                    const auto& N = fe_face_values.N();
                    F_elem_bc += N * value_ * fe_face_values.JxW();
                }
                
                std::vector<int> dofs(face_element.getNumNodes());
                for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                    dofs[i] = dof_manager.getNodeDof(face_element.getNodeId(i), 0);
                }

                for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                    F_global(dofs[i]) += F_elem_bc(i);
                }
            }
        }
        
        BCType getType() const override { return BCType::Neumann; }

    private:
        double value_;
    };
}
```

***注意***: `NeumannBC` 和 `CauchyBC` 的实现依赖于一个 `FEFaceValues` 类，该类专门用于在 TDim-1 维度的边界单元上进行积分。你需要自行实现这个类，其功能与现有的 `FEValues` 类似。

##### **2.4 `fem/bcs/CauchyBC.hpp`**

```cpp
#pragma once

#include "../core/BoundaryCondition.hpp"
#include "../core/FEValues.hpp"

namespace FEM {
    template<int TDim>
    class CauchyBC : public BoundaryCondition<TDim> {
    public:
        CauchyBC(const std::string& boundary_name, double h_val, double T_inf_val)
            : BoundaryCondition<TDim>(boundary_name), h_(h_val), T_inf_(T_inf_val) {}

        void apply(const Mesh& mesh, const DofManager& dof_manager,
                   Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) const override {
            
            const auto& boundary_elements = mesh.getBoundaryElements(this->boundary_name_);

            for (const auto& elem_ptr : boundary_elements) {
                const Element& face_element = *elem_ptr;
                FEFaceValues fe_face_values(face_element, 1, AnalysisType::SCALAR_DIFFUSION);

                Eigen::MatrixXd K_elem_bc = Eigen::MatrixXd::Zero(face_element.getNumNodes(), face_element.getNumNodes());
                Eigen::VectorXd F_elem_bc = Eigen::VectorXd::Zero(face_element.getNumNodes());

                for (size_t q = 0; q < fe_face_values.n_quad_points(); ++q) {
                    fe_face_values.reinit(q);
                    const auto& N = fe_face_values.N();
                    K_elem_bc += h_ * N * N.transpose() * fe_face_values.JxW();
                    F_elem_bc += h_ * T_inf_ * N * fe_face_values.JxW();
                }

                std::vector<int> dofs(face_element.getNumNodes());
                for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                    dofs[i] = dof_manager.getNodeDof(face_element.getNodeId(i), 0);
                }

                for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                    F_global(dofs[i]) += F_elem_bc(i);
                    for (size_t j = 0; j < face_element.getNumNodes(); ++j) {
                        K_global.coeffRef(dofs[i], dofs[j]) += K_elem_bc(i, j);
                    }
                }
            }
        }
        
        BCType getType() const override { return BCType::Cauchy; }

    private:
        double h_;
        double T_inf_;
    };
}
```

#### **第 3 步：修改现有核心类**

##### **3.1 修改 `fem/mesh/Mesh.hpp`**

为 `Mesh` 类添加存储和查询命名边界的功能。

```cpp
// 在 Mesh.hpp 中
#include <map>
#include <string>
#include <vector>
#include <set> // 新增
#include <memory> // 确保包含

// ...

class Mesh {
    // ...
private:
    // ...
    // key 是边界名称, value 是构成该边界的单元列表
    std::map<std::string, std::vector<std::unique_ptr<Element>>> boundary_elements_;

public:
    // ... (现有方法)

    // 在网格导入时调用此方法来添加命名的边界单元
    void addBoundaryElement(const std::string& boundary_name, std::unique_ptr<Element> element) {
        boundary_elements_[boundary_name].push_back(std::move(element));
    }

    // 获取指定名称的边界单元集合
    const std::vector<std::unique_ptr<Element>>& getBoundaryElements(const std::string& boundary_name) const {
        auto it = boundary_elements_.find(boundary_name);
        if (it == boundary_elements_.end()) {
            throw std::runtime_error("Boundary with name '" + boundary_name + "' not found.");
        }
        return it->second;
    }

    // 获取指定名称边界上的所有唯一节点ID
    std::vector<int> getBoundaryNodes(const std::string& boundary_name) const {
        std::vector<int> node_ids;
        const auto& b_elements = getBoundaryElements(boundary_name);
        
        std::set<int> unique_node_ids;
        for (const auto& elem : b_elements) {
            for (size_t i = 0; i < elem->getNumNodes(); ++i) {
                unique_node_ids.insert(elem->getNodeId(i));
            }
        }
        
        node_ids.assign(unique_node_ids.begin(), unique_node_ids.end());
        return node_ids;
    }
};
```

##### **3.2 修改 `fem/physics/PhysicsField.hpp`**

让 `PhysicsField` 管理边界条件。

```cpp
// 在 PhysicsField.hpp 中
#pragma once
#include <Eigen/Sparse>
#include <vector>
#include <memory>
#include "../mesh/Mesh.hpp"
#include "../core/DofManager.hpp"
#include "../core/BoundaryCondition.hpp" // 引入新头文件

namespace FEM {
    template<int TDim>
    class PhysicsField {
    public:
        virtual ~PhysicsField() = default;

        // 重命名: assemble -> assemble_volume
        virtual void assemble_volume(const Mesh& mesh, const DofManager& dof_manager,
                                     Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) = 0;

        // 应用“自然”边界条件 (Neumann, Cauchy)
        void applyNaturalBCs(const Mesh& mesh, const DofManager& dof_manager,
                             Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) const {
            for (const auto& bc : boundary_conditions_) {
                if (bc->getType() != BCType::Dirichlet) {
                    bc->apply(mesh, dof_manager, K_global, F_global);
                }
            }
        }
        
        void addBoundaryCondition(std::unique_ptr<BoundaryCondition<TDim>> bc) {
            boundary_conditions_.push_back(std::move(bc));
        }

        const std::vector<std::unique_ptr<BoundaryCondition<TDim>>>& getBoundaryConditions() const {
            return boundary_conditions_;
        }

        virtual std::string getName() const = 0;

    private:
        std::vector<std::unique_ptr<BoundaryCondition<TDim>>> boundary_conditions_;
    };
}
```

**重要工作**：将所有继承自 `PhysicsField` 的具体物理场类（如 `HeatTransfer`、`Electrostatics`）中的 `assemble` 方法重命名为 `assemble_volume`。

##### **3.3 修改 `fem/core/Problem.hpp`**

这是改动最大的部分。`Problem` 类将成为一个高级协调器。

```cpp
// 在 Problem.hpp 中
#pragma once

// ... (其他 includes)
#include <vector>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include "DofManager.hpp"
#include "../physics/PhysicsField.hpp"
#include "LinearSolver.hpp"
#include "../bcs/DirichletBC.hpp" // 包含头文件

namespace FEM {
    template<int TDim>
    class Problem {
    public:
        // 构造函数保持不变

        void assemble() {
            PROFILE_FUNCTION();
            auto sparsity_pattern = dof_manager_->computeSparsityPattern(*mesh_);
            K_global_.reserve(sparsity_pattern.size());
            
            for (const auto& physics : physics_fields_) {
                physics->assemble_volume(*mesh_, *dof_manager_, K_global_, F_global_);
                physics->applyNaturalBCs(*mesh_, *dof_manager_, K_global_, F_global_);
            }
        }

        void solve() {
            applyDirichletBCs(); // 在求解前应用
            LinearSolver solver(solver_type_);
            U_solution_ = solver.solve(K_global_, F_global_);
        }

        // ... (Getters 保持不变)

    private:
        void initializeSystem() {
            size_t num_dofs = dof_manager_->getNumDofs();
            K_global_.resize(num_dofs, num_dofs);
            F_global_.resize(num_dofs);
            U_solution_.resize(num_dofs);
            F_global_.setZero();
        }

        void applyDirichletBCs() {
            PROFILE_FUNCTION();

            std::vector<std::pair<int, double>> all_dirichlet_dofs;
            for (const auto& physics : physics_fields_) {
                for (const auto& bc : physics->getBoundaryConditions()) {
                    if (bc->getType() == BCType::Dirichlet) {
                        const auto* dirichlet_bc = static_cast<const DirichletBC<TDim>*>(bc.get());
                        const double bc_value = dirichlet_bc->getValue();
                        
                        const auto& boundary_nodes = mesh_->getBoundaryNodes(dirichlet_bc->getBoundaryName());
                        for (int node_id : boundary_nodes) {
                            int dof_index = dof_manager_->getNodeDof(node_id, 0);
                            all_dirichlet_dofs.push_back({dof_index, bc_value});
                        }
                    }
                }
            }

            if (all_dirichlet_dofs.empty()) return;

            // (这里粘贴你原有的 applyBCs 的完整实现逻辑来修改 K_global 和 F_global)
            std::vector<int> bc_dofs;
            bc_dofs.reserve(all_dirichlet_dofs.size());
            for(const auto& bc : all_dirichlet_dofs){
                bc_dofs.push_back(bc.first);
            }
            std::sort(bc_dofs.begin(), bc_dofs.end());
            bc_dofs.erase(std::unique(bc_dofs.begin(), bc_dofs.end()), bc_dofs.end());

            for (const auto& bc : all_dirichlet_dofs) {
                int dof_bc = bc.first;
                double val_bc = bc.second;

                for (Eigen::SparseMatrix<double>::InnerIterator it(K_global_, dof_bc); it; ++it) {
                    if (!std::binary_search(bc_dofs.begin(), bc_dofs.end(), it.row())) {
                        F_global_(it.row()) -= it.value() * val_bc;
                    }
                }
            }
            
            for (int dof_bc : bc_dofs) {
                for (Eigen::SparseMatrix<double>::InnerIterator it(K_global_, dof_bc); it; ++it) { it.valueRef() = 0.0; }
                for (int k = 0; k < K_global_.outerSize(); ++k) {
                    for (Eigen::SparseMatrix<double>::InnerIterator it(K_global_, k); it; ++it) {
                        if (it.row() == dof_bc) { it.valueRef() = 0.0; }
                    }
                }
            }

            for (const auto& bc : all_dirichlet_dofs) {
                K_global_.coeffRef(bc.first, bc.first) = 1.0;
                F_global_(bc.first) = bc.second;
            }

            K_global_.prune(0.0);
        }

        std::unique_ptr<Mesh> mesh_;
        std::vector<std::unique_ptr<PhysicsField<TDim>>> physics_fields_;
        std::unique_ptr<DofManager> dof_manager_;
        SolverType solver_type_;

        Eigen::SparseMatrix<double> K_global_;
        Eigen::VectorXd F_global_;
        Eigen::VectorXd U_solution_;
        // 移除 dirichlet_bcs_ 成员变量
    };
}
```

-----

### **重构后的工作流程示例 (`main.cpp`)**

你的主程序或测试用例现在将以一种更清晰、统一的方式来定义和设置问题。

```cpp
#include "fem/core/Problem.hpp"
#include "fem/physics/HeatTransfer.hpp" // 示例物理场
#include "fem/bcs/DirichletBC.hpp"
#include "fem/bcs/NeumannBC.hpp"
#include "fem/bcs/CauchyBC.hpp"
// ...

int main() {
    // 1. 设置网格和材料
    auto mesh = std::make_unique<Mesh>(...); // 假设已从文件加载并设置好命名边界
    Material material(...);
    
    // 2. 创建物理场
    auto heat_physics = std::make_unique<HeatTransfer<3>>(material);
    
    // 3. 以统一的方式添加所有边界条件
    heat_physics->addBoundaryCondition(
        std::make_unique<DirichletBC<3>>("inlet_boundary", 100.0)
    );
    heat_physics->addBoundaryCondition(
        std::make_unique<NeumannBC<3>>("flux_boundary", 50.0)
    );
    heat_physics->addBoundaryCondition(
        std::make_unique<CauchyBC<3>>("convection_boundary", 25.0, 298.15)
    );
    
    // 4. 创建并配置 Problem
    std::vector<std::unique_ptr<PhysicsField<3>>> physics_fields;
    physics_fields.push_back(std::move(heat_physics));
    Problem<3> problem(std::move(mesh), std::move(physics_fields));
    
    // 5. 组装和求解
    problem.assemble(); // 内部自动处理 Neumann 和 Cauchy
    problem.solve();    // 内部自动处理 Dirichlet
    
    // 6. 导出结果...
    
    return 0;
}
```
