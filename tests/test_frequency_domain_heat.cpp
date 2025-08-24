#include <gtest/gtest.h>
#include "fem/core/Problem.hpp"
#include "fem/materials/Material.hpp"
#include "fem/kernels/HeatDiffusionKernel.hpp"
#include "fem/kernels/HeatCapacityKernel.hpp" // 引入新Kernel
#include "fem/physics/HeatTransfer.hpp"
#include "fem/bcs/DirichletBC.hpp"
#include <complex>
#define _USE_MATH_DEFINES
#include <math.h>

// 定义标量类型为复数
using ComplexScalar = std::complex<double>;

TEST(FrequencyDomainTest, 1DHeatWave) {
    constexpr int dim = 1;
    const double length = 1.0;
    const int num_elements = 50; // 使用足够多的单元以保证精度
    const double omega = 2.0 * M_PI;

    // 1. 创建网格
    auto mesh = FEM::Mesh::create_uniform_1d_mesh(length, num_elements);

    // 2. 定义材料 (增加密度和比热)
    FEM::Material material("TestMaterial");
    material.setProperty("thermal_conductivity", 1.0);
    material.setProperty("density", 1.0);
    material.setProperty("specific_heat", 1.0);

    // 3. 创建物理场 (使用复数类型)
    auto heat_physics = std::make_unique<FEM::HeatTransfer<dim, ComplexScalar>>();

    // 4. 添加稳态和频域 Kernels
    heat_physics->addKernel(
        std::make_unique<FEM::HeatDiffusionKernel<dim, ComplexScalar>>(material)
    );
    heat_physics->addKernel(
        std::make_unique<FEM::HeatCapacityKernel<dim, ComplexScalar>>(material, omega)
    );

    // 5. 添加边界条件
    heat_physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim, ComplexScalar>>("Temperature", "left", ComplexScalar(1.0, 0.0))
    );
    heat_physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim, ComplexScalar>>("Temperature", "right", ComplexScalar(0.0, 0.0))
    );
    
    // 6. 创建 Problem (使用复数类型)
    auto problem = std::make_unique<FEM::Problem<dim, ComplexScalar>>(std::move(mesh), std::move(heat_physics));
    
    // 7. 求解
    problem->assemble();
    problem->solve();

    // 8. 验证结果
    const auto& solution = problem->getSolution();
    const auto& result_mesh = problem->getMesh();
    
    // 解析解
    const ComplexScalar j(0.0, 1.0);
    const ComplexScalar gamma = std::sqrt(j * omega); // k=rho=c=1
    auto analytical_solution = [&](double x) {
        return (std::exp(-gamma * x) - std::exp(gamma * (x - 2.0 * length))) / (1.0 - std::exp(-2.0 * gamma * length));
    };

    double max_error = 0.0;
    for (const auto& node_ptr : result_mesh.getNodes()) {
        const auto& node = *node_ptr;
        int dof_index = problem->getDofManager().getNodeDof("Temperature", node.getId());
        ComplexScalar fem_val = solution(dof_index);
        ComplexScalar analytical_val = analytical_solution(node.getX());
        
        double error = std::abs(fem_val - analytical_val);
        if (error > max_error) {
            max_error = error;
        }
    }
    
    // 期望误差在一个较小的范围内
    EXPECT_LT(max_error, 1e-4);
}