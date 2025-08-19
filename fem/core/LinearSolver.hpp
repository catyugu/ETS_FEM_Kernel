#pragma once

#include <Eigen/Sparse>
#include <Eigen/SparseLU> // 使用稀疏 LU 分解作为示例
#include <stdexcept>
#include "utils/SimpleLogger.hpp"

namespace FEM {

    class LinearSolver {
    public:
        /**
         * @brief 使用 Eigen 的稀疏 LU 分解求解线性系统 Ax = b
         * @param A 稀疏矩阵 (系数矩阵)
         * @param b 向量 (右端项)
         * @return Eigen::VectorXd 求解得到的解向量 x
         */
        Eigen::VectorXd solve(const Eigen::SparseMatrix<double>& A, const Eigen::VectorXd& b) const {
            // 创建一个 LU 分解求解器
            Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;

            // 分解矩阵 A
            solver.compute(A);
            if (solver.info() != Eigen::Success) {
                // 分解失败，矩阵可能是奇异的
                throw std::runtime_error("Eigen::SparseLU decomposition failed.");
            }

            // 求解 Ax = b
            Eigen::VectorXd x = solver.solve(b);
            if (solver.info() != Eigen::Success) {
                // 求解失败
                throw std::runtime_error("Eigen::SparseLU solving failed.");
            }
            
            Utils::Logger::instance().info("Linear system solved successfully.");
            return x;
        }
    };

} // namespace FEM