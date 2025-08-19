#pragma once
#include <Eigen/Dense>
#include <stdexcept>

namespace FEM::Utils {

    class ShapeFunctions {
    public:
        // --- 1D Line (no changes) ---
        static void getLineShapeFunctions(int order, double xi, Eigen::VectorXd& N);
        static void getLineShapeFunctionDerivatives(int order, double xi, Eigen::MatrixXd& dN_dxi);

        // --- 2D Quad (NEW) ---
        static void getQuadShapeFunctions(int order, double xi, double eta, Eigen::VectorXd& N) {
            if (order == 1) {
                N.resize(4);
                N << 0.25 * (1.0 - xi) * (1.0 - eta),
                     0.25 * (1.0 + xi) * (1.0 - eta),
                     0.25 * (1.0 + xi) * (1.0 + eta),
                     0.25 * (1.0 - xi) * (1.0 + eta);
            } else throw std::invalid_argument("Quad shape function order > 1 not implemented.");
        }

        static void getQuadShapeFunctionDerivatives(int order, double xi, double eta, Eigen::MatrixXd& dN_dxi) {
            if (order == 1) {
                dN_dxi.resize(2, 4);
                // d/dxi
                dN_dxi.row(0) << -0.25 * (1.0 - eta), 0.25 * (1.0 - eta), 0.25 * (1.0 + eta), -0.25 * (1.0 + eta);
                // d/deta
                dN_dxi.row(1) << -0.25 * (1.0 - xi), -0.25 * (1.0 + xi), 0.25 * (1.0 + xi), 0.25 * (1.0 - xi);
            } else throw std::invalid_argument("Quad shape function derivative order > 1 not implemented.");
        }

        // --- 3D Hex (NEW) ---
        static void getHexShapeFunctions(int order, double xi, double eta, double zeta, Eigen::VectorXd& N) {
            if (order == 1) {
                N.resize(8);
                N << 0.125 * (1-xi)*(1-eta)*(1-zeta), 0.125 * (1+xi)*(1-eta)*(1-zeta),
                     0.125 * (1+xi)*(1+eta)*(1-zeta), 0.125 * (1-xi)*(1+eta)*(1-zeta),
                     0.125 * (1-xi)*(1-eta)*(1+zeta), 0.125 * (1+xi)*(1-eta)*(1+zeta),
                     0.125 * (1+xi)*(1+eta)*(1+zeta), 0.125 * (1-xi)*(1+eta)*(1+zeta);
            } else throw std::invalid_argument("Hex shape function order > 1 not implemented.");
        }

        static void getHexShapeFunctionDerivatives(int order, double xi, double eta, double zeta, Eigen::MatrixXd& dN_dxi) {
            if (order == 1) {
                dN_dxi.resize(3, 8);
                // d/dxi
                dN_dxi.row(0) << -0.125*(1-eta)*(1-zeta), 0.125*(1-eta)*(1-zeta), 0.125*(1+eta)*(1-zeta), -0.125*(1+eta)*(1-zeta),
                                 -0.125*(1-eta)*(1+zeta), 0.125*(1-eta)*(1+zeta), 0.125*(1+eta)*(1+zeta), -0.125*(1+eta)*(1+zeta);
                // d/deta
                dN_dxi.row(1) << -0.125*(1-xi)*(1-zeta), -0.125*(1+xi)*(1-zeta), 0.125*(1+xi)*(1-zeta), 0.125*(1-xi)*(1-zeta),
                                 -0.125*(1-xi)*(1+zeta), -0.125*(1+xi)*(1+zeta), 0.125*(1+xi)*(1+zeta), 0.125*(1-xi)*(1+zeta);
                // d/dzeta
                dN_dxi.row(2) << -0.125*(1-xi)*(1-eta), -0.125*(1+xi)*(1-eta), -0.125*(1+xi)*(1+eta), -0.125*(1-xi)*(1+eta),
                                  0.125*(1-xi)*(1-eta),  0.125*(1+xi)*(1-eta),  0.125*(1+xi)*(1+eta),  0.125*(1-xi)*(1+eta);
            } else throw std::invalid_argument("Hex shape function derivative order > 1 not implemented.");
        }
    };

    // --- We must also provide the implementations for Line, which were missing ---
    inline void ShapeFunctions::getLineShapeFunctions(int order, double xi, Eigen::VectorXd& N) {
        if (order == 1) { N.resize(2); N << (1.0-xi)/2.0, (1.0+xi)/2.0; }
        else throw std::invalid_argument("Line order > 1 not supported.");
    }
    inline void ShapeFunctions::getLineShapeFunctionDerivatives(int order, double, Eigen::MatrixXd& dN_dxi) {
        if (order == 1) { dN_dxi.resize(1,2); dN_dxi << -0.5, 0.5; }
        else throw std::invalid_argument("Line order > 1 not supported.");
    }
}