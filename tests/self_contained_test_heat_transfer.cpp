#include <gtest/gtest.h>
#include <memory>
#include <iostream>
#include "fem/core/Problem.hpp"
#include "fem/physics/HeatTransfer.hpp"
#include "fem/kernels/HeatDiffusionKernel.hpp"
#include "fem/materials/Material.hpp"
#include "fem/io/Exporter.hpp"
#include "fem/mesh/Mesh.hpp"
#include "fem/bcs/DirichletBC.hpp"

template<int dim>
void test_heat_transfer() {
    auto mesh = FEM::Mesh::create_uniform_1d_mesh(1.0, 10);
    auto material = std::make_unique<FEM::Material>("Copper");
    material->setProperty("thermal_conductivity", 1.0);

    auto physics = std::make_unique<FEM::HeatTransfer<dim>>();
    physics->addKernel(std::make_unique<FEM::HeatDiffusionKernel<dim, 2>>(*material));

    // 使用物理场管理边界条件
    // 注意：对于程序生成的简单网格，我们使用假的边界名称
    physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim>>("left_end", 100.0)
    );
    physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim>>("right_end", 0.0)
    );

    auto problem = std::make_unique<FEM::Problem<dim>>(std::move(mesh), std::move(physics));

    problem->assemble();
    problem->solve();

    const auto& solution = problem->getSolution();
    ASSERT_NEAR(solution(5), 50.0, 1e-9);
    // FEM::IO::Exporter::write_vtk("heat_1d_results.vtk", *problem);
}

class HeatTransferTest : public ::testing::Test {
protected:
    void SetUp() override {
        material = std::make_unique<FEM::Material>("Copper");
        material->setProperty("thermal_conductivity", 400.0);
    }
    std::unique_ptr<FEM::Material> material;
};

// 1D Test
TEST_F(HeatTransferTest, Solves1DProblem) {
    test_heat_transfer<1>();
}

// 2D Test
TEST_F(HeatTransferTest, Solves2DProblem) {
    constexpr int dim = 2;
    constexpr int num_nodes_per_elem = 4;
    const int nx = 10, ny = 10;

    auto mesh = FEM::Mesh::create_uniform_2d_mesh(1.0, 1.0, nx, ny);
    auto material = std::make_unique<FEM::Material>("Copper");
    material->setProperty("thermal_conductivity", 1.0);
    
    auto physics = std::make_unique<FEM::HeatTransfer<dim>>();

    physics->addKernel(
        std::make_unique<FEM::HeatDiffusionKernel<dim, num_nodes_per_elem>>(*material)
    );

    // 添加边界条件 - 使用物理场管理
    physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim>>("left_end", 100.0)
    );
    physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim>>("right_end", 0.0)
    );

    auto problem = std::make_unique<FEM::Problem<dim>>(std::move(mesh), std::move(physics));

    problem->assemble();
    problem->solve();

    const auto& solution = problem->getSolution();
    // 在2D情况下，检查几个关键点的解
    ASSERT_NEAR(solution(0), 100.0, 1e-9);
    ASSERT_NEAR(solution(10), 0.0, 1e-9);
    // FEM::IO::Exporter::write_vtk("heat_2d_results.vtk", *problem);
}

// 3D Test
TEST_F(HeatTransferTest, Solves3DProblem) {
    constexpr int dim = 3;
    constexpr int num_nodes_per_elem = 8;
    const int nx = 5, ny = 5, nz = 5;

    auto mesh = FEM::Mesh::create_uniform_3d_mesh(1.0, 1.0, 1.0, nx, ny, nz);
    auto material = std::make_unique<FEM::Material>("Copper");
    material->setProperty("thermal_conductivity", 1.0);
    
    auto physics = std::make_unique<FEM::HeatTransfer<dim>>();

    physics->addKernel(
        std::make_unique<FEM::HeatDiffusionKernel<dim, num_nodes_per_elem>>(*material)
    );

    // 添加边界条件 - 使用物理场管理
    physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim>>("left_end", 100.0)
    );
    physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim>>("right_end", 0.0)
    );

    auto problem = std::make_unique<FEM::Problem<dim>>(std::move(mesh), std::move(physics));

    problem->assemble();
    problem->solve();

    const auto& solution = problem->getSolution();
    // 在3D情况下，检查几个关键点的解
    ASSERT_NEAR(solution(0), 100.0, 1e-9);
    ASSERT_NEAR(solution(5), 0.0, 1e-9);
    // FEM::IO::Exporter::write_vtk("heat_3d_results.vtk", *problem);
}