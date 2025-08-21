#pragma once
#include <vector>
#include <memory>
#include <Eigen/Sparse>
#include "../mesh/Element.hpp"
#include "../core/DofManager.hpp"
#include "Kernel.hpp"

namespace FEM {

    /**
     * @brief 内核抽象接口
     * 
     * 定义所有内核都需要实现的接口，隐藏模板参数 TNumNodes
     */
    template<int TDim>
    class IKernel {
    public:
        virtual ~IKernel() = default;
        virtual void assemble_element(const Element& element, Eigen::SparseMatrix<double>& K_global, const DofManager& dof_manager) = 0;
    };

    /**
     * @brief 内核包装器
     * 
     * 模板化的包装器，实现 IKernel 接口，用于包装具体的 Kernel 实现
     */
    template<int TDim, int TNumNodes>
    class KernelWrapper : public IKernel<TDim> {
    public:
        explicit KernelWrapper(std::unique_ptr<Kernel<TDim, TNumNodes>> kernel) : kernel_(std::move(kernel)) {}

        void assemble_element(const Element& element, Eigen::SparseMatrix<double>& K_global, const DofManager& dof_manager) override {
            // 只为节点数匹配的单元执行组装
            if (element.getNumNodes() != TNumNodes) return;

            Eigen::MatrixXd K_elem_dense = kernel_->compute_element_matrix(element);

            // 获取单元自由度
            std::vector<int> dofs;
            const auto& nodes = element.getNodes();
            dofs.reserve(nodes.size());
            
            for (const auto& node : nodes) {
                int dof = dof_manager.getNodeDof(node->getId(), 0);
                if (dof >= 0) dofs.push_back(dof);
            }
            
            for (size_t i = 0; i < dofs.size(); ++i) {
                for (size_t j = 0; j < dofs.size(); ++j) {
                    K_global.coeffRef(dofs[i], dofs[j]) += K_elem_dense(i, j);
                }
            }
        }
    private:
        std::unique_ptr<Kernel<TDim, TNumNodes>> kernel_;
    };

} // namespace FEM