#pragma once

#include "../mesh/Mesh.hpp"
#include "DofManager.hpp"
#include "../physics/PhysicsField.hpp"
#include "LinearSolver.hpp"
#include "../bcs/DirichletBC.hpp" // 包含DirichletBC头文件
#include <Eigen/Sparse>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <utils/Profiler.hpp>

// 注意：我们已经移除了 #include "../io/Exporter.hpp"

namespace FEM {
    template<int TDim>
    class Problem {
    public:
        Problem(std::unique_ptr<Mesh> mesh, std::unique_ptr<PhysicsField<TDim>> physics, SolverType solver_type = SolverType::SparseLU)
            : mesh_(std::move(mesh)), solver_type_(solver_type) {
            physics_fields_.push_back(std::move(physics));
            dof_manager_ = std::make_unique<DofManager>(*mesh_);
            dof_manager_->buildDofMap(1);
            initializeSystem();
        }

        // 新增：支持多物理场的构造函数
        Problem(std::unique_ptr<Mesh> mesh, std::vector<std::unique_ptr<PhysicsField<TDim>>> physics_fields, SolverType solver_type = SolverType::SparseLU)
            : mesh_(std::move(mesh)), physics_fields_(std::move(physics_fields)), solver_type_(solver_type) {
            dof_manager_ = std::make_unique<DofManager>(*mesh_);
            dof_manager_->buildDofMap(1);
            initializeSystem();
        }

        void assemble() {
            PROFILE_FUNCTION();
            auto sparsity_pattern = dof_manager_->computeSparsityPattern(*mesh_);
            K_global_.reserve(sparsity_pattern.size());
            
            // 组装所有物理场
            for (const auto& physics : physics_fields_) {
                physics->assemble_volume(*mesh_, *dof_manager_, K_global_, F_global_);
                physics->applyNaturalBCs(*mesh_, *dof_manager_, K_global_, F_global_);
            }
        }


        void solve() {
            applyDirichletBCs(); // 在求解前应用Dirichlet边界条件
            LinearSolver solver(solver_type_);
            U_solution_ = solver.solve(K_global_, F_global_);
        }

        // --- 新增的公共访问器 (Getters) ---
        // Exporter 需要通过这些接口来获取数据
        const Mesh& getMesh() const { return *mesh_; }
        const Eigen::VectorXd& getSolution() const { return U_solution_; }
        const DofManager& getDofManager() const { return *dof_manager_; }
        
        // 新增：获取物理场的接口
        const PhysicsField<TDim>& getPhysicsField(size_t index = 0) const { 
            if (index < physics_fields_.size()) {
                return *physics_fields_[index];
            }
            throw std::out_of_range("Physics field index out of range");
        }
        
        size_t getNumPhysicsFields() const { return physics_fields_.size(); }

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

            // 处理通过物理场添加的Dirichlet边界条件
            std::vector<std::pair<int, double>> all_dirichlet_dofs;
            for (const auto& physics : physics_fields_) {
                for (const auto& bc : physics->getBoundaryConditions()) {
                    if (bc->getType() == BCType::Dirichlet) {
                        const auto* dirichlet_bc = static_cast<const DirichletBC<TDim>*>(bc.get());
                        const double bc_value = dirichlet_bc->getValue();
                        
                        try {
                            // 尝试获取边界节点，如果边界不存在则跳过
                            const auto& boundary_nodes = mesh_->getBoundaryNodes(dirichlet_bc->getBoundaryName());
                            for (int node_id : boundary_nodes) {
                                int dof_index = dof_manager_->getNodeDof(node_id, 0);
                                all_dirichlet_dofs.push_back({dof_index, bc_value});
                            }
                        } catch (const std::runtime_error& e) {
                            // 如果边界不存在，我们只输出警告信息但不中断程序
                            std::cerr << "Warning: " << e.what() << std::endl;
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
        
    };
}