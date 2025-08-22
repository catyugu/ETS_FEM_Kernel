#include <gtest/gtest.h>
#include <memory>
#include <cmath>
#include <vector>

#include "fem/core/Problem.hpp"
#include "fem/materials/Material.hpp"
#include "fem/physics/HeatTransfer.hpp"
#include "fem/kernels/HeatDiffusionKernel.hpp"
#include "fem/bcs/NeumannBC.hpp"
#include "fem/bcs/CauchyBC.hpp"
#include "fem/mesh/Mesh.hpp"
#include "fem/core/DofManager.hpp"

// 测试套件，用于验证混合边界条件（诺伊曼 + 柯西）
class MixedBoundaryConditionsTest : public ::testing::Test {
protected:
    // 定义物理和几何参数
    static constexpr int problem_dim = 1;
    const double L = 1.0;         // 杆的长度
    const double k = 2.0;         // 热导率
    const double q0 = 50.0;       // 左端的热流密度 (Neumann)
    const double h = 10.0;        // 对流系数 (Cauchy)
    const double T_inf = 20.0;    // 环境温度 (Cauchy)
    const int num_elements = 20;  // 网格单元数量

    // 解析解函数
    double analytical_solution(double x) const {
        return (-q0 / k) * x + q0 * (1.0 / h + L / k) + T_inf;
    }
};

// 测试案例：1D热传导问题，左端为Neumann，右端为Cauchy
TEST_F(MixedBoundaryConditionsTest, HeatTransfer_Neumann_Cauchy_1D) {
    // 1. 创建网格
    auto mesh = FEM::Mesh::create_uniform_1d_mesh(L, num_elements);

    // 2. 定义材料
    FEM::Material test_material("TestMaterial");
    test_material.setProperty("thermal_conductivity", k);

    // 3. 创建物理场
    auto heat_physics = std::make_unique<FEM::HeatTransfer<problem_dim>>();

    // 4. 添加计算核 (热扩散)
    heat_physics->addKernel(
        std::make_unique<FEM::HeatDiffusionKernel<problem_dim>>(test_material)
    );

    // 5. 添加边界条件
    // 左端 (x=0) 添加 Neumann 边界条件
    heat_physics->addBoundaryCondition(
        std::make_unique<FEM::NeumannBC<problem_dim>>("left", q0)
    );
    // 右端 (x=L) 添加 Cauchy 边界条件
    heat_physics->addBoundaryCondition(
        std::make_unique<FEM::CauchyBC<problem_dim>>("right", h, T_inf)
    );

    // 6. 创建并配置 Problem
    auto problem = std::make_unique<FEM::Problem<problem_dim>>(
        std::move(mesh), std::move(heat_physics)
    );

    // 7. 组装并求解
    problem->assemble();
    problem->solve();

    // 8. 验证结果
    const auto& solution_vector = problem->getSolution();
    const auto& fem_mesh = problem->getMesh();
    const auto& dof_manager = problem->getDofManager();

    // 检查网格上每个节点的解是否与解析解匹配
    for (const auto& node : fem_mesh.getNodes()) {
        const int node_id = node->getId();
        const double node_x = node->getX();

        // 获取该节点的自由度索引
        const int dof_index = dof_manager.getNodeDof(node_id, 0);

        // 获取有限元计算得到的温度值
        const double numerical_temp = solution_vector(dof_index);

        // 计算解析解的温度值
        const double analytical_temp = analytical_solution(node_x);

        // 打印对比结果，方便调试
        std::cout << "Node at x=" << node_x
                  << ", Numerical T=" << numerical_temp
                  << ", Analytical T=" << analytical_temp << std::endl;

        // 断言：数值解与解析解的误差在可接受范围内
        EXPECT_NEAR(numerical_temp, analytical_temp, 1e-5);
    }
}