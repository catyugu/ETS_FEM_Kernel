#pragma once
#include <Eigen/Dense>
#include <stdexcept>

namespace FEM::Utils {

    class ShapeFunctions {
    public:
        // --- 1D: Line Element (2 nodes) ---
        // 节点顺序: (-1), (+1) in local coordinates
        static void getLineShapeFunctions(int order, double xi, Eigen::VectorXd& N) {
            if (order == 1) {
                N.resize(2);
                N << 0.5 * (1.0 - xi),
                     0.5 * (1.0 + xi);
            } else {
                throw std::invalid_argument("Line shape function order > 1 not implemented.");
            }
        }

        static void getLineShapeFunctionDerivatives(int order, double xi, Eigen::MatrixXd& dN_dxi) {
            if (order == 1) {
                dN_dxi.resize(1, 2);
                dN_dxi << -0.5, 0.5;
            } else {
                throw std::invalid_argument("Line shape function derivative order > 1 not implemented.");
            }
        }

        // --- 2D: Triangle Element (3 nodes) ---
        // 节点顺序: (0,0), (1,0), (0,1) in local coordinates
        static void getTriangleShapeFunctions(int order, double xi, double eta, Eigen::VectorXd& N) {
            if (order == 1) {
                N.resize(3);
                N << 1.0 - xi - eta, // N0 @ (0,0) is 1
                     xi,            // N1 @ (1,0) is 1
                     eta;           // N2 @ (0,1) is 1
            } else {
                throw std::invalid_argument("Triangle shape function order > 1 not implemented.");
            }
        }

        static void getTriangleShapeFunctionDerivatives(int order, double xi, double eta, Eigen::MatrixXd& dN_dxi) {
            if (order == 1) {
                dN_dxi.resize(2, 3);
                // Derivatives w.r.t. xi and eta for N0, N1, N2
                dN_dxi.row(0) << -1.0, 1.0, 0.0; // d/dxi
                dN_dxi.row(1) << -1.0, 0.0, 1.0; // d/deta
            } else {
                throw std::invalid_argument("Triangle shape function derivative order > 1 not implemented.");
            }
        }

        // --- 2D: Quadrilateral Element (4 nodes) ---
        // 节点顺序: (-1,-1), (1,-1), (1,1), (-1,1) in local coordinates (counter-clockwise)
        static void getQuadShapeFunctions(int order, double xi, double eta, Eigen::VectorXd& N) {
            if (order == 1) {
                N.resize(4);
                N << 0.25 * (1.0 - xi) * (1.0 - eta), // N0 @ (-1,-1) is 1
                     0.25 * (1.0 + xi) * (1.0 - eta), // N1 @ ( 1,-1) is 1
                     0.25 * (1.0 + xi) * (1.0 + eta), // N2 @ ( 1, 1) is 1
                     0.25 * (1.0 - xi) * (1.0 + eta); // N3 @ (-1, 1) is 1
            } else {
                throw std::invalid_argument("Quad shape function order > 1 not implemented.");
            }
        }

        static void getQuadShapeFunctionDerivatives(int order, double xi, double eta, Eigen::MatrixXd& dN_dxi) {
            if (order == 1) {
                dN_dxi.resize(2, 4);
                // d/dxi
                dN_dxi.row(0) << -0.25 * (1.0 - eta), 0.25 * (1.0 - eta), 0.25 * (1.0 + eta), -0.25 * (1.0 + eta);
                // d/deta
                dN_dxi.row(1) << -0.25 * (1.0 - xi), -0.25 * (1.0 + xi), 0.25 * (1.0 + xi), 0.25 * (1.0 - xi);
            } else {
                throw std::invalid_argument("Quad shape function derivative order > 1 not implemented.");
            }
        }

        // --- 3D: Tetrahedron Element (4 nodes) ---
        // 节点顺序: (0,0,0), (1,0,0), (0,1,0), (0,0,1) in local coordinates
        static void getTetShapeFunctions(int order, double xi, double eta, double zeta, Eigen::VectorXd& N) {
            if (order == 1) {
                N.resize(4);
                N << 1.0 - xi - eta - zeta, // N0 @ (0,0,0) is 1
                     xi,                    // N1 @ (1,0,0) is 1
                     eta,                   // N2 @ (0,1,0) is 1
                     zeta;                  // N3 @ (0,0,1) is 1
            } else {
                throw std::invalid_argument("Tet shape function order > 1 not implemented.");
            }
        }

        static void getTetShapeFunctionDerivatives(int order, double xi, double eta, double zeta, Eigen::MatrixXd& dN_dxi) {
            if (order == 1) {
                dN_dxi.resize(3, 4);
                // Derivatives w.r.t. xi, eta, zeta for N0, N1, N2, N3
                dN_dxi.row(0) << -1.0, 1.0, 0.0, 0.0; // d/dxi
                dN_dxi.row(1) << -1.0, 0.0, 1.0, 0.0; // d/deta
                dN_dxi.row(2) << -1.0, 0.0, 0.0, 1.0; // d/dzeta
            } else {
                throw std::invalid_argument("Tet shape function derivative order > 1 not implemented.");
            }
        }

        // --- 3D: Hexahedron Element (8 nodes) ---
        // 节点顺序: (-1,-1,-1), (1,-1,-1), (1,1,-1), (-1,1,-1), (-1,-1,1), (1,-1,1), (1,1,1), (-1,1,1)
        static void getHexShapeFunctions(int order, double xi, double eta, double zeta, Eigen::VectorXd& N) {
            if (order == 1) {
                N.resize(8);
                N << 0.125 * (1.0 - xi) * (1.0 - eta) * (1.0 - zeta), // N0 @ (-1,-1,-1)
                     0.125 * (1.0 + xi) * (1.0 - eta) * (1.0 - zeta), // N1 @ ( 1,-1,-1)
                     0.125 * (1.0 + xi) * (1.0 + eta) * (1.0 - zeta), // N2 @ ( 1, 1,-1)
                     0.125 * (1.0 - xi) * (1.0 + eta) * (1.0 - zeta), // N3 @ (-1, 1,-1)
                     0.125 * (1.0 - xi) * (1.0 - eta) * (1.0 + zeta), // N4 @ (-1,-1, 1)
                     0.125 * (1.0 + xi) * (1.0 - eta) * (1.0 + zeta), // N5 @ ( 1,-1, 1)
                     0.125 * (1.0 + xi) * (1.0 + eta) * (1.0 + zeta), // N6 @ ( 1, 1, 1)
                     0.125 * (1.0 - xi) * (1.0 + eta) * (1.0 + zeta); // N7 @ (-1, 1, 1)
            } else {
                throw std::invalid_argument("Hex shape function order > 1 not implemented.");
            }
        }

        static void getHexShapeFunctionDerivatives(int order, double xi, double eta, double zeta, Eigen::MatrixXd& dN_dxi) {
            if (order == 1) {
                dN_dxi.resize(3, 8);
                // d/dxi
                dN_dxi.row(0) << -0.125 * (1 - eta) * (1 - zeta), 0.125 * (1 - eta) * (1 - zeta), 0.125 * (1 + eta) * (1 - zeta), -0.125 * (1 + eta) * (1 - zeta),
                                 -0.125 * (1 - eta) * (1 + zeta), 0.125 * (1 - eta) * (1 + zeta), 0.125 * (1 + eta) * (1 + zeta), -0.125 * (1 + eta) * (1 + zeta);
                // d/deta
                dN_dxi.row(1) << -0.125 * (1 - xi) * (1 - zeta), -0.125 * (1 + xi) * (1 - zeta), 0.125 * (1 + xi) * (1 - zeta), 0.125 * (1 - xi) * (1 - zeta),
                                 -0.125 * (1 - xi) * (1 + zeta), -0.125 * (1 + xi) * (1 + zeta), 0.125 * (1 + xi) * (1 + zeta), 0.125 * (1 - xi) * (1 + zeta);
                // d/dzeta
                dN_dxi.row(2) << -0.125 * (1 - xi) * (1 - eta), -0.125 * (1 + xi) * (1 - eta), -0.125 * (1 + xi) * (1 + eta), -0.125 * (1 - xi) * (1 + eta),
                                  0.125 * (1 - xi) * (1 - eta),  0.125 * (1 + xi) * (1 - eta),  0.125 * (1 + xi) * (1 + eta),  0.125 * (1 - xi) * (1 + eta);
            } else {
                throw std::invalid_argument("Hex shape function derivative order > 1 not implemented.");
            }
        }
    };
}