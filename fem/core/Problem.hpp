#pragma once

#include "../mesh/Mesh.hpp"
#include "DofManager.hpp"
#include "../physics/PhysicsField.hpp"
#include "LinearSolver.hpp"
#include <Eigen/Sparse>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>

// 注意：我们已经移除了 #include "../io/Exporter.hpp"

namespace FEM {
    template<int TDim>
    class Problem {
    public:
        Problem(std::unique_ptr<Mesh> mesh, std::unique_ptr<PhysicsField<TDim>> physics, SolverType solver_type = SolverType::SparseLU)
            : mesh_(std::move(mesh)), physics_(std::move(physics)), dof_manager_(*mesh_), solver_type_(solver_type) {
            dof_manager_.buildDofMap(1);
            size_t num_dofs = dof_manager_.getNumDofs();
            K_global_.resize(num_dofs, num_dofs);
            F_global_.resize(num_dofs);
            U_solution_.resize(num_dofs);
            F_global_.setZero();
        }

        void assemble() {
            // 预计算稀疏模式以提高性能
            auto sparsity_pattern = dof_manager_.computeSparsityPattern(*mesh_);
            K_global_.reserve(sparsity_pattern.size());
            
            physics_->assemble(*mesh_, dof_manager_, K_global_, F_global_);
        }

        void addDirichletBC(int node_id, double value) {
            int dof_index = dof_manager_.getNodeDof(node_id, 0);
            dirichlet_bcs_.push_back({dof_index, value});
        }

        void applyBCs() {
            std::vector<int> bc_dofs;
            for(const auto& bc : dirichlet_bcs_){
                bc_dofs.push_back(bc.first);
            }
            std::sort(bc_dofs.begin(), bc_dofs.end());

            for (const auto& bc : dirichlet_bcs_) {
                int dof_index = bc.first;
                double value = bc.second;
                for (int j = 0; j < K_global_.rows(); ++j) {
                    if (!std::binary_search(bc_dofs.begin(), bc_dofs.end(), j)) {
                         F_global_(j) -= K_global_.coeff(j, dof_index) * value;
                    }
                }
            }

            for (const auto& bc : dirichlet_bcs_) {
                int dof_index = bc.first;
                double value = bc.second;

                for (int k = 0; k < K_global_.outerSize(); ++k) {
                    for (Eigen::SparseMatrix<double>::InnerIterator it(K_global_, k); it; ++it) {
                        if (it.row() == dof_index || it.col() == dof_index) {
                            it.valueRef() = 0.0;
                        }
                    }
                }
                K_global_.coeffRef(dof_index, dof_index) = 1.0;
                F_global_(dof_index) = value;
            }
            K_global_.makeCompressed();
        }

        void solve() {
            LinearSolver solver(solver_type_);
            U_solution_ = solver.solve(K_global_, F_global_);
        }

        void printResults() const {
            std::cout << "Node ID\tTemperature (K)" << std::endl;
            std::cout << "-------\t-----------------" << std::endl;
            for (size_t i = 0; i < mesh_->getNodes().size(); ++i) {
                int dof = dof_manager_.getNodeDof(i, 0);
                std::cout << i << "\t" << U_solution_(dof) << std::endl;
            }
        }

        // --- 新增的公共访问器 (Getters) ---
        // Exporter 需要通过这些接口来获取数据
        const Mesh& getMesh() const { return *mesh_; }
        const Eigen::VectorXd& getSolution() const { return U_solution_; }
        const PhysicsField<TDim>& getPhysicsField() const { return *physics_; }
        const DofManager& getDofManager() const { return dof_manager_; }

    private:
        std::unique_ptr<Mesh> mesh_;
        std::unique_ptr<PhysicsField<TDim>> physics_;
        DofManager dof_manager_;
        SolverType solver_type_;

        Eigen::SparseMatrix<double> K_global_;
        Eigen::VectorXd F_global_;
        Eigen::VectorXd U_solution_;
        std::vector<std::pair<int, double>> dirichlet_bcs_;
    };
}