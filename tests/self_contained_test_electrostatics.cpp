#include <gtest/gtest.h>
#include <memory>
#include <iostream>
#include "fem/core/Problem.hpp"
#include "fem/physics/Electrostatics.hpp"
#include "fem/kernels/ElectrostaticsKernel.hpp"
#include "fem/materials/Material.hpp"
#include "fem/io/Exporter.hpp"
#include "fem/mesh/Mesh.hpp"
#include "fem/bcs/DirichletBC.hpp"

template<int dim>
void test_electrostatics() {
    auto mesh = FEM::Mesh::create_uniform_1d_mesh(1.0, 10);
    auto material = std::make_unique<FEM::Material>("Copper");
    material->setProperty("electrical_conductivity", 1.0);

    auto physics = std::make_unique<FEM::Electrostatics<dim>>();
    physics->addKernel(std::make_unique<FEM::ElectrostaticsKernel<dim, 2>>(*material));

    // 使用物理场管理边界条件 - 修改为正确的边界名称
    physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim>>("left", 10.0)
    );
    physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim>>("right", 0.0)
    );

    auto problem = std::make_unique<FEM::Problem<dim>>(std::move(mesh), std::move(physics));

    problem->assemble();
    problem->solve();

    const auto& solution = problem->getSolution();
    // 对于线性分布，中间节点的电势应该是两端的平均值
    ASSERT_NEAR(solution(5), 5.0, 1e-9);
    // FEM::IO::Exporter::write_vtk("electrostatics_1d_results.vtk", *problem);
}

class ElectrostaticsTest : public ::testing::Test {
protected:
    void SetUp() override {
        material = std::make_unique<FEM::Material>("Copper");
        material->setProperty("electrical_conductivity", 8.854187817e-12); // 真空介电常数
    }
    std::unique_ptr<FEM::Material> material;
};

// 1D Test
TEST_F(ElectrostaticsTest, Solves1DProblem) {
    test_electrostatics<1>();
}

// 2D Test
TEST_F(ElectrostaticsTest, Solves2DProblem) {
    constexpr int dim = 2;
    constexpr int num_nodes_per_elem = 4;
    const int nx = 10, ny = 10;

    auto mesh = FEM::Mesh::create_uniform_2d_mesh(1.0, 1.0, nx, ny);
    auto material = std::make_unique<FEM::Material>("Copper");
    material->setProperty("electrical_conductivity", 1.0);
    
    auto physics = std::make_unique<FEM::Electrostatics<dim>>();

    physics->addKernel(
        std::make_unique<FEM::ElectrostaticsKernel<dim, num_nodes_per_elem>>(*material)
    );

    // 使用物理场管理边界条件 - 修改为正确的边界名称
    physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim>>("left", 10.0)
    );
    physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim>>("right", 0.0)
    );

    auto problem = std::make_unique<FEM::Problem<dim>>(std::move(mesh), std::move(physics));
    
    problem->assemble();
    problem->solve();

    const auto& solution = problem->getSolution();
    // 在2D情况下，检查几个关键点的解
    ASSERT_NEAR(solution(0), 10.0, 1e-9);
    ASSERT_NEAR(solution(10), 0.0, 1e-9);
    // FEM::IO::Exporter::write_vtk("electrostatics_2d_results.vtk", *problem);
}

// 3D Test
TEST_F(ElectrostaticsTest, Solves3DProblem) {
    constexpr int dim = 3;
    constexpr int num_nodes_per_elem = 8;
    const int nx = 5, ny = 5, nz = 5;

    auto mesh = FEM::Mesh::create_uniform_3d_mesh(1.0, 1.0, 1.0, nx, ny, nz);
    auto material = std::make_unique<FEM::Material>("Copper");
    material->setProperty("electrical_conductivity", 1.0);
    
    auto physics = std::make_unique<FEM::Electrostatics<dim>>();

    physics->addKernel(
        std::make_unique<FEM::ElectrostaticsKernel<dim, num_nodes_per_elem>>(*material)
    );

    // 使用物理场管理边界条件
    physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim>>("left", 10.0)
    );
    physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<dim>>("right", 0.0)
    );

    auto problem = std::make_unique<FEM::Problem<dim>>(std::move(mesh), std::move(physics));
    
    problem->assemble();
    problem->solve();

    const auto& solution = problem->getSolution();
    // 在3D情况下，检查几个关键点的解
    ASSERT_NEAR(solution(0), 10.0, 1e-9);
    ASSERT_NEAR(solution(5), 0.0, 1e-9);
    // FEM::IO::Exporter::write_vtk("electrostatics_3d_results.vtk", *problem);
}