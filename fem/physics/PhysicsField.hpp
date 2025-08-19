#pragma once
#include <Eigen/Sparse>
#include "../mesh/Mesh.hpp"
#include "../core/DofManager.hpp"

namespace FEM {

    /**
     * @brief 物理场抽象基类
     * 
     * 定义所有物理场都需要实现的接口，为多物理场耦合提供统一接口
     */
    template<int TDim>
    class PhysicsField {
    public:
        virtual ~PhysicsField() = default;

        /**
         * @brief 组装全局刚度矩阵和载荷向量
         * @param mesh 网格对象
         * @param dof_manager 自由度管理器
         * @param K_global 全局刚度矩阵
         * @param F_global 全局载荷向量
         */
        virtual void assemble(const Mesh& mesh, const DofManager& dof_manager,
                              Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) = 0;

        /**
         * @brief 应用边界条件
         * @param mesh 网格对象
         * @param dof_manager 自由度管理器
         * @param K_global 全局刚度矩阵
         * @param F_global 全局载荷向量
         */
        virtual void applyBoundaryConditions(const Mesh& mesh, const DofManager& dof_manager,
                                             Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) {
            // 默认实现为空，具体物理场可以重写
        }

        /**
         * @brief 获取物理场名称
         * @return 物理场名称
         */
        virtual std::string getName() const = 0;
    };

} // namespace FEM