#pragma once

#include <vector>
#include <Eigen/Dense>
#include "ReferenceElement.hpp"
#include "AnalysisTypes.hpp"
#include "QuadraturePoint.hpp" // 引入新头文件

namespace FEM {
    class FEValues {
    public:
        friend class QuadraturePoint; // 允许 QuadraturePoint 访问其数据

        // 添加一个默认构造函数，以便在循环外创建实例
        FEValues() = default;

        // 让现有构造函数调用 reinit，保持接口兼容
        FEValues(const Element &elem, int order, AnalysisType analysis_type) {
            reinit(elem, order, analysis_type);
        }

        FEValues(const Element &elem, AnalysisType analysis_type)
            : FEValues(elem, getRecommendedOrder(elem.getType()), analysis_type) {
        }
        
        // 创建 reinit 函数，包含所有核心计算逻辑
        void reinit(const Element& elem, int order, AnalysisType analysis_type) {
            PROFILE_FUNCTION(); // 我们可以保留对 reinit 本身的性能分析

            // 更新指向的单元和参考单元数据
            element_ = &elem;
            ref_data_ = &ReferenceElement::get(elem.getType(), order);
            analysis_type_ = analysis_type;
            
            // 清空向量，但保留已分配的内存以供下次使用
            all_JxW_.clear();
            all_dN_dx_.clear();

            // 对Point单元进行特殊处理
            if (elem.getType() == ElementType::Point) {
                all_JxW_.push_back(1.0);
                all_dN_dx_.emplace_back(0, 1);
                return;
            }
            
            const auto &nodes = element_->getNodes();
            if (nodes.empty()) {
                throw std::runtime_error("Element has no nodes");
            }

            const int dim = nodes[0]->getCoords().size();
            const int num_nodes = element_->getNumNodes();

            for (const auto &node: nodes) {
                if (node->getCoords().size() != dim) {
                    throw std::runtime_error("Inconsistent node coordinate dimensions");
                }
            }
            
            Eigen::MatrixXd node_coords(dim, num_nodes);
            for (int i = 0; i < num_nodes; ++i) {
                node_coords.col(i) = Eigen::Map<const Eigen::VectorXd>(nodes[i]->getCoords().data(), dim);
            }
            
            // 如果容量不足，则重新分配。多数情况下将复用现有容量。
            if (all_JxW_.capacity() < ref_data_->q_points.size()) {
                all_JxW_.reserve(ref_data_->q_points.size());
            }
            if (all_dN_dx_.capacity() < ref_data_->q_points.size()) {
                 all_dN_dx_.reserve(ref_data_->q_points.size());
            }

            for (size_t q = 0; q < ref_data_->q_points.size(); ++q) {
                const auto &dN_dxi = ref_data_->dN_dxi_values[q];
                Eigen::MatrixXd coords_for_jacobian = node_coords;
                Eigen::MatrixXd jacobian = coords_for_jacobian * dN_dxi.transpose();
                double detJ = jacobian.determinant();

                if (elem.getType() == ElementType::Line && detJ < 0) {
                    coords_for_jacobian.col(0).swap(coords_for_jacobian.col(1));
                    jacobian = coords_for_jacobian * dN_dxi.transpose();
                    detJ = jacobian.determinant();
                }

                all_JxW_.push_back(std::abs(detJ) * ref_data_->q_points[q].weight);
                Eigen::MatrixXd dN_dx = jacobian.inverse().transpose() * dN_dxi;
                all_dN_dx_.push_back(dN_dx);
            }
        }
        
        // 添加重载版本，自动选择推荐的积分阶数
        void reinit(const Element& elem, AnalysisType analysis_type) {
            reinit(elem, getRecommendedOrder(elem.getType()), analysis_type);
        }

        size_t n_quad_points() const { return ref_data_->q_points.size(); } // 使用 ->

        // --- 迭代器实现 ---
        class Iterator {
        public:
            Iterator(const FEValues &fe_values, size_t q_index)
                : q_point_(fe_values, q_index) {
            }

            QuadraturePoint &operator*() { return q_point_; }
            const QuadraturePoint &operator*() const { return q_point_; }

            Iterator &operator++() {
                q_point_.q_index_++;
                return *this;
            }

            bool operator!=(const Iterator &other) const {
                return q_point_.q_index_ != other.q_point_.q_index_;
            }

        private:
            QuadraturePoint q_point_;
        };

        Iterator begin() const { return Iterator(*this, 0); }
        Iterator end() const { return Iterator(*this, n_quad_points()); }
        
        // 添加shape_value函数以修复CauchyBC中的编译错误
        double shape_value(size_t i, size_t q) const { return ref_data_->N_values[q](i); } // 使用 ->

    private:
        // 将成员变量从引用改为指针
        const Element* element_ = nullptr;
        const ReferenceElementData* ref_data_ = nullptr;
        AnalysisType analysis_type_;

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
                default: return 1;
            }
        }

        // 更新私有访问器以使用指针
        const Eigen::VectorXd &getN(size_t q_index) const { return ref_data_->N_values[q_index]; }
        const Eigen::MatrixXd &get_dN_dx(size_t q_index) const { return all_dN_dx_[q_index]; }
        double get_JxW(size_t q_index) const { return all_JxW_[q_index]; }
    };

    // --- QuadraturePoint 成员函数的实现 ---
    inline const Eigen::VectorXd &QuadraturePoint::N() const { return fe_values_.getN(q_index_); }
    inline const Eigen::MatrixXd &QuadraturePoint::dN_dx() const { return fe_values_.get_dN_dx(q_index_); }
    inline double QuadraturePoint::JxW() const { return fe_values_.get_JxW(q_index_); }
}