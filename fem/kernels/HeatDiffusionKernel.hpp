#pragma once
#include "Kernel.hpp"
#include "../materials/Material.hpp"
#include "../core/FEValues.hpp"

namespace FEM {
    template<int TDim, int TNumNodes>
    class HeatDiffusionKernel : public Kernel<TDim, TNumNodes> {
    public:
        explicit HeatDiffusionKernel(const Material& material) : mat_(material) {}

        Eigen::Matrix<double, TNumNodes, TNumNodes>
        compute_element_matrix(const Element& element) override {
            Eigen::Matrix<double, TNumNodes, TNumNodes> K_elem;
            K_elem.setZero();

            // FEValues 不再需要知道分析类型来构建B矩阵
            FEValues fe_values(element, 1, AnalysisType::SCALAR_DIFFUSION);
            const MaterialProperty& k_prop = mat_.getProperty("thermal_conductivity");

            for (size_t q = 0; q < fe_values.n_quad_points(); ++q) {
                fe_values.reinit(q);

                double k = k_prop.evaluate();

                // --- 正确的实现 ---
                // 内核(Kernel)现在负责从FEValues获取梯度dN_dx，并将其用作B矩阵
                const auto& B = fe_values.dN_dx();
                double D = k; // 材料本构关系 (对于热传导是标量)

                // --- 通用形式： K_elem += B^T * D * B * dV ---
                K_elem += (B.transpose() * D * B) * fe_values.JxW();
            }
            return K_elem;
        }
    private:
        const Material& mat_;
    };
}