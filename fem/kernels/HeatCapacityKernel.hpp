#pragma once
#include "Kernel.hpp"
#include "../materials/Material.hpp"
#include "../core/FEValues.hpp"
#include <complex>

namespace FEM {
    template<int TDim, int TNumNodes_, typename TScalar = double>
    class HeatCapacityKernel : public Kernel<TDim, TNumNodes_, TScalar> {
    public:
        using MatrixType = typename Kernel<TDim, TNumNodes_, TScalar>::MatrixType;

        HeatCapacityKernel(const Material& material, double omega) : material_(material), omega_(omega) {}

        MatrixType compute_element_matrix(const Element& element) override {
            FEValues fe_values(element, 1, AnalysisType::SCALAR_DIFFUSION);
            MatrixType M_elem = MatrixType::Zero();

            const double rho = material_.getProperty("density").evaluate();
            const double c = material_.getProperty("specific_heat").evaluate();
            
            // 频域项: j * omega * rho * c
            const TScalar coeff = TScalar(0.0, 1.0) * static_cast<TScalar>(omega_ * rho * c);

            for (size_t q = 0; q < fe_values.n_quad_points(); ++q) {
                fe_values.reinit(q);
                TScalar JxW = static_cast<TScalar>(fe_values.JxW());
                for (size_t i = 0; i < TNumNodes_; ++i) {
                    for (size_t j = 0; j < TNumNodes_; ++j) {
                        // M_ij = ∫(coeff * Ni * Nj) dV
                        M_elem(i, j) += coeff * static_cast<TScalar>(fe_values.N()(i) * fe_values.N()(j)) * JxW;
                    }
                }
            }
            return M_elem;
        }

    private:
        const Material& material_;
        double omega_; // 角频率
    };
}