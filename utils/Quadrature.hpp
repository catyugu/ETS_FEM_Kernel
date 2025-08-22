#pragma once
#include <vector>
#include <Eigen/Dense>
#include <stdexcept>
#include <cmath>

#include "Profiler.hpp"

namespace FEM::Utils {

    struct QuadraturePoint {
        Eigen::VectorXd point;
        double weight;
    };

    class Quadrature {
    public:
        static std::vector<QuadraturePoint> getLineQuadrature(int order) {
            if (order == 1) { // 2-point Gauss
                return {
                    {(Eigen::Vector<double, 1>() << -1.0/sqrt(3.0)).finished(), 1.0},
                    {(Eigen::Vector<double, 1>() <<  1.0/sqrt(3.0)).finished(), 1.0}
                };
            }
            throw std::invalid_argument("Line quadrature order > 1 not implemented.");
        }

        static std::vector<QuadraturePoint> getTriangleQuadrature(int order) {
            if (order == 1) {
                // 1点积分规则，权重应为参考单元面积 (0.5)
                return {
                    {(Eigen::Vector2d() << 1.0/3.0, 1.0/3.0).finished(), 0.5}
                };
            } else if (order == 2) {
                // 3点积分规则，总权重为0.5
                const double weight = 0.5 / 3.0; // 每个点的权重为 面积/3
                double p1 = 1.0/6.0;
                double p2 = 2.0/3.0;
                return {
                    {(Eigen::Vector2d() << p1, p1).finished(), weight},
                    {(Eigen::Vector2d() << p2, p1).finished(), weight},
                    {(Eigen::Vector2d() << p1, p2).finished(), weight}
                };
            }
            throw std::invalid_argument("Triangle quadrature order > 2 not implemented.");
        }

        // --- NEW: Quadrilateral Quadrature (2x2 Gauss) ---
        static std::vector<QuadraturePoint> getQuadrilateralQuadrature(int order) {
            if (order == 1) {
                double pt = 1.0 / sqrt(3.0);
                return {
                    {(Eigen::Vector2d() << -pt, -pt).finished(), 1.0},
                    {(Eigen::Vector2d() <<  pt, -pt).finished(), 1.0},
                    {(Eigen::Vector2d() <<  pt,  pt).finished(), 1.0},
                    {(Eigen::Vector2d() << -pt,  pt).finished(), 1.0}
                };
            }
            throw std::invalid_argument("Quadrilateral quadrature order > 1 not implemented.");
        }

        static std::vector<QuadraturePoint> getTetrahedronQuadrature(int order) {
            if (order == 1) {
                // 1点积分，权重为参考单元体积 (1/6)
                return {
                    {(Eigen::Vector3d() << 0.25, 0.25, 0.25).finished(), 1.0/6.0}
                };
            }
            if (order == 2) {
                // 4点积分，总权重为 1/6
                const double weight = (1.0 / 6.0) / 4.0; // 每个点的权重为 体积/4
                double a = 0.5854101966249685;
                double b = 0.1381966011250105;
                return {
                        {(Eigen::Vector3d() << a, b, b).finished(), weight},
                        {(Eigen::Vector3d() << b, a, b).finished(), weight},
                        {(Eigen::Vector3d() << b, b, a).finished(), weight},
                        {(Eigen::Vector3d() << b, b, b).finished(), weight}
                };
            }
            throw std::invalid_argument("Tetrahedron quadrature order > 1 not implemented.");
        }

        // --- NEW: Hexahedron Quadrature (2x2x2 Gauss) ---
        static std::vector<QuadraturePoint> getHexahedronQuadrature(int order) {
            if (order == 1) {
                double pt = 1.0 / sqrt(3.0);
                return {
                    {(Eigen::Vector3d() << -pt, -pt, -pt).finished(), 1.0},
                    {(Eigen::Vector3d() <<  pt, -pt, -pt).finished(), 1.0},
                    {(Eigen::Vector3d() <<  pt,  pt, -pt).finished(), 1.0},
                    {(Eigen::Vector3d() << -pt,  pt, -pt).finished(), 1.0},
                    {(Eigen::Vector3d() << -pt, -pt,  pt).finished(), 1.0},
                    {(Eigen::Vector3d() <<  pt, -pt,  pt).finished(), 1.0},
                    {(Eigen::Vector3d() <<  pt,  pt,  pt).finished(), 1.0},
                    {(Eigen::Vector3d() << -pt,  pt,  pt).finished(), 1.0}
                };
            }
            throw std::invalid_argument("Hexahedron quadrature order > 1 not implemented.");
        }
    };
}