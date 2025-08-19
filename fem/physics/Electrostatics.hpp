#pragma once
#include <vector>
#include <memory>
#include <Eigen/Sparse>
#include "../kernels/Kernel.hpp"
#include "../kernels/KernelWrappers.hpp"
#include "../mesh/Mesh.hpp"
#include "../core/DofManager.hpp"
#include "PhysicsField.hpp"

namespace FEM {

    /**
     * @brief 静电场物理场类
     * 
     * 实现静电场问题的求解，计算电势分布
     */
    template<int TDim>
    class Electrostatics : public PhysicsField<TDim> {
    public:
        /**
         * @brief 添加一个计算核
         * @tparam KernelType 核类型
         * @param kernel 计算核指针
         */
        template<typename KernelType>
        void addKernel(std::unique_ptr<KernelType> kernel) {
            // 在内部创建包装器，将派生类指针安全地转换为基类接口指针
            kernels_.push_back(std::make_unique<KernelWrapper<TDim, KernelType::NumNodes>>(std::move(kernel)));
        }

        /**
         * @brief 组装全局刚度矩阵和载荷向量
         * @param mesh 网格对象
         * @param dof_manager 自由度管理器
         * @param K_global 全局刚度矩阵
         * @param F_global 全局载荷向量
         */
        void assemble(const Mesh& mesh, const DofManager& dof_manager,
                      Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& F_global) override {
            for (const auto& elem : mesh.getElements()) {
                for (const auto& kernel_wrapper : kernels_) {
                    kernel_wrapper->assemble_element(*elem, K_global, dof_manager);
                }
            }
        }

        /**
         * @brief 获取物理场名称
         * @return 物理场名称
         */
        std::string getName() const override {
            return "Electrostatics";
        }

    private:
        std::vector<std::unique_ptr<IKernel<TDim>>> kernels_;
    };
}