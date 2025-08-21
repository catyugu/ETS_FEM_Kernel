#pragma once

#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include <Eigen/IterativeLinearSolvers>
#include <stdexcept>
#include "utils/SimpleLogger.hpp"

namespace FEM {

    enum class SolverType {
        SparseLU,
        ConjugateGradient
    };

    class LinearSolver {
    public:
        /**
         * @brief 构造函数，指定求解器类型
         * @param type 求解器类型
         */
        explicit LinearSolver(SolverType type = SolverType::SparseLU) : type_(type) {}

        /**
         * @brief 使用指定的求解器求解线性系统 Ax = b
         * @param A 稀疏矩阵 (系数矩阵)
         * @param b 向量 (右端项)
         * @return Eigen::VectorXd 求解得到的解向量 x
         */
        Eigen::VectorXd solve(const Eigen::SparseMatrix<double>& A, const Eigen::VectorXd& b) const {
            switch (type_) {
                case SolverType::SparseLU:
                    return solveWithSparseLU(A, b);
                case SolverType::ConjugateGradient:
                    return solveWithConjugateGradient(A, b);
                default:
                    throw std::runtime_error("Unknown solver type.");
            }
        }

    private:
        /**
         * @brief 使用 Eigen 的稀疏 LU 分解求解线性系统 Ax = b
         * @param A 稀疏矩阵 (系数矩阵)
         * @param b 向量 (右端项)
         * @return Eigen::VectorXd 求解得到的解向量 x
         */
        Eigen::VectorXd solveWithSparseLU(const Eigen::SparseMatrix<double>& A, const Eigen::VectorXd& b) const {
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
            ::Utils::Logger::instance().info("Linear system solved successfully with SparseLU.");
            return x;
        }

        /**
         * @brief 使用共轭梯度法求解线性系统 Ax = b
         * @param A 稀疏矩阵 (系数矩阵)
         * @param b 向量 (右端项)
         * @return Eigen::VectorXd 求解得到的解向量 x
         */
        Eigen::VectorXd solveWithConjugateGradient(const Eigen::SparseMatrix<double>& A, const Eigen::VectorXd& b) const {
            // 创建共轭梯度求解器
            Eigen::ConjugateGradient<Eigen::SparseMatrix<double>, Eigen::Lower|Eigen::Upper> solver;

            // 设置求解器参数
            solver.setMaxIterations(1000);
            solver.setTolerance(1e-6);

            // 分解矩阵 A
            solver.compute(A);
            if (solver.info() != Eigen::Success) {
                // 分解失败
                throw std::runtime_error("ConjugateGradient decomposition failed.");
            }

            // 求解 Ax = b
            Eigen::VectorXd x = solver.solve(b);
            if (solver.info() != Eigen::Success) {
                // 求解失败
                throw std::runtime_error("ConjugateGradient solving failed.");
            }

            ::Utils::Logger::instance().info("Linear system solved successfully with ConjugateGradient.");
            ::Utils::Logger::instance().info("CG iterations: " + std::to_string(solver.iterations()) +
                                           ", estimated error: " + std::to_string(solver.error()));
            return x;
        }

        SolverType type_;
    };

} // namespace FEM