#pragma once

#include <vector>
#include <Eigen/Dense>
#include "ReferenceElement.hpp" // <--- 替换 FiniteElement.hpp
#include "AnalysisTypes.hpp"
#include "../mesh/Element.hpp"

namespace FEM {
    class FEFaceValues {
    public:
        FEFaceValues(const Element& elem, int order, AnalysisType analysis_type)
            : element_(elem),
              // 1. 直接从缓存获取预计算数据
              ref_data_(ReferenceElement::get(elem.getType(), order)),
              analysis_type_(analysis_type) {
            
            const auto& nodes = element_.getNodes();
            const int dim = nodes[0]->getCoords().size();
            const int num_nodes = element_.getNumNodes();
            Eigen::MatrixXd node_coords(dim, num_nodes);
            for(int i=0; i<num_nodes; ++i) {
                node_coords.col(i) = Eigen::Map<const Eigen::VectorXd>(nodes[i]->getCoords().data(), dim);
            }

            all_JxW_.reserve(ref_data_.q_points.size());
            all_dN_dx_.reserve(ref_data_.q_points.size());

            // 2. 遍历预计算好的积分点
            for (size_t q = 0; q < ref_data_.q_points.size(); ++q) {
                // 3. 使用预计算好的参考导数
                const auto& dN_dxi = ref_data_.dN_dxi_values[q];
                
                Eigen::MatrixXd jacobian = node_coords * dN_dxi.transpose();
                double detJ = jacobian.determinant();
                
                // 4. 使用预计算好的积分权重
                all_JxW_.push_back(detJ * ref_data_.q_points[q].weight);
                Eigen::MatrixXd dN_dx = jacobian.inverse() * dN_dxi;
                all_dN_dx_.push_back(dN_dx);
            }
        }

        void reinit(int q_index) { q_point_index_ = q_index; }
        size_t n_quad_points() const { return ref_data_.q_points.size(); }

        // 访问器现在从 ref_data_ 中获取数据
        const Eigen::VectorXd& N() const { return ref_data_.N_values[q_point_index_]; }
        const Eigen::MatrixXd& dN_dx() const { return all_dN_dx_[q_point_index_]; }
        double JxW() const { return all_JxW_[q_point_index_]; }
        
        // 添加shape_value函数以修复CauchyBC中的编译错误
        double shape_value(size_t i, size_t q) const { return ref_data_.N_values[q](i); }

    private:
        const Element& element_;
        // 5. 成员变量改变
        const ReferenceElementData& ref_data_;
        int q_point_index_ = -1;
        AnalysisType analysis_type_;

        std::vector<double> all_JxW_;
        std::vector<Eigen::MatrixXd> all_dN_dx_;
    };
}