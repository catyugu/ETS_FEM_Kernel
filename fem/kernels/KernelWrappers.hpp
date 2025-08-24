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
        virtual void assemble_element(const Element& element, std::vector<Eigen::Triplet<TScalar>>& triplet_list, const DofManager& dof_manager) = 0;
    };

    /**
     * @brief 内核包装器
     * @tparam TDim 问题维度
     */
    template<int TDim, typename TScalar = double>
    class KernelWrapper : public IKernel<TDim, TScalar> {
    public:
        KernelWrapper(std::unique_ptr<Kernel<TDim, TScalar>> kernel, const std::string& var_name) 
            : kernel_(std::move(kernel)), var_name_(var_name) {}

        void assemble_element(const Element& element, std::vector<Eigen::Triplet<TScalar>>& triplet_list, const DofManager& dof_manager) override {
            // 计算单元矩阵
            auto K_elem = kernel_->compute_element_matrix(element);
            
            // 获取单元节点数
            const int num_nodes = element.getNumNodes();

            // 获取单元的自由度索引
            std::vector<int> dof_indices(num_nodes);
            for (int i = 0; i < num_nodes; ++i) {
                dof_indices[i] = dof_manager.getNodeDof(var_name_, element.getNodeId(i), 0);
            }

            // 组装到Triplet列表
            for (int i = 0; i < num_nodes; ++i) {
                for (int j = 0; j < num_nodes; ++j) {
                    triplet_list.emplace_back(dof_indices[i], dof_indices[j], K_elem(i, j));
                }
            }
        }

    private:
        std::unique_ptr<Kernel<TDim, TScalar>> kernel_;
        std::string var_name_;
    };
}