#pragma once

#include <vector>
#include <memory>
#include <Eigen/Dense>
#include "../mesh/Element.hpp"
#include "../utils/Quadrature.hpp"
#include "../utils/ShapeFunctions.hpp"

namespace FEM {
    /**
     * @brief 有限单元基类
     * 
     * 该类封装了特定单元类型的形函数、积分规则和插值逻辑，
     * 是连接单元几何与物理计算的桥梁
     */
    class FiniteElement {
    public:
        virtual ~FiniteElement() = default;

        /**
         * @brief 获取单元类型
         * @return 单元类型枚举值
         */
        virtual ElementType getType() const = 0;

        /**
         * @brief 获取单元节点数
         * @return 节点数量
         */
        virtual int getNumNodes() const = 0;

        /**
         * @brief 获取积分点数量
         * @return 积分点数量
         */
        virtual size_t getNumQuadPoints() const = 0;

        /**
         * @brief 获取指定积分点的形函数值
         * @param q_index 积分点索引
         * @return 形函数值向量
         */
        virtual const Eigen::VectorXd& getShapeFunctions(size_t q_index) const = 0;

        /**
         * @brief 获取指定积分点的形函数导数（参考坐标系）
         * @param q_index 积分点索引
         * @return 形函数导数矩阵
         */
        virtual const Eigen::MatrixXd& getShapeFunctionDerivatives(size_t q_index) const = 0;

        /**
         * @brief 获取指定积分点的积分权重
         * @param q_index 积分点索引
         * @return 积分权重
         */
        virtual double getQuadWeight(size_t q_index) const = 0;

        /**
         * @brief 创建指定类型和阶次的有限单元
         * @param type 单元类型
         * @param order 积分阶次
         * @return 有限单元指针
         */
        static std::unique_ptr<FiniteElement> create(ElementType type, int order);
    };

    /**
     * @brief 模板化的有限单元实现类
     * 
     * 该模板类提供了具体单元类型的实现
     */
    template<ElementType TType, int TNumNodes>
    class FiniteElementImpl : public FiniteElement {
    public:
        /**
         * @brief 构造函数
         * @param order 积分阶次
         */
        explicit FiniteElementImpl(int order) : order_(order) {
            // 预计算形函数和积分点
            precompute();
        }

        ElementType getType() const override {
            return TType;
        }

        int getNumNodes() const override {
            return TNumNodes;
        }

        size_t getNumQuadPoints() const override {
            return q_points_.size();
        }

        const Eigen::VectorXd& getShapeFunctions(size_t q_index) const override {
            return N_values_.at(q_index);
        }

        const Eigen::MatrixXd& getShapeFunctionDerivatives(size_t q_index) const override {
            return dN_dxi_values_.at(q_index);
        }

        double getQuadWeight(size_t q_index) const override {
            return q_points_.at(q_index).weight;
        }

    private:
        /**
         * @brief 预计算形函数和积分点
         */
        void precompute() {
            // 获取积分点
            if constexpr (TType == ElementType::Line) {
                q_points_ = Utils::Quadrature::getLineQuadrature(order_);
            } else if constexpr (TType == ElementType::Triangle) {
                q_points_ = Utils::Quadrature::getTriangleQuadrature(order_);
            } else if constexpr (TType == ElementType::Quadrilateral) {
                q_points_ = Utils::Quadrature::getQuadrilateralQuadrature(order_);
            } else if constexpr (TType == ElementType::Tetrahedron) {
                q_points_ = Utils::Quadrature::getTetrahedronQuadrature(order_);
            } else if constexpr (TType == ElementType::Hexahedron) {
                q_points_ = Utils::Quadrature::getHexahedronQuadrature(order_);
            }

            // 计算形函数值和导数
            for (const auto& qp : q_points_) {
                Eigen::VectorXd N;
                Eigen::MatrixXd dN_dxi;
                
                if constexpr (TType == ElementType::Line) {
                    Utils::ShapeFunctions::getLineShapeFunctions(order_, qp.point(0), N);
                    Utils::ShapeFunctions::getLineShapeFunctionDerivatives(order_, qp.point(0), dN_dxi);
                } else if constexpr (TType == ElementType::Triangle) {
                    Utils::ShapeFunctions::getTriangleShapeFunctions(order_, qp.point(0), qp.point(1), N);
                    Utils::ShapeFunctions::getTriangleShapeFunctionDerivatives(order_, qp.point(0), qp.point(1), dN_dxi);
                } else if constexpr (TType == ElementType::Quadrilateral) {
                    Utils::ShapeFunctions::getQuadShapeFunctions(order_, qp.point(0), qp.point(1), N);
                    Utils::ShapeFunctions::getQuadShapeFunctionDerivatives(order_, qp.point(0), qp.point(1), dN_dxi);
                } else if constexpr (TType == ElementType::Tetrahedron) {
                    Utils::ShapeFunctions::getTetShapeFunctions(order_, qp.point(0), qp.point(1), qp.point(2), N);
                    Utils::ShapeFunctions::getTetShapeFunctionDerivatives(order_, qp.point(0), qp.point(1), qp.point(2), dN_dxi);
                } else if constexpr (TType == ElementType::Hexahedron) {
                    Utils::ShapeFunctions::getHexShapeFunctions(order_, qp.point(0), qp.point(1), qp.point(2), N);
                    Utils::ShapeFunctions::getHexShapeFunctionDerivatives(order_, qp.point(0), qp.point(1), qp.point(2), dN_dxi);
                }
                
                N_values_.push_back(N);
                dN_dxi_values_.push_back(dN_dxi);
            }
        }

        int order_;
        std::vector<Utils::QuadraturePoint> q_points_;
        std::vector<Eigen::VectorXd> N_values_;
        std::vector<Eigen::MatrixXd> dN_dxi_values_;
    };
    
    // 实现FiniteElement的create函数
    inline std::unique_ptr<FiniteElement> FiniteElement::create(ElementType type, int order) {
        switch (type) {
            case ElementType::Line:
                return std::make_unique<FiniteElementImpl<ElementType::Line, 2>>(order);
            case ElementType::Triangle:
                return std::make_unique<FiniteElementImpl<ElementType::Triangle, 3>>(order);
            case ElementType::Quadrilateral:
                return std::make_unique<FiniteElementImpl<ElementType::Quadrilateral, 4>>(order);
            case ElementType::Tetrahedron:
                return std::make_unique<FiniteElementImpl<ElementType::Tetrahedron, 4>>(order);
            case ElementType::Hexahedron:
                return std::make_unique<FiniteElementImpl<ElementType::Hexahedron, 8>>(order);
            default:
                throw std::runtime_error("Unsupported element type");
        }
    }
}