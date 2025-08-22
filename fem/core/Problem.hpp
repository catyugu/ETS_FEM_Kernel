#pragma once

#include "../mesh/Mesh.hpp"
#include "DofManager.hpp"
#include "../physics/PhysicsField.hpp"
#include "LinearSolver.hpp"
#include "../bcs/DirichletBC.hpp"
#include <Eigen/Sparse>
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <utils/Profiler.hpp>
#include <complex>

namespace FEM {
    template<int TDim, typename TScalar = double>
    class Problem {
    public:
        Problem(std::unique_ptr<Mesh> mesh, std::unique_ptr<PhysicsField<TDim, TScalar>> physics, SolverType solver_type = SolverType::SparseLU)
            : mesh_(std::move(mesh)), solver_type_(solver_type) {
            physics_fields_.push_back(std::move(physics));
            dof_manager_ = std::make_unique<DofManager>(*mesh_);
            dof_manager_->buildDofMap(1);
            initializeSystem();
        }

        Problem(std::unique_ptr<Mesh> mesh, std::vector<std::unique_ptr<PhysicsField<TDim, TScalar>>> physics_fields, SolverType solver_type = SolverType::SparseLU)
            : mesh_(std::move(mesh)), physics_fields_(std::move(physics_fields)), solver_type_(solver_type) {
            dof_manager_ = std::make_unique<DofManager>(*mesh_);
            dof_manager_->buildDofMap(1);
            initializeSystem();
        }

        void assemble() {
            PROFILE_FUNCTION();

            // 创建一个 Triplet 列表来存储非零元
            std::vector<Eigen::Triplet<TScalar>> triplet_list;

            // 预估非零元数量并为其预留空间，以避免多次内存重分配
            auto sparsity_pattern = dof_manager_->computeSparsityPattern(*mesh_);
            triplet_list.reserve(sparsity_pattern.size());

            for (const auto& physics : physics_fields_) {
                // 将 triplet_list 传递给物理场进行填充，而不是 K_global_
                physics->assemble_volume(*mesh_, *dof_manager_, triplet_list, F_global_);
                physics->applyNaturalBCs(*mesh_, *dof_manager_, triplet_list, F_global_);
            }

            // 在所有单元和边界计算完成后，一次性高效构建稀疏矩阵
            K_global_.setFromTriplets(triplet_list.begin(), triplet_list.end());
            applyDirichletBCs();
        }

        void solve() {
            LinearSolver solver(solver_type_);
            U_solution_ = solver.solve(K_global_, F_global_);
        }

        const Mesh& getMesh() const { return *mesh_; }
        const Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& getSolution() const { return U_solution_; }
        const DofManager& getDofManager() const { return *dof_manager_; }

        const PhysicsField<TDim, TScalar>& getPhysicsField(size_t index = 0) const {
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

            std::vector<std::pair<int, TScalar>> all_dirichlet_dofs;
            for (const auto& physics : physics_fields_) {
                for (const auto& bc : physics->getBoundaryConditions()) {
                    if (bc->getType() == BCType::Dirichlet) {
                        const auto* dirichlet_bc = dynamic_cast<const DirichletBC<TDim, TScalar>*>(bc.get());
                        if (dirichlet_bc) {
                            const TScalar bc_value = dirichlet_bc->getValue();
                            try {
                                const auto& boundary_nodes = mesh_->getBoundaryNodes(dirichlet_bc->getBoundaryName());
                                for (int node_id : boundary_nodes) {
                                    int dof_index = dof_manager_->getNodeDof(node_id, 0);
                                    all_dirichlet_dofs.push_back({dof_index, bc_value});
                                }
                            } catch (const std::runtime_error& e) {
                                std::cerr << "Warning: " << e.what() << std::endl;
                            }
                        }
                    }
                }
            }

            if (all_dirichlet_dofs.empty()) return;

            std::vector<int> bc_dofs;
            bc_dofs.reserve(all_dirichlet_dofs.size());
            for(const auto& bc : all_dirichlet_dofs){
                bc_dofs.push_back(bc.first);
            }
            std::sort(bc_dofs.begin(), bc_dofs.end());
            bc_dofs.erase(std::unique(bc_dofs.begin(), bc_dofs.end()), bc_dofs.end());

            for (const auto& bc : all_dirichlet_dofs) {
                int dof_bc = bc.first;
                TScalar val_bc = bc.second;

                for (typename Eigen::SparseMatrix<TScalar>::InnerIterator it(K_global_, dof_bc); it; ++it) {
                    if (!std::binary_search(bc_dofs.begin(), bc_dofs.end(), it.row())) {
                        F_global_(it.row()) -= it.value() * val_bc;
                    }
                }
            }

            for (int dof_bc : bc_dofs) {
                for (typename Eigen::SparseMatrix<TScalar>::InnerIterator it(K_global_, dof_bc); it; ++it) { it.valueRef() = TScalar{0.0}; }
                for (int k = 0; k < K_global_.outerSize(); ++k) {
                    for (typename Eigen::SparseMatrix<TScalar>::InnerIterator it(K_global_, k); it; ++it) {
                        if (it.row() == dof_bc) { it.valueRef() = TScalar{0.0}; }
                    }
                }
            }

            for (const auto& bc : all_dirichlet_dofs) {
                K_global_.coeffRef(bc.first, bc.first) = TScalar{1.0};
                F_global_(bc.first) = bc.second;
            }

            K_global_.prune([](const typename Eigen::SparseMatrix<TScalar>::StorageIndex&,
                               const typename Eigen::SparseMatrix<TScalar>::StorageIndex&,
                               const TScalar& value) {
                // The comparison must be between two double-precision floating-point numbers.
                // std::abs(complex) returns a double (the magnitude).
                return std::abs(value) > 1e-12;
            });
        }

        std::unique_ptr<Mesh> mesh_;
        std::vector<std::unique_ptr<PhysicsField<TDim, TScalar>>> physics_fields_;
        std::unique_ptr<DofManager> dof_manager_;
        SolverType solver_type_;

        Eigen::SparseMatrix<TScalar> K_global_;
        Eigen::Matrix<TScalar, Eigen::Dynamic, 1> F_global_;
        Eigen::Matrix<TScalar, Eigen::Dynamic, 1> U_solution_;
    };
}