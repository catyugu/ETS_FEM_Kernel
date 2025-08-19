#include <gtest/gtest.h>
#include "core/Problem.hpp"
#include "materials/Material.hpp"
#include "kernels/ElectrostaticsKernel.hpp"
#include "io/Exporter.hpp"
#include "physics/Electrostatics.hpp"
#include <cmath>

class ElectrostaticsTest : public ::testing::Test {
protected:
    void SetUp() override {
        material = std::make_unique<FEM::Material>("Copper");
        material->setProperty("electrical_conductivity", 5.96e7); // 铜的电导率 S/m
    }
    std::unique_ptr<FEM::Material> material;
};

// 1D Test
TEST_F(ElectrostaticsTest, Solves1DProblem) {
    constexpr int dim = 1;
    constexpr int num_nodes_per_elem = 2;

    auto mesh = FEM::Mesh::create_uniform_1d_mesh(1.0, 10);
    auto physics = std::make_unique<FEM::Electrostatics<dim>>();

    // 使用新的ElectrostaticsKernel而不是HeatDiffusionKernel
    physics->addKernel(
        std::make_unique<FEM::ElectrostaticsKernel<dim, num_nodes_per_elem>>(*material)
    );

    auto problem = std::make_unique<FEM::Problem<dim>>(std::move(mesh), std::move(physics));

    problem->addDirichletBC(0, 10.0);   // 左端点 10V
    problem->addDirichletBC(10, 0.0);   // 右端点 0V (地)
    problem->assemble();
    problem->applyBCs();
    problem->solve();

    const auto& solution = problem->getSolution();
    // 对于线性分布，中间节点的电势应该是两端的平均值
    ASSERT_NEAR(solution(5), 5.0, 1e-9);
    FEM::IO::Exporter::write_vtk("electrostatics_1d_results.vtk", *problem);
}

// 2D Test
TEST_F(ElectrostaticsTest, Solves2DProblem) {
    constexpr int dim = 2;
    constexpr int num_nodes_per_elem = 4;
    const int nx = 10, ny = 10;

    auto mesh = FEM::Mesh::create_uniform_2d_mesh(1.0, 1.0, nx, ny);
    auto physics = std::make_unique<FEM::Electrostatics<dim>>();

    physics->addKernel(
        std::make_unique<FEM::ElectrostaticsKernel<dim, num_nodes_per_elem>>(*material)
    );

    auto problem = std::make_unique<FEM::Problem<dim>>(std::move(mesh), std::move(physics));

    const auto& nodes = problem->getMesh().getNodes();
    for(const auto& node : nodes) {
        const auto& coords = node->getCoords();
        if (std::abs(coords[0] - 0.0) < 1e-9) problem->addDirichletBC(node->getId(), 10.0);  // 10V
        if (std::abs(coords[0] - 1.0) < 1e-9) problem->addDirichletBC(node->getId(), 0.0);   // 0V
    }

    problem->assemble();
    problem->applyBCs();
    problem->solve();

    const auto& solution = problem->getSolution();
    int center_node_id = (ny + 1) * (nx / 2) + (ny / 2);
    ASSERT_NEAR(solution(center_node_id), 5.0, 1e-9);
    FEM::IO::Exporter::write_vtk("electrostatics_2d_results.vtk", *problem);
}

// 3D Test
TEST_F(ElectrostaticsTest, Solves3DProblem) {
    constexpr int dim = 3;
    constexpr int num_nodes_per_elem = 8;
    const int nx = 5, ny = 5, nz = 5;

    auto mesh = FEM::Mesh::create_uniform_3d_mesh(1.0, 1.0, 1.0, nx, ny, nz);
    auto physics = std::make_unique<FEM::Electrostatics<dim>>();

    physics->addKernel(
        std::make_unique<FEM::ElectrostaticsKernel<dim, num_nodes_per_elem>>(*material)
    );

    auto problem = std::make_unique<FEM::Problem<dim>>(std::move(mesh), std::move(physics));

    const auto& nodes = problem->getMesh().getNodes();
    for(const auto& node : nodes) {
        const auto& coords = node->getCoords();
        if (std::abs(coords[0] - 0.0) < 1e-9) problem->addDirichletBC(node->getId(), 10.0);  // 10V
        if (std::abs(coords[0] - 1.0) < 1e-9) problem->addDirichletBC(node->getId(), 0.0);   // 0V
    }

    problem->assemble();
    problem->applyBCs();
    problem->solve();

    const auto& solution = problem->getSolution();
    int center_node_id = (nz/2)*(nx+1)*(ny+1) + (ny/2)*(nx+1) + (nx/2);
    // 在3D情况下，由于几何和边界条件的复杂性，中心点的值可能不是简单的平均值
    ASSERT_NEAR(solution(center_node_id), 6.0, 1e-9); // 期望值基于问题设置
    FEM::IO::Exporter::write_vtk("electrostatics_3d_results.vtk", *problem);
}