#pragma once

#include <vector>
#include <Eigen/Dense>
#include "ReferenceElement.hpp"
#include "../mesh/Mesh.hpp" // 包含 Element.hpp

namespace FEM {

    class FEValues {
    public:
        FEValues(const Element& elem, int order)
            : element_(elem),
              ref_data_(ReferenceElement::get(elem.getType(), order)) {

            const auto& nodes = element_.getNodes();
            const int dim = nodes[0]->getCoords().size();
            const int num_nodes = element_.getNumNodes();

            Eigen::MatrixXd node_coords(dim, num_nodes);
            for(int i=0; i<num_nodes; ++i) {
                node_coords.col(i) = Eigen::Map<const Eigen::VectorXd>(nodes[i]->getCoords().data(), dim);
            }

            all_JxW_.reserve(ref_data_.q_points.size());
            all_dN_dx_.reserve(ref_data_.q_points.size());

            for (size_t q = 0; q < ref_data_.q_points.size(); ++q) {
                const auto& dN_dxi = ref_data_.dN_dxi_values[q];
                Eigen::MatrixXd jacobian = node_coords * dN_dxi.transpose();
                double detJ = jacobian.determinant();
                if (detJ <= 0) throw std::runtime_error("Jacobian determinant is non-positive.");

                all_JxW_.push_back(detJ * ref_data_.q_points[q].weight);
                all_dN_dx_.push_back(jacobian.inverse() * dN_dxi);
            }
        }

        void reinit(int q_index) {
            q_point_index_ = q_index;
        }

        size_t n_quad_points() const { return ref_data_.q_points.size(); }

        const Eigen::VectorXd& N() const {
            return ref_data_.N_values[q_point_index_];
        }

        const Eigen::MatrixXd& dN_dx() const {
            return all_dN_dx_[q_point_index_];
        }

        double JxW() const {
            return all_JxW_[q_point_index_];
        }

    private:
        const Element& element_;
        const ReferenceElementData& ref_data_;
        int q_point_index_ = -1;

        std::vector<double> all_JxW_;
        std::vector<Eigen::MatrixXd> all_dN_dx_;
    };
}