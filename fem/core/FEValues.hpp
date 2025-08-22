#pragma once

#include <vector>
#include <Eigen/Dense>
#include "FiniteElement.hpp"
#include "AnalysisTypes.hpp" // <--- 引入分析类型

namespace FEM {
    class FEValues {
    public:
        FEValues(const Element& elem, int order, AnalysisType analysis_type)
            : element_(elem),
              finite_element_(FiniteElement::create(elem.getType(), order)),
              analysis_type_(analysis_type) { // <--- 保存分析类型

            // 对Point单元进行特殊处理
            if (elem.getType() == ElementType::Point) {
                // Point单元不需要计算雅可比矩阵
                all_JxW_.push_back(1.0);
                // 对于0维问题，导数矩阵为空
                all_dN_dx_.push_back(Eigen::MatrixXd(0, 1)); // 0行1列的矩阵
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
            all_JxW_.reserve(finite_element_->getNumQuadPoints());
            all_dN_dx_.reserve(finite_element_->getNumQuadPoints());

            // 对每个积分点计算雅可比矩阵
            for (size_t q = 0; q < finite_element_->getNumQuadPoints(); ++q) {
                const auto& dN_dxi = finite_element_->getShapeFunctionDerivatives(q);
                
                // 对于线单元，创建可能需要节点顺序调整的副本
                Eigen::MatrixXd coords_for_jacobian = node_coords;
                
                // 计算雅可比矩阵
                Eigen::MatrixXd jacobian = coords_for_jacobian * dN_dxi.transpose();
                
                // 检查雅可比行列式
                double detJ = jacobian.determinant();
                
                // 对于1D线单元，如果行列式为负，交换节点顺序重新计算
                if (elem.getType() == ElementType::Line && detJ < 0) {
                    // 交换节点坐标矩阵的列（仅用于当前计算）
                    coords_for_jacobian.col(0).swap(coords_for_jacobian.col(1));
                    
                    // 重新计算雅可比矩阵
                    jacobian = coords_for_jacobian * dN_dxi.transpose();
                    detJ = jacobian.determinant();
                }
                
                if (detJ <= 0) {
                    // 如果雅可比行列式仍然非正，输出调试信息
                    throw std::runtime_error("Jacobian determinant is non-positive: " + std::to_string(detJ) + 
                                           ". Element ID: " + std::to_string(elem.getId()) + 
                                           ", Type: " + std::to_string(static_cast<int>(elem.getType())) +
                                           ", Nodes: " + std::to_string(num_nodes));
                }

                // 存储JxW和dN_dx
                // JxW是雅可比行列式的绝对值乘以积分权重，用于数值积分
                all_JxW_.push_back(std::abs(detJ) * finite_element_->getQuadWeight(q));
                // dN_dx是形函数在物理空间中的梯度，仅使用未加权的雅可比矩阵逆进行计算
                Eigen::MatrixXd dN_dx = jacobian.inverse().transpose() * dN_dxi;
                all_dN_dx_.push_back(dN_dx);
            }
        }
        
        // 新增构造函数：自动根据单元类型选择推荐的积分阶数
        FEValues(const Element& elem, AnalysisType analysis_type)
            : FEValues(elem, FiniteElement::getRecommendedOrder(elem.getType()), analysis_type) {
        }

        void reinit(int q_index) { q_point_index_ = q_index; }
        size_t n_quad_points() const { return finite_element_->getNumQuadPoints(); }

        // 访问器
        const Eigen::VectorXd& N() const { return finite_element_->getShapeFunctions(q_point_index_); }
        const Eigen::MatrixXd& dN_dx() const { return all_dN_dx_[q_point_index_]; }
        double JxW() const { return all_JxW_[q_point_index_]; }

    private:
        // ... (其他成员变量不变) ...
        const Element& element_;
        std::unique_ptr<FiniteElement> finite_element_;
        int q_point_index_ = -1;
        AnalysisType analysis_type_; // <--- 新增

        std::vector<double> all_JxW_;
        std::vector<Eigen::MatrixXd> all_dN_dx_;
    };
}