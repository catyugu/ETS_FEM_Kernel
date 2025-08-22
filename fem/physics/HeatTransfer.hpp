#pragma once
#include <vector>
#include <memory>
#include <Eigen/Sparse>
#include "../kernels/Kernel.hpp"
#include "../kernels/KernelWrappers.hpp"
#include "../mesh/Mesh.hpp"
#include "../core/DofManager.hpp"
#include "PhysicsField.hpp"
#include <complex>

namespace FEM {

    /**
     * @brief 热传导物理场类
     * 
     * 实现热传导问题的求解，计算温度分布
     */
    template<int TDim, typename TScalar = double>
    class HeatTransfer : public PhysicsField<TDim, TScalar> {
    public:
        using KernelBase = Kernel<TDim, TScalar>; // 移除硬编码的节点数2

        /**
         * @brief 获取物理场名称
         * @return 物理场名称
         */
        std::string getName() const override {
            return "HeatTransfer";
        }
    };
}