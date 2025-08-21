#pragma once

#include "Kernel.hpp"
#include "../mesh/Element.hpp"
#include "../core/DofManager.hpp"
#include <Eigen/Sparse>
#include <memory>
#include <complex>

namespace FEM {

    /**
     * @brief 内核抽象接口
     * @tparam TDim 问题维度
     */
    template<int TDim, typename TScalar = double>
    class IKernel {
    public:
        virtual ~IKernel() = default;
        virtual void assemble_element(const Element& element, Eigen::SparseMatrix<TScalar>& K_global, const DofManager& dof_manager) = 0;
    };

    /**
     * @brief 内核包装器
     * @tparam TDim 问题维度
     * @tparam TNumNodes 单元节点数
     */
    template<int TDim, int TNumNodes, typename TScalar = double>
    class KernelWrapper : public IKernel<TDim, TScalar> {
    public:
        explicit KernelWrapper(std::unique_ptr<Kernel<TDim, TNumNodes, TScalar>> kernel) : kernel_(std::move(kernel)) {}

        void assemble_element(const Element& element, Eigen::SparseMatrix<TScalar>& K_global, const DofManager& dof_manager) override {
            // 检查单元节点数是否与内核匹配
            if (element.getNodeIds().size() != TNumNodes) {
                return;
            }

            // 计算单元矩阵
            auto K_elem = kernel_->compute_element_matrix(element);

            // 获取单元的自由度索引
            std::vector<int> dof_indices(TNumNodes);
            for (size_t i = 0; i < TNumNodes; ++i) {
                dof_indices[i] = dof_manager.getNodeDof(element.getNodeIds()[i], 0);
            }

            // 组装到全局矩阵
            for (size_t i = 0; i < TNumNodes; ++i) {
                for (size_t j = 0; j < TNumNodes; ++j) {
                    K_global.coeffRef(dof_indices[i], dof_indices[j]) += K_elem(i, j);
                }
            }
        }

    private:
        std::unique_ptr<Kernel<TDim, TNumNodes, TScalar>> kernel_;
    };
}