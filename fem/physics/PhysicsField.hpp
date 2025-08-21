#pragma once
#include <Eigen/Sparse>
#include <vector>
#include <memory>
#include "../mesh/Mesh.hpp"
#include "../core/DofManager.hpp"
#include "../core/BoundaryCondition.hpp" // 引入新头文件

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
         * @brief 组装全局刚度矩阵和载荷向量（体积项）
         * @param mesh 网格对象
         * @param dof_manager 自由度管理器
         * @param K_global 全局刚度矩阵
         * @param F_global 全局载荷向量
         */
        virtual void assemble_volume(const Mesh& mesh, const DofManager& dof_manager,
                              Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) = 0;

        /**
         * @brief 应用"自然"边界条件 (Neumann, Cauchy)
         * @param mesh 网格对象
         * @param dof_manager 自由度管理器
         * @param K_global 全局刚度矩阵
         * @param F_global 全局载荷向量
         */
        void applyNaturalBCs(const Mesh& mesh, const DofManager& dof_manager,
                             Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) const {
            for (const auto& bc : boundary_conditions_) {
                if (bc->getType() != BCType::Dirichlet) {
                    bc->apply(mesh, dof_manager, K_global, F_global);
                }
            }
        }
        
        void addBoundaryCondition(std::unique_ptr<BoundaryCondition<TDim>> bc) {
            boundary_conditions_.push_back(std::move(bc));
        }

        const std::vector<std::unique_ptr<BoundaryCondition<TDim>>>& getBoundaryConditions() const {
            return boundary_conditions_;
        }

        /**
         * @brief 获取物理场名称
         * @return 物理场名称
         */
        virtual std::string getName() const = 0;

    private:
        std::vector<std::unique_ptr<BoundaryCondition<TDim>>> boundary_conditions_;
    };

} // namespace FEM