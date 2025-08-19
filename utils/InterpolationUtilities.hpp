#pragma once

#include <Eigen/Dense>
#include <map>
#include <string>
#include <vector>
#include "core/Problem.hpp" // 需要Problem来获取场和解
#include "mesh/Element.hpp"

namespace FEM::Utils {

    class InterpolationUtilities {
    public:
        /**
         * @brief 在积分点上插值场变量的值
         * @param element 当前单元
         * @param shape_values 当前积分点上的形函数值 (N)
         * @param problem Problem实例，用于访问所有物理场和解
         * @return 一个从变量名到插值的映射
         */
        static std::map<std::string, double> interpolateAtQuadraturePoint(
            const Element& element,
            const Eigen::VectorXd& shape_values,
            const Problem<1>& problem // 简化为1D，实际应为模板
        ) {
            // 在一个完整的多场框架中，这里会更复杂
            // 此处简化实现：我们只插值第一个场（温度）
            std::map<std::string, double> interpolated_values;
            const auto& field = problem.getPhysicsField(); // 假设Problem有此方法
            const auto& solution = problem.getSolution();
            const auto& dof_manager = problem.getDofManager();
            const auto& nodes = element.getNodes();

            double interpolated_value = 0.0;
            for (size_t i = 0; i < nodes.size(); ++i) {
                int dof_index = dof_manager.getNodeDof(nodes[i]->getId(), 0);
                if (dof_index < solution.size()) {
                    interpolated_value += shape_values(i) * solution(dof_index);
                }
            }
            interpolated_values["Temperature"] = interpolated_value;
            return interpolated_values;
        }
    };
}