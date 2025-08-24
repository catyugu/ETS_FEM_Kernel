#pragma once
#include <vector>
#include <memory>
#include <Eigen/Sparse>
#include "../kernels/Kernel.hpp"
#include "../kernels/KernelWrappers.hpp"
#include "../mesh/Mesh.hpp"
#include "../core/DofManager.hpp"
#include "PhysicsField.hpp"

namespace FEM {

    /**
     * @brief 静电场物理场类
     * 
     * 实现静电场问题的求解，计算电势分布
     */
    template<int TDim, typename TScalar = double>
    class Electrostatics : public PhysicsField<TDim, TScalar> {
    public:
        using KernelBase = Kernel<TDim, TScalar>; // 移除硬编码的节点数

        Electrostatics() : variable_name_("Voltage") {}

        /**
         * @brief 获取变量名称
         * @return 变量名称
         */
        const std::string& getVariableName() const override {
            return variable_name_;
        }

        /**
         * @brief 定义变量到自由度管理器
         * @param dof_manager 自由度管理器
         */
        void defineVariables(DofManager& dof_manager) const override {
            dof_manager.addVariable(getVariableName(), DofType::NODE, 1);
        }

        /**
         * @brief 获取物理场名称
         * @return 物理场名称
         */
        std::string getName() const override {
            return "Electrostatics";
        }

    private:
        std::string variable_name_;
    };
}