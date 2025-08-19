#pragma once

#include <vector>
#include <Eigen/Dense>
#include "ReferenceElement.hpp"
#include "AnalysisTypes.hpp" // <--- 引入分析类型

namespace FEM {
    class FEValues {
    public:
        FEValues(const Element& elem, int order, AnalysisType analysis_type)
            : element_(elem),
              ref_data_(ReferenceElement::get(elem.getType(), order)),
              analysis_type_(analysis_type) { // <--- 保存分析类型

            // ... (雅可比计算部分与之前相同) ...
            const auto& nodes = element_.getNodes();
            const int dim = nodes[0]->getCoords().size();
            const int num_nodes = element_.getNumNodes();
            Eigen::MatrixXd node_coords(dim, num_nodes);
            for(int i=0; i<num_nodes; ++i) {
                node_coords.col(i) = Eigen::Map<const Eigen::VectorXd>(nodes[i]->getCoords().data(), dim);
            }

            all_JxW_.reserve(ref_data_.q_points.size());
            all_dN_dx_.reserve(ref_data_.q_points.size());
            all_B_matrices_.reserve(ref_data_.q_points.size()); // <--- 为B矩阵预留空间

            for (size_t q = 0; q < ref_data_.q_points.size(); ++q) {
                const auto& dN_dxi = ref_data_.dN_dxi_values[q];
                Eigen::MatrixXd jacobian = node_coords * dN_dxi.transpose();
                double detJ = jacobian.determinant();
                if (detJ <= 0) throw std::runtime_error("Jacobian determinant is non-positive.");

                all_JxW_.push_back(detJ * ref_data_.q_points[q].weight);
                Eigen::MatrixXd dN_dx = jacobian.inverse() * dN_dxi;
                all_dN_dx_.push_back(dN_dx);

                // --- 新增：构建并缓存B矩阵 ---
                all_B_matrices_.push_back(buildBMatrix(dN_dx));
            }
        }

        void reinit(int q_index) { q_point_index_ = q_index; }
        size_t n_quad_points() const { return ref_data_.q_points.size(); }

        // --- 访问器 ---
        const Eigen::VectorXd& N() const { return ref_data_.N_values[q_point_index_]; }
        const Eigen::MatrixXd& dN_dx() const { return all_dN_dx_[q_point_index_]; }
        double JxW() const { return all_JxW_[q_point_index_]; }
        const Eigen::MatrixXd& B() const { return all_B_matrices_[q_point_index_]; } // <--- 获取B矩阵

    private:
        // 根据分析类型构建B矩阵
        Eigen::MatrixXd buildBMatrix(const Eigen::MatrixXd& dN_dx) {
            if (analysis_type_ == AnalysisType::SCALAR_DIFFUSION) {
                // 对于标量扩散，B矩阵就是梯度算子
                return dN_dx;
            }
            // 未来可添加其他类型的B矩阵
            throw std::runtime_error("Unsupported analysis type for B-matrix construction.");
        }

        // ... (其他成员变量不变) ...
        const Element& element_;
        const ReferenceElementData& ref_data_;
        int q_point_index_ = -1;
        AnalysisType analysis_type_; // <--- 新增

        std::vector<double> all_JxW_;
        std::vector<Eigen::MatrixXd> all_dN_dx_;
        std::vector<Eigen::MatrixXd> all_B_matrices_; // <--- 新增
    };
}