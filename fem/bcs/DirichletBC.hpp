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