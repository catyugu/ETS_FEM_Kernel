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
            double k = mat_.getProperty("thermal_conductivity");

            // 使用新的 FEValues
            FEValues fe_values(element, 1); // 使用1阶单元和1阶积分

            for (size_t q = 0; q < fe_values.n_quad_points(); ++q) {
                fe_values.reinit(q); // 移动到当前积分点
                const auto& grad_N = fe_values.dN_dx();
                K_elem += (grad_N.transpose() * k * grad_N) * fe_values.JxW();
            }
            return K_elem;
        }
    private:
        const Material& mat_;
    };
}