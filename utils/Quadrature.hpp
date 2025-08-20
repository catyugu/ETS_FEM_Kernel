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
                double a = 0.58541020;
                double b = 0.13819660;
                double w = 1.0/24.0;
                return {
                        {(Eigen::Vector3d() << a, b, b).finished(), w},
                        {(Eigen::Vector3d() << b, a, b).finished(), w},
                        {(Eigen::Vector3d() << b, b, a).finished(), w},
                        {(Eigen::Vector3d() << b, b, b).finished(), w}
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
