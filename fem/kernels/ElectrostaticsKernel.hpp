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

            FEValues fe_values(element, 1, AnalysisType::SCALAR_DIFFUSION);
            const MaterialProperty& eps_prop = mat_.getProperty("permittivity");

            for (size_t q = 0; q < fe_values.n_quad_points(); ++q) {
                fe_values.reinit(q);

                double eps = eps_prop.evaluate();

                // --- 正确的实现 ---
                const auto& B = fe_values.dN_dx();
                TScalar D = static_cast<TScalar>(eps); // 材料本构关系 (对于静电是标量)

                // --- 通用形式： K_elem += B^T * D * B * dV ---
                K_elem += (B.transpose() * D * B) * static_cast<TScalar>(fe_values.JxW());
            }
            return K_elem;
        }

    private:
        const Material& mat_;
    };
}