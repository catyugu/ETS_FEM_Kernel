#pragma once

#include "../core/BoundaryCondition.hpp"
#include "../core/FEFaceValues.hpp"

namespace FEM {
    template<int TDim>
    class CauchyBC : public BoundaryCondition<TDim> {
    public:
        CauchyBC(const std::string& boundary_name, double h_val, double T_inf_val)
            : BoundaryCondition<TDim>(boundary_name), h_(h_val), T_inf_(T_inf_val) {}

        void apply(const Mesh& mesh, const DofManager& dof_manager,
                   Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) const override {
            
            const auto& boundary_elements = mesh.getBoundaryElements(this->boundary_name_);

            for (const auto& elem_ptr : boundary_elements) {
                const Element& face_element = *elem_ptr;
                FEFaceValues fe_face_values(face_element, 1, AnalysisType::SCALAR_DIFFUSION);

                Eigen::MatrixXd K_elem_bc = Eigen::MatrixXd::Zero(face_element.getNumNodes(), face_element.getNumNodes());
                Eigen::VectorXd F_elem_bc = Eigen::VectorXd::Zero(face_element.getNumNodes());

                for (size_t q = 0; q < fe_face_values.n_quad_points(); ++q) {
                    fe_face_values.reinit(q);
                    const auto& N = fe_face_values.N();
                    K_elem_bc += h_ * N * N.transpose() * fe_face_values.JxW();
                    F_elem_bc += h_ * T_inf_ * N * fe_face_values.JxW();
                }

                std::vector<int> dofs(face_element.getNumNodes());
                for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                    dofs[i] = dof_manager.getNodeDof(face_element.getNodeId(i), 0);
                }

                for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                    F_global(dofs[i]) += F_elem_bc(i);
                    for (size_t j = 0; j < face_element.getNumNodes(); ++j) {
                        K_global.coeffRef(dofs[i], dofs[j]) += K_elem_bc(i, j);
                    }
                }
            }
        }
        
        BCType getType() const override { return BCType::Cauchy; }

    private:
        double h_;
        double T_inf_;
    };
}