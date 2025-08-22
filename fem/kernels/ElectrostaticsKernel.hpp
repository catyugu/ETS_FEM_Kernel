#pragma once
#include "Kernel.hpp"
#include "../materials/Material.hpp"
#include "../core/FEValues.hpp"
#include <complex>

namespace FEM {
    template<int TDim, typename TScalar = double>
    class ElectrostaticsKernel : public Kernel<TDim, TScalar> {
    public:
        using MatrixType = typename Kernel<TDim, TScalar>::MatrixType;

        explicit ElectrostaticsKernel(const Material& material) : mat_(material) {}

        MatrixType compute_element_matrix(const Element& element) override {
            // 在运行时获取节点数
            const int num_nodes = element.getNumNodes();

            MatrixType K_elem = MatrixType::Zero(num_nodes, num_nodes);

            // 使用新的FEValues构造函数，自动选择合适的积分阶数
            FEValues fe_values(element, AnalysisType::SCALAR_DIFFUSION);
            const MaterialProperty& eps_prop = mat_.getProperty("permittivity");

            for (const auto& q_point : fe_values) {
                double eps = eps_prop.evaluate();

                // --- 正确的实现 ---
                const auto& B = q_point.dN_dx();
                auto D = static_cast<TScalar>(eps); // 材料本构关系 (对于静电是标量)

                // --- 通用形式： K_elem += B^T * D * B * dV ---
                K_elem += (B.transpose() * D * B) * static_cast<TScalar>(q_point.JxW());
            }
            return K_elem;
        }

    private:
        const Material& mat_;
    };
}