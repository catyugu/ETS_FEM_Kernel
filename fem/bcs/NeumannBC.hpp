#pragma once

#include "../core/BoundaryCondition.hpp"
#include "../core/FEFaceValues.hpp" // 使用 FEFaceValues
#include <complex>

namespace FEM {
    template<int TDim, typename TScalar = double>
    class NeumannBC : public BoundaryCondition<TDim, TScalar> {
    public:
        /**
         * @brief 构造一个诺伊曼边界条件 (指定热流密度).
         * @param boundary_name 边界的名称.
         * @param value 热流密度的值 (q₀).
         * @note 遵循行业标准约定:
         * - value > 0 表示热量流出 (outward flux, 冷却).
         * - value < 0 表示热量流入 (inward flux, 加热).
         */
        NeumannBC(const std::string& boundary_name, TScalar value)
            : BoundaryCondition<TDim, TScalar>(boundary_name), value_(value) {}

        /**
         * @brief 返回边界条件的类型.
         * @return BCType::NEUMANN
         */
        BCType getType() const override { return BCType::Neumann; }

        void apply(const Mesh& mesh, const DofManager& dof_manager,
                   std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const override {

            const auto& boundary_elements = mesh.getBoundaryElements(this->boundary_name_);
            for (const auto& elem_ptr : boundary_elements) {
                const Element& face_element = *elem_ptr;
                FEFaceValues fe_face_values(face_element, 1, AnalysisType::SCALAR_DIFFUSION);
                Eigen::Matrix<TScalar, Eigen::Dynamic, 1> F_elem_bc = Eigen::Matrix<TScalar, Eigen::Dynamic, 1>::Zero(face_element.getNumNodes());

                for (size_t q = 0; q < fe_face_values.n_quad_points(); ++q) {
                    fe_face_values.reinit(q);
                    auto JxW = static_cast<TScalar>(fe_face_values.JxW());
                    for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                        // Neumann BC: 边界积分项为 ∫(-value * N_i) dS
                        // 'value_' 代表向外的热流密度 (q_outward)。
                        // 根据热传导方程的弱形式推导, 力向量的贡献应该是 ∫(-q_outward * N_i)dS。
                        F_elem_bc(i) -= value_ * static_cast<TScalar>(fe_face_values.shape_value(i, q)) * JxW;
                    }
                }

                // 将局部向量映射到全局向量
                for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                    int global_dof = dof_manager.getNodeDof(face_element.getNodeId(i), 0);
                    F_global(global_dof) += F_elem_bc(i);
                }
            }
        }

    private:
        TScalar value_; // 热流密度 (q₀)
    };
}