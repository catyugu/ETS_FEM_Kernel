#pragma once

#include <Eigen/Sparse>
#include <string>
#include <vector>
#include "../mesh/Mesh.hpp"
#include "../mesh/Geometry.hpp"
#include "DofManager.hpp"
#include <complex>

namespace FEM {

    // 边界条件类型的枚举
    enum class BCType {
        Dirichlet,
        Neumann,
        Cauchy
    };

    // 边界条件抽象基类
    template<int TDim, typename TScalar = double>
    class BoundaryCondition {
    public:
        virtual ~BoundaryCondition() = default;

        // 应用边界条件。对于Neumann和Cauchy，此方法会修改K和F。
        // 对于Dirichlet，此方法为空，因为它的应用逻辑是特殊的。
        virtual void apply(const Geometry& geometry, const DofManager& dof_manager,
                           std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const = 0;
                           
        virtual BCType getType() const = 0;
        
        const std::string& getBoundaryName() const { return boundary_name_; }

    protected:
        BoundaryCondition(const std::string& boundary_name) : boundary_name_(boundary_name) {}
        std::string boundary_name_;
    };
}