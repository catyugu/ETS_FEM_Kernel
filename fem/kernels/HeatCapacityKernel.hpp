#pragma once
#include "Kernel.hpp"
#include "../materials/Material.hpp"
#include "../core/FEValues.hpp"
#include <complex>

namespace FEM {
    template<int TDim, typename TScalar = double>
    class HeatCapacityKernel : public Kernel<TDim, TScalar> {
    public:
        using MatrixType = typename Kernel<TDim, TScalar>::MatrixType;

        // 构造函数支持频率参数（用于频域分析）
        explicit HeatCapacityKernel(const Material& material, double omega = 0.0) 
            : mat_(material), omega_(omega) {}

        MatrixType compute_element_matrix(const Element& element) override {
            // 在运行时获取节点数
            const int num_nodes = element.getNumNodes();

            MatrixType C_elem = MatrixType::Zero(num_nodes, num_nodes);

            FEValues fe_values(element, 1, AnalysisType::SCALAR_DIFFUSION);
            const MaterialProperty& rho_prop = mat_.getProperty("density");
            const MaterialProperty& cp_prop = mat_.getProperty("specific_heat");

            for (size_t q = 0; q < fe_values.n_quad_points(); ++q) {
                fe_values.reinit(q);

                double rho = rho_prop.evaluate();
                double cp = cp_prop.evaluate();

                // --- 一致的质量矩阵 ---
                const auto& N = fe_values.N(); // 形函数值 (VectorXd)
                double JxW = fe_values.JxW();

                // 对于频域分析，需要乘以j*omega
                TScalar factor = static_cast<TScalar>(rho * cp);
                if (omega_ != 0.0) {
                    // 在频域分析中，质量矩阵需要乘以j*omega
                    factor *= TScalar(0.0, omega_);
                }

                // C_elem += factor * N * N^T * dV
                C_elem += factor * N * N.transpose() * static_cast<TScalar>(JxW);
            }
            return C_elem;
        }

    private:
        const Material& mat_;
        double omega_ = 0.0; // 频率参数，用于频域分析
    };
}