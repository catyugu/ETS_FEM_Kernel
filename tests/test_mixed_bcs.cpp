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
#include "fem/bcs/DirichletBC.hpp"
#include "fem/mesh/Mesh.hpp"
#include "fem/core/DofManager.hpp"

// 测试套件，用于验证混合边界条件（诺伊曼 + 柯西）
class MixedBoundaryConditionsTest : public ::testing::Test {
protected:
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
    constexpr int problem_dim = 1;
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
    auto neumann_bc = std::make_unique<FEM::NeumannBC<problem_dim>>("Temperature", "left", q0);
    std::cout << "Adding Neumann BC with value: " << -q0 << std::endl;
    heat_physics->addBoundaryCondition(std::move(neumann_bc));
    
    // 右端 (x=L) 添加 Cauchy 边界条件
    auto cauchy_bc = std::make_unique<FEM::CauchyBC<problem_dim>>("Temperature", "right", h, T_inf);
    std::cout << "Adding Cauchy BC with h: " << h << ", T_inf: " << T_inf << std::endl;
    heat_physics->addBoundaryCondition(std::move(cauchy_bc));

    // 6. 创建并配置 Problem
    auto problem = std::make_unique<FEM::Problem<problem_dim>>(
        std::move(mesh), std::move(heat_physics)
    );


    // 7. 组装并求解
    problem->assemble();
    
    // 添加调试信息
    std::cout << "Global Stiffness Matrix K:" << std::endl << problem->getStiffnessMatrix() << std::endl;
    std::cout << "Global Load Vector F:" << std::endl << problem->getLoadVector() << std::endl;
    
    problem->solve();
    
    std::cout << "Solution Vector U:" << std::endl << problem->getSolution() << std::endl;

    // 8. 验证结果
    const auto& solution_vector = problem->getSolution();
    const auto& fem_mesh = problem->getMesh();
    const auto& dof_manager = problem->getDofManager();

    // 检查网格上每个节点的解是否与解析解匹配
    for (const auto& node : fem_mesh.getNodes()) {
        const int node_id = node->getId();
        const double node_x = node->getX();

        // 获取该节点的自由度索引
        const int dof_index = dof_manager.getNodeDof("Temperature", node_id);

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
TEST_F(MixedBoundaryConditionsTest, HeatTransfer_2D_Analytic) {
    // --- 1. 问题定义 ---
    constexpr int problem_dim = 2;
    const double k = 1.0; // 热导率

    // 解析解: T(x, y) = 2x + 1
    auto analytical_solution = [](double x, double) {
        return 2.0 * x + 1.0;
    };

    // --- 2. 创建网格 ---
    auto mesh = FEM::Mesh::create_uniform_2d_mesh(1.0, 1.0, 10, 10);

    // --- 3. 定义材料和物理场 ---
    FEM::Material material("TestMat");
    material.setProperty("thermal_conductivity", k);
    auto heat_physics = std::make_unique<FEM::HeatTransfer<problem_dim>>();
    heat_physics->addKernel(std::make_unique<FEM::HeatDiffusionKernel<problem_dim>>(material));

    // --- 4. 施加边界条件 ---
    // 左边界 (x=0): Cauchy, -k*dT/dx = h(T - T_inf) => -1*2 = 2(1 - T_inf) => T_inf = 2
    heat_physics->addBoundaryCondition(std::make_unique<FEM::CauchyBC<problem_dim>>("Temperature", "left", -2.0, 2.0));
    // 右边界 (x=1): Dirichlet, T(1,y) = 2*1 + 1 = 3
    heat_physics->addBoundaryCondition(std::make_unique<FEM::DirichletBC<problem_dim>>("Temperature", "right", 3.0));
    // 顶部 (y=1) 和 底部 (y=0): Neumann, dT/dy = 0 (绝热)
    // heat_physics->addBoundaryCondition(std::make_unique<FEM::NeumannBC<problem_dim>>("top", 0.0));
    // heat_physics->addBoundaryCondition(std::make_unique<FEM::NeumannBC<problem_dim>>("bottom", 0.0));

    // --- 5. 创建问题并求解 ---
    auto problem = std::make_unique<FEM::Problem<problem_dim>>(std::move(mesh), std::move(heat_physics));
    problem->assemble();
    problem->solve();

    // --- 6. 验证结果 ---
    const auto& solution_vector = problem->getSolution();
    const auto& fem_mesh = problem->getMesh();
    const auto& dof_manager = problem->getDofManager();

    for (const auto& node : fem_mesh.getNodes()) {
        const double node_x = node->getX();
        const double node_y = node->getY();
        const int dof_index = dof_manager.getNodeDof("Temperature", node->getId());
        const double numerical_temp = solution_vector(dof_index);
        const double analytical_temp = analytical_solution(node_x, node_y);
        EXPECT_NEAR(numerical_temp, analytical_temp, 1e-5);
    }
}
TEST_F(MixedBoundaryConditionsTest, HeatTransfer_3D_Analytic) {
    // --- 1. 问题定义 ---
    constexpr int problem_dim = 3;
    const double k = 1.0; // 热导率

    // 解析解: T(x, y, z) = 5z + 10
    auto analytical_solution = [](double, double, double z) {
        return 5.0 * z + 10.0;
    };

    // --- 2. 创建网格 ---
    auto mesh = FEM::Mesh::create_uniform_3d_mesh(1.0, 1.0, 1.0, 5, 5, 5);

    // --- 3. 定义材料和物理场 ---
    FEM::Material material("TestMat");
    material.setProperty("thermal_conductivity", k);
    auto heat_physics = std::make_unique<FEM::HeatTransfer<problem_dim>>();
    heat_physics->addKernel(std::make_unique<FEM::HeatDiffusionKernel<problem_dim>>(material));

    // --- 4. 施加边界条件 ---
    // 底面 (z=0): Dirichlet, T(x,y,0) = 10
    heat_physics->addBoundaryCondition(std::make_unique<FEM::DirichletBC<problem_dim>>("Temperature", "bottom", 10.0));
    // 顶面 (z=1): Neumann, -k*dT/dz = -1*5 = -5
    heat_physics->addBoundaryCondition(std::make_unique<FEM::NeumannBC<problem_dim>>("Temperature", "top", 5.0));
    // 其他四个侧面: Neumann, dT/dn = 0 (绝热)
    heat_physics->addBoundaryCondition(std::make_unique<FEM::NeumannBC<problem_dim>>("Temperature", "front", 0.0));
    heat_physics->addBoundaryCondition(std::make_unique<FEM::NeumannBC<problem_dim>>("Temperature", "back", 0.0));
    heat_physics->addBoundaryCondition(std::make_unique<FEM::NeumannBC<problem_dim>>("Temperature", "left", 0.0));
    heat_physics->addBoundaryCondition(std::make_unique<FEM::NeumannBC<problem_dim>>("Temperature", "right", 0.0));

    // --- 5. 创建问题并求解 ---
    auto problem = std::make_unique<FEM::Problem<problem_dim>>(std::move(mesh), std::move(heat_physics));
    problem->assemble();
    problem->solve();

    // --- 6. 验证结果 ---
    const auto& solution_vector = problem->getSolution();
    const auto& fem_mesh = problem->getMesh();
    const auto& dof_manager = problem->getDofManager();

    for (const auto& node : fem_mesh.getNodes()) {
        const double node_x = node->getX();
        const double node_y = node->getY();
        const double node_z = node->getZ();
        const int dof_index = dof_manager.getNodeDof("Temperature", node->getId());
        const double numerical_temp = solution_vector(dof_index);
        const double analytical_temp = analytical_solution(node_x, node_y, node_z);
        EXPECT_NEAR(numerical_temp, analytical_temp, 1e-5);
    }
}