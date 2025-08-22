#pragma once

#include "../core/BoundaryCondition.hpp"
#include "../core/FEFaceValues.hpp"
#include <complex>

namespace FEM {
    template<int TDim, typename TScalar = double>
    class CauchyBC : public BoundaryCondition<TDim, TScalar> {
    public:
        CauchyBC(const std::string& boundary_name, TScalar h_val, TScalar T_inf_val)
            : BoundaryCondition<TDim, TScalar>(boundary_name), h_(h_val), T_inf_(T_inf_val) {}

        void apply(const Mesh& mesh, const DofManager& dof_manager,
                   std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const override {
            
            const auto& boundary_elements = mesh.getBoundaryElements(this->boundary_name_);

            for (const auto& elem_ptr : boundary_elements) {
                const Element& face_element = *elem_ptr;
                FEFaceValues fe_face_values(face_element, 1, AnalysisType::SCALAR_DIFFUSION);

                Eigen::Matrix<TScalar, Eigen::Dynamic, Eigen::Dynamic> K_elem_bc = Eigen::Matrix<TScalar, Eigen::Dynamic, Eigen::Dynamic>::Zero(face_element.getNumNodes(), face_element.getNumNodes());
                Eigen::Matrix<TScalar, Eigen::Dynamic, 1> F_elem_bc = Eigen::Matrix<TScalar, Eigen::Dynamic, 1>::Zero(face_element.getNumNodes());

                for (size_t q = 0; q < fe_face_values.n_quad_points(); ++q) {
                    fe_face_values.reinit(q);
                    TScalar JxW = static_cast<TScalar>(fe_face_values.JxW());
                    
                    for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                        for (size_t j = 0; j < face_element.getNumNodes(); ++j) {
                            // Cauchy BC: h * ∫(N_i * N_j) dS
                            K_elem_bc(i, j) += h_ * static_cast<TScalar>(fe_face_values.shape_value(i, q) * fe_face_values.shape_value(j, q)) * JxW;
                        }
                        // Cauchy BC: h * T_inf * ∫(N_i) dS
                        F_elem_bc(i) += h_ * T_inf_ * static_cast<TScalar>(fe_face_values.shape_value(i, q)) * JxW;
                    }
                }

                // 将局部矩阵和向量映射到全局系统
                for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                    int global_i = dof_manager.getNodeDof(face_element.getNodeId(i), 0);
                    F_global(global_i) += F_elem_bc(i);
                    for (size_t j = 0; j < face_element.getNumNodes(); ++j) {
                        int global_j = dof_manager.getNodeDof(face_element.getNodeId(j), 0);
                        triplet_list.emplace_back(global_i, global_j, K_elem_bc(i, j));
                    }
                }
            }
        }
        
        BCType getType() const override { return BCType::Cauchy; }

        TScalar getH() const { return h_; }
        TScalar getTInf() const { return T_inf_; }

    private:
        TScalar h_;      // 传热系数
        TScalar T_inf_;  // 环境温度
    };
}