#pragma once

#include "../core/BoundaryCondition.hpp"
#include "../mesh/Geometry.hpp"
#include "../core/FEFaceValues.hpp"
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

            const auto& boundary_elements = geometry.getBoundary(this->boundary_name_).getElements();
            for (const auto& elem_ptr : boundary_elements) {
                const Element& face_element = *elem_ptr;
                FEFaceValues fe_face_values(face_element, 1, AnalysisType::SCALAR_DIFFUSION);
                Eigen::Matrix<TScalar, Eigen::Dynamic, 1> F_elem_bc = Eigen::Matrix<TScalar, Eigen::Dynamic, 1>::Zero(face_element.getNumNodes());

                for (size_t q = 0; q < fe_face_values.n_quad_points(); ++q) {
                    fe_face_values.reinit(q);
                    auto JxW = static_cast<TScalar>(fe_face_values.JxW());
                    for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                        // 关键修正：必须是加法。热流是源项。
                        F_elem_bc(i) += value_ * static_cast<TScalar>(fe_face_values.shape_value(i, q)) * JxW;
                    }
                }

                for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                    int global_dof = dof_manager.getNodeDof(face_element.getNodeId(i), 0);
                    F_global(global_dof) += F_elem_bc(i);
                }
            }
        }

    private:
        TScalar value_;
    };
}