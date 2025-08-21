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
    template<int TDim, typename TScalar = double>
    class Electrostatics : public PhysicsField<TDim, TScalar> {
    public:
        using KernelBase = Kernel<TDim, TScalar>; // 移除硬编码的节点数

        /**
         * @brief 添加一个计算核
         * @tparam KernelType 核类型
         * @param kernel 计算核指针
         */
        template<typename KernelType>
        void addKernel(std::unique_ptr<KernelType> kernel) {
            // 在内部创建包装器，将派生类指针安全地转换为基类接口指针
            kernels_.push_back(std::make_unique<KernelWrapper<TDim, TScalar>>(std::move(kernel)));
        }

        /**
         * @brief 组装全局刚度矩阵和载荷向量（体积项）
         * @param mesh 网格对象
         * @param dof_manager 自由度管理器
         * @param K_global 全局刚度矩阵
         * @param F_global 全局载荷向量
         */
        void assemble_volume(const Mesh& mesh, const DofManager& dof_manager,
                      Eigen::SparseMatrix<TScalar>& K_global, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) override {
            // 只对适当的单元类型进行组装
            for (const auto& elem : mesh.getElements()) {
                // 根据问题维度过滤单元类型
                bool should_assemble = shouldAssembleElement(*elem, TDim);
                if (should_assemble) {
                    for (const auto& kernel_wrapper : kernels_) {
                        kernel_wrapper->assemble_element(*elem, K_global, dof_manager);
                    }
                }
            }
        }

        /**
         * @brief 判断单元是否应该参与组装
         * @param element 单元对象
         * @param problem_dim 问题维度
         * @return 是否应该组装
         */
        bool shouldAssembleElement(const Element& element, int problem_dim) const {
            ElementType type = element.getType();
            
            // 根据问题维度决定哪些单元参与域内组装
            switch (problem_dim) {
                case 1:
                    // 一维问题，线单元参与组装
                    return type == ElementType::Line;
                    
                case 2:
                    // 二维问题，面单元参与组装
                    return type == ElementType::Triangle || type == ElementType::Quadrilateral;
                    
                case 3:
                    // 三维问题，体单元参与组装
                    return type == ElementType::Tetrahedron || type == ElementType::Hexahedron;
                    
                default:
                    // 默认情况，点单元通常不参与域内组装
                    return type == ElementType::Line || 
                           type == ElementType::Triangle || 
                           type == ElementType::Quadrilateral ||
                           type == ElementType::Tetrahedron || 
                           type == ElementType::Hexahedron;
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
        std::vector<std::unique_ptr<IKernel<TDim, TScalar>>> kernels_;
    };
}