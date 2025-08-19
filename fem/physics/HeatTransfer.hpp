#pragma once
#include <vector>
#include <memory>
#include <Eigen/Sparse>
#include "../kernels/Kernel.hpp"
#include "../mesh/Mesh.hpp"
#include "../core/DofManager.hpp"

namespace FEM {

    // 1. 定义一个抽象接口，隐藏模板参数 TNumNodes
    template<int TDim>
    class IKernel {
    public:
        virtual ~IKernel() = default;
        virtual void assemble_element(const Element& element, Eigen::SparseMatrix<double>& K_global, const DofManager& dof_manager) = 0;
    };

    // 2. 实现一个模板化的包装器，它实现了上述接口
    template<int TDim, int TNumNodes>
    class KernelWrapper : public IKernel<TDim> {
    public:
        explicit KernelWrapper(std::unique_ptr<Kernel<TDim, TNumNodes>> kernel) : kernel_(std::move(kernel)) {}

        void assemble_element(const Element& element, Eigen::SparseMatrix<double>& K_global, const DofManager& dof_manager) override {
            // 只为节点数匹配的单元执行组装
            if (element.getNumNodes() != TNumNodes) return;

            Eigen::MatrixXd K_elem_dense = kernel_->compute_element_matrix(element);

            auto dofs = dof_manager.getElementDofs(element);
            for (size_t i = 0; i < dofs.size(); ++i) {
                for (size_t j = 0; j < dofs.size(); ++j) {
                    K_global.coeffRef(dofs[i], dofs[j]) += K_elem_dense(i, j);
                }
            }
        }
    private:
        std::unique_ptr<Kernel<TDim, TNumNodes>> kernel_;
    };

    // 3. 物理场现在存储抽象接口的指针
    template<int TDim>
    class HeatTransfer {
    public:
        // addKernel 是一个模板，可以接受任何类型的 Kernel 子类
        template<typename KernelType>
        void addKernel(std::unique_ptr<KernelType> kernel) {
            // 在内部创建包装器，将派生类指针安全地转换为基类接口指针
            kernels_.push_back(std::make_unique<KernelWrapper<TDim, KernelType::NumNodes>>(std::move(kernel)));
        }

        void assemble(const Mesh& mesh, const DofManager& dof_manager,
                      Eigen::SparseMatrix<double>& K_global, Eigen::VectorXd& /*F_global*/) {
            for (const auto& elem : mesh.getElements()) {
                for (const auto& kernel_wrapper : kernels_) {
                    kernel_wrapper->assemble_element(*elem, K_global, dof_manager);
                }
            }
        }
    private:
        std::vector<std::unique_ptr<IKernel<TDim>>> kernels_;
    };
}