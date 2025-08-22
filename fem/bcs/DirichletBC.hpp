#pragma once

#include "../core/BoundaryCondition.hpp"
#include <complex>

namespace FEM {
    template<int TDim, typename TScalar = double>
    class DirichletBC : public BoundaryCondition<TDim, TScalar> {
    public:
        DirichletBC(const std::string& boundary_name, TScalar value)
            : BoundaryCondition<TDim, TScalar>(boundary_name), value_(value) {}

        // 留空，由 Problem 类统一处理
        void apply(const Mesh& mesh, const DofManager& dof_manager,
                   std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const override {}
        
        BCType getType() const override { return BCType::Dirichlet; }

        TScalar getValue() const { return value_; }

    private:
        TScalar value_;
    };
}