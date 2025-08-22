#pragma once

#include <vector>
#include <Eigen/Dense>
#include "ReferenceElement.hpp"
#include "AnalysisTypes.hpp"

namespace FEM {
    class FEValues {
    public:
        FEValues(const Element& elem, int order, AnalysisType analysis_type)
            : element_(elem),
              ref_data_(ReferenceElement::get(elem.getType(), order)),
              analysis_type_(analysis_type) {
            PROFILE_FUNCTION();

            // 对Point单元进行特殊处理
            if (elem.getType() == ElementType::Point) {
                all_JxW_.push_back(1.0);
                all_dN_dx_.push_back(Eigen::MatrixXd(0, 1));
                return;
            }

            // 检查节点数量
            const auto& nodes = element_.getNodes();
            if (nodes.empty()) {
                throw std::runtime_error("Element has no nodes");
            }

            // 获取维度和节点数
            const int dim = nodes[0]->getCoords().size();
            const int num_nodes = element_.getNumNodes();
            
            // 检查节点坐标维度一致性
            for (const auto& node : nodes) {
                if (node->getCoords().size() != dim) {
                    throw std::runtime_error("Inconsistent node coordinate dimensions");
                }
            }

            // 构建节点坐标矩阵
            Eigen::MatrixXd node_coords(dim, num_nodes);
            for(int i = 0; i < num_nodes; ++i) {
                node_coords.col(i) = Eigen::Map<const Eigen::VectorXd>(nodes[i]->getCoords().data(), dim);
            }

            // 预分配空间
            all_JxW_.reserve(ref_data_.q_points.size());
            all_dN_dx_.reserve(ref_data_.q_points.size());

            // 遍历预计算好的积分点
            for (size_t q = 0; q < ref_data_.q_points.size(); ++q) {
                // 使用预计算好的参考导数
                const auto& dN_dxi = ref_data_.dN_dxi_values[q];
                
                Eigen::MatrixXd coords_for_jacobian = node_coords;
                
                Eigen::MatrixXd jacobian = coords_for_jacobian * dN_dxi.transpose();
                
                double detJ = jacobian.determinant();
                
                if (elem.getType() == ElementType::Line && detJ < 0) {
                    coords_for_jacobian.col(0).swap(coords_for_jacobian.col(1));
                    jacobian = coords_for_jacobian * dN_dxi.transpose();
                    detJ = jacobian.determinant();
                }

                // 使用预计算好的积分权重
                all_JxW_.push_back(std::abs(detJ) * ref_data_.q_points[q].weight);
                Eigen::MatrixXd dN_dx = jacobian.inverse().transpose() * dN_dxi;
                all_dN_dx_.push_back(dN_dx);
            }
        }
        
        // 新增构造函数：自动根据单元类型选择推荐的积分阶数
        FEValues(const Element& elem, AnalysisType analysis_type)
            : FEValues(elem, getRecommendedOrder(elem.getType()), analysis_type) {
        }

        void reinit(int q_index) { q_point_index_ = q_index; }
        size_t n_quad_points() const { return ref_data_.q_points.size(); }

        // 访问器现在从 ref_data_ 中获取数据
        const Eigen::VectorXd& N() const { return ref_data_.N_values[q_point_index_]; }
        const Eigen::MatrixXd& dN_dx() const { return all_dN_dx_[q_point_index_]; }
        double JxW() const { return all_JxW_[q_point_index_]; }

    private:
        // ... (其他成员变量不变) ...
        const Element& element_;
        const ReferenceElementData& ref_data_;
        int q_point_index_ = -1;
        AnalysisType analysis_type_; // <--- 新增

        std::vector<double> all_JxW_;
        std::vector<Eigen::MatrixXd> all_dN_dx_;
        
    private:
        // 从FiniteElement迁移的方法
        static int getRecommendedOrder(ElementType type) {
            switch (type) {
                case ElementType::Point: return 1;
                case ElementType::Line: return 1;
                case ElementType::Triangle: return 1;
                case ElementType::Quadrilateral: return 1;
                case ElementType::Tetrahedron: return 1;
                case ElementType::Hexahedron: return 1;
                default: throw std::runtime_error("Unsupported element type");
            }
        }
    };
}