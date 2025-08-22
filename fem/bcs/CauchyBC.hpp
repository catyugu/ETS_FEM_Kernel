#pragma once

#include "../core/BoundaryCondition.hpp"
#include "../mesh/Geometry.hpp"
#include "../core/FEValues.hpp"
#include <complex>

namespace FEM {
    template<int TDim, typename TScalar = double>
    class CauchyBC : public BoundaryCondition<TDim, TScalar> {
    public:
        CauchyBC(const std::string& boundary_name, TScalar h_val, TScalar T_inf_val)
            : BoundaryCondition<TDim, TScalar>(boundary_name), h_(h_val), T_inf_(T_inf_val) {}

        BCType getType() const override { return BCType::Cauchy; }

        void apply(const Geometry& geometry, const DofManager& dof_manager,
                   std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const override {

            std::cout << "Applying Cauchy BC with h: " << h_ << ", T_inf: " << T_inf_ << std::endl;
            
            const auto& boundary_elements = geometry.getBoundary(this->boundary_name_).getElements();

            for (const auto& elem_ptr : boundary_elements) {
                const Element& face_element = *elem_ptr;
                // 修复：使用正确的FEValues构造函数
                FEValues fe_values(face_element, AnalysisType::SCALAR_DIFFUSION);

                Eigen::Matrix<TScalar, Eigen::Dynamic, Eigen::Dynamic> K_elem_bc = Eigen::Matrix<TScalar, Eigen::Dynamic, Eigen::Dynamic>::Zero(face_element.getNumNodes(), face_element.getNumNodes());
                Eigen::Matrix<TScalar, Eigen::Dynamic, 1> F_elem_bc = Eigen::Matrix<TScalar, Eigen::Dynamic, 1>::Zero(face_element.getNumNodes());

                // 对于PointElement，直接添加值，不需要积分
                if (face_element.getType() == ElementType::Point) {
                    for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                        for (size_t j = 0; j < face_element.getNumNodes(); ++j) {
                            K_elem_bc(i, j) += h_;
                        }
                        // 关键修正：必须是加法。环境温度是源项。
                        F_elem_bc(i) += h_ * T_inf_;
                    }
                } else {
                    // 对于其他类型的边界单元，执行正常的积分计算
                    size_t q_index = 0;
                    for (const auto& q_point : fe_values) {
                        auto JxW = static_cast<TScalar>(q_point.JxW());

                        for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                            for (size_t j = 0; j < face_element.getNumNodes(); ++j) {
                                K_elem_bc(i, j) += h_ * static_cast<TScalar>(fe_values.shape_value(i, q_index) * fe_values.shape_value(j, q_index)) * JxW;
                            }
                            // 关键修正：必须是加法。环境温度是源项。
                            F_elem_bc(i) += h_ * T_inf_ * static_cast<TScalar>(fe_values.shape_value(i, q_index)) * JxW;
                        }
                        ++q_index;
                    }
                }

                for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                    int global_i = dof_manager.getNodeDof(face_element.getNodeId(i), 0);
                    F_global(global_i) += F_elem_bc(i);
                    std::cout << "Cauchy BC: Updating F_global(" << global_i << ") by " << F_elem_bc(i) << std::endl;
                    for (size_t j = 0; j < face_element.getNumNodes(); ++j) {
                        int global_j = dof_manager.getNodeDof(face_element.getNodeId(j), 0);
                        triplet_list.emplace_back(global_i, global_j, K_elem_bc(i, j));
                    }
                }
            }
        }

    private:
        TScalar h_;
        TScalar T_inf_;
    };
}