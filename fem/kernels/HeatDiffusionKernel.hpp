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

            // FEValues现在需要知道分析类型
            FEValues fe_values(element, 1, AnalysisType::SCALAR_DIFFUSION);
            const MaterialProperty& k_prop = mat_.getProperty("thermal_conductivity");

            for (size_t q = 0; q < fe_values.n_quad_points(); ++q) {
                fe_values.reinit(q);

                // 在一个完整实现中，我们会先插值温度T_q

                // 简化：暂时仍使用常数
                double k = k_prop.evaluate();

                // 构建B矩阵 (梯度算子)
                const auto& dN_dx = fe_values.dN_dx();
                Eigen::MatrixXd B = dN_dx;

                double D = k; // 获取D矩阵 (标量)

                // --- 通用形式： K_elem += B^T * D * B * dV ---
                K_elem += (B.transpose() * D * B) * fe_values.JxW();
            }
            return K_elem;
        }
    private:
        const Material& mat_;
    };
}