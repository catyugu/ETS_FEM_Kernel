#pragma once

#include "../core/BoundaryCondition.hpp"
#include "../mesh/Geometry.hpp"
#include "../core/FEValues.hpp"
#include <complex>

namespace FEM {
    template<int TDim, typename TScalar = double>
    class NeumannBC : public BoundaryCondition<TDim, TScalar> {
    public:
        NeumannBC(const std::string& boundary_name, TScalar value)
            : BoundaryCondition<TDim, TScalar>(boundary_name), value_(value) {}

        BCType getType() const override { return BCType::Neumann; }

        void apply(const Geometry& geometry, const DofManager& dof_manager,
                   std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const override {

            std::cout << "Applying Neumann BC with value: " << value_ << std::endl;
            
            const auto& boundary_elements = geometry.getBoundary(this->boundary_name_).getElements();
            for (const auto& elem_ptr : boundary_elements) {
                const Element& face_element = *elem_ptr;
                // 修复：使用正确的FEValues构造函数
                FEValues fe_values(face_element, AnalysisType::SCALAR_DIFFUSION);
                Eigen::Matrix<TScalar, Eigen::Dynamic, 1> F_elem_bc = Eigen::Matrix<TScalar, Eigen::Dynamic, 1>::Zero(face_element.getNumNodes());

                // 对于PointElement，直接添加值，不需要积分
                if (face_element.getType() == ElementType::Point) {
                    for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                        F_elem_bc(i) += value_;
                    }
                } else {
                    // 对于其他类型的边界单元，执行正常的积分计算
                    size_t q_index = 0;
                    for (const auto& q_point : fe_values) {
                        auto JxW = static_cast<TScalar>(q_point.JxW());
                        for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                            // 关键修正：必须是加法。热流是源项。
                            F_elem_bc(i) += value_ * static_cast<TScalar>(fe_values.shape_value(i, q_index)) * JxW;
                        }
                        ++q_index;
                    }
                }

                for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                    int global_dof = dof_manager.getNodeDof(face_element.getNodeId(i), 0);
                    F_global(global_dof) += F_elem_bc(i);
                    std::cout << "Neumann BC: Updating F_global(" << global_dof << ") by " << F_elem_bc(i) << std::endl;
                }
            }
        }

    private:
        TScalar value_;
    };
}