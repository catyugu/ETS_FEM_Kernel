#pragma once

#include "../core/BoundaryCondition.hpp"
#include "../mesh/Geometry.hpp"
#include <complex>

namespace FEM {
    template<int TDim, typename TScalar = double>
    class DirichletBC : public BoundaryCondition<TDim, TScalar> {
    public:
        // --- 构造函数更新 ---
        DirichletBC(const std::string& variable_name, const std::string& boundary_name, TScalar value)
            : BoundaryCondition<TDim, TScalar>(variable_name, boundary_name, BCType::Dirichlet), value_(value) {}

        // 留空，由 Problem 类统一处理
        void apply(const Geometry& geometry, const DofManager& dof_manager,
                   std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const override {}
        
        BCType getType() const override { return BCType::Dirichlet; }

        TScalar getValue() const { return value_; }

    private:
        TScalar value_;
    };
}