#pragma once

#include "../core/BoundaryCondition.hpp"
#include "../core/FEFaceValues.hpp" // 使用 FEFaceValues

namespace FEM {
    template<int TDim>
    class NeumannBC : public BoundaryCondition<TDim> {
    public:
        NeumannBC(const std::string& boundary_name, double value)
            : BoundaryCondition<TDim>(boundary_name), value_(value) {}

        void apply(const Mesh& mesh, const DofManager& dof_manager,
                   Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) const override {
            
            const auto& boundary_elements = mesh.getBoundaryElements(this->boundary_name_);

            for (const auto& elem_ptr : boundary_elements) {
                const Element& face_element = *elem_ptr;
                FEFaceValues fe_face_values(face_element, 1, AnalysisType::SCALAR_DIFFUSION);
                Eigen::VectorXd F_elem_bc = Eigen::VectorXd::Zero(face_element.getNumNodes());

                for (size_t q = 0; q < fe_face_values.n_quad_points(); ++q) {
                    fe_face_values.reinit(q);
                    const auto& N = fe_face_values.N();
                    F_elem_bc += N * value_ * fe_face_values.JxW();
                }
                
                std::vector<int> dofs(face_element.getNumNodes());
                for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                    dofs[i] = dof_manager.getNodeDof(face_element.getNodeId(i), 0);
                }

                for (size_t i = 0; i < face_element.getNumNodes(); ++i) {
                    F_global(dofs[i]) += F_elem_bc(i);
                }
            }
        }
        
        BCType getType() const override { return BCType::Neumann; }

    private:
        double value_;
    };
}