#pragma once

#include "../core/BoundaryCondition.hpp"
#include "../core/FEFaceValues.hpp" // 使用 FEFaceValues
#include <complex>

namespace FEM {
    template<int TDim, typename TScalar = double>
    class NeumannBC : public BoundaryCondition<TDim, TScalar> {
    public:
        NeumannBC(const std::string& boundary_name, TScalar value)
            : BoundaryCondition<TDim, TScalar>(boundary_name), value_(value) {}

        void apply(const Mesh& mesh, const DofManager& dof_manager,
                   std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const override {

            const auto& boundary_elements = mesh.getBoundaryElements(this->boundary_name_);
            if constexpr (TDim == 1) {
                const auto& boundary_nodes = mesh.getBoundaryNodes(this->getBoundaryName());
                for (int node_id : boundary_nodes) {
                    int dof_index = dof_manager.getNodeDof(node_id, 0);
                    // 对于1D的点边界，直接施加集中载荷
                    F_global(dof_index) += value_;
                }
                return;
            }
            for (const auto& elem_ptr : boundary_elements) {
                const Element& face_element = *elem_ptr;
                FEFaceValues fe_face_values(face_element, 1, AnalysisType::SCALAR_DIFFUSION);
                Eigen::Matrix<TScalar, Eigen::Dynamic, 1> F_elem_bc = Eigen::Matrix<TScalar, Eigen::Dynamic, 1>::Zero(face_element.getNumNodes());

                for (size_t q = 0; q < fe_face_values.n_quad_points(); ++q) {
                    fe_face_values.reinit(q);
                    TScalar JxW = static_cast<TScalar>(fe_face_values.JxW());
                    for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                        // Neumann BC: 面积分 ∫(value * N_i) dS
                        // 根据热传导方程的弱形式推导，对于Neumann边界条件(-k * dT/dx = q₀)，
                        // 力向量的贡献应该是 +q₀
                        F_elem_bc(i) += value_ * static_cast<TScalar>(fe_face_values.shape_value(i, q)) * JxW;
                    }
                }

                // 将局部向量映射到全局向量
                for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                    int global_dof = dof_manager.getNodeDof(face_element.getNodeId(i), 0);
                    F_global(global_dof) += F_elem_bc(i);
                }
            }
        }

        BCType getType() const override { return BCType::Neumann; }

        TScalar getValue() const { return value_; }

    private:
        TScalar value_;
    };
}