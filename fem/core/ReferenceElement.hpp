#pragma once

#include <vector>
#include <map>
#include <Eigen/Dense>
#include "../utils/Quadrature.hpp"
#include "../utils/ShapeFunctions.hpp"
#include "../mesh/Element.hpp"

namespace FEM {
    // 预计算并缓存的数据结构体
    struct ReferenceElementData {
        std::vector<Utils::QuadraturePoint> q_points;
        std::vector<Eigen::VectorXd> N_values;
        std::vector<Eigen::MatrixXd> dN_dxi_values;
    };

    // 缓存管理器类
    class ReferenceElement {
    public:
        // 获取指定类型和阶次单元的缓存数据
        static const ReferenceElementData& get(ElementType type, int order) {
            auto key = std::make_pair(type, order);
            if (cache_.find(key) == cache_.end()) {
                cache_[key] = precompute(type, order);
            }
            return cache_.at(key);
        }

    private:
        // 执行一次性的预计算
        static ReferenceElementData precompute(ElementType type, int order) {
            ReferenceElementData data;
            if (type == ElementType::Line) {
                data.q_points = Utils::Quadrature::getLineQuadrature(order);
            } else if (type == ElementType::Triangle) {
                data.q_points = Utils::Quadrature::getTriangleQuadrature(order);
            } else if (type == ElementType::Quadrilateral) {
                data.q_points = Utils::Quadrature::getQuadrilateralQuadrature(order);
            } else if (type == ElementType::Tetrahedron) {
                data.q_points = Utils::Quadrature::getTetrahedronQuadrature(order);
            } else if (type == ElementType::Hexahedron) {
                data.q_points = Utils::Quadrature::getHexahedronQuadrature(order);
            }

            for (const auto& qp : data.q_points) {
                Eigen::VectorXd N;
                Eigen::MatrixXd dN_dxi;
                if (type == ElementType::Line) {
                    Utils::ShapeFunctions::getLineShapeFunctions(order, qp.point(0), N);
                    Utils::ShapeFunctions::getLineShapeFunctionDerivatives(order, qp.point(0), dN_dxi);
                } else if (type == ElementType::Triangle) {
                    Utils::ShapeFunctions::getTriangleShapeFunctions(order, qp.point(0), qp.point(1), N);
                    Utils::ShapeFunctions::getTriangleShapeFunctionDerivatives(order, qp.point(0), qp.point(1), dN_dxi);
                } else if (type == ElementType::Quadrilateral) {
                    Utils::ShapeFunctions::getQuadShapeFunctions(order, qp.point(0), qp.point(1), N);
                    Utils::ShapeFunctions::getQuadShapeFunctionDerivatives(order, qp.point(0), qp.point(1), dN_dxi);
                } else if (type == ElementType::Tetrahedron) {
                    Utils::ShapeFunctions::getTetShapeFunctions(order, qp.point(0), qp.point(1), qp.point(2), N);
                    Utils::ShapeFunctions::getTetShapeFunctionDerivatives(order, qp.point(0), qp.point(1), qp.point(2), dN_dxi);
                } else if (type == ElementType::Hexahedron) {
                    Utils::ShapeFunctions::getHexShapeFunctions(order, qp.point(0), qp.point(1), qp.point(2), N);
                    Utils::ShapeFunctions::getHexShapeFunctionDerivatives(order, qp.point(0), qp.point(1), qp.point(2), dN_dxi);
                } else if (type == ElementType::Point) {
                    N.resize(1);
                    N << 1.0;
                    dN_dxi.resize(0, 1); // 0维导数
                }
                data.N_values.push_back(N);
                data.dN_dxi_values.push_back(dN_dxi);
            }
            return data;
        }
        inline static std::map<std::pair<ElementType, int>, ReferenceElementData> cache_;
    };

}