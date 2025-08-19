#include <gtest/gtest.h>
#include "core/Problem.hpp"
#include "materials/Material.hpp"
#include "kernels/HeatDiffusionKernel.hpp"
#include "io/Exporter.hpp"
#include <cmath>

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
    constexpr int dim = 1;
    constexpr int num_nodes_per_elem = 2;

    auto mesh = FEM::Mesh::create_uniform_1d_mesh(1.0, 10);
    auto physics = std::make_unique<FEM::HeatTransfer<dim>>();

    // 代码恢复简洁，新的 addKernel 会自动处理所有权和类型转换
    physics->addKernel(
        std::make_unique<FEM::HeatDiffusionKernel<dim, num_nodes_per_elem>>(*material)
    );

    auto problem = std::make_unique<FEM::Problem<dim>>(std::move(mesh), std::move(physics));

    problem->addDirichletBC(0, 100.0);
    problem->addDirichletBC(10, 0.0);
    problem->assemble();
    problem->applyBCs();
    problem->solve();

    const auto& solution = problem->getSolution();
    ASSERT_NEAR(solution(5), 50.0, 1e-9);
    FEM::IO::Exporter::write_vtk("heat_1d_results.vtk", *problem);
}

// 2D Test
TEST_F(HeatTransferTest, Solves2DProblem) {
    constexpr int dim = 2;
    constexpr int num_nodes_per_elem = 4;
    const int nx = 10, ny = 10;

    auto mesh = FEM::Mesh::create_uniform_2d_mesh(1.0, 1.0, nx, ny);
    auto physics = std::make_unique<FEM::HeatTransfer<dim>>();

    physics->addKernel(
        std::make_unique<FEM::HeatDiffusionKernel<dim, num_nodes_per_elem>>(*material)
    );

    auto problem = std::make_unique<FEM::Problem<dim>>(std::move(mesh), std::move(physics));

    const auto& nodes = problem->getMesh().getNodes();
    for(const auto& node : nodes) {
        const auto& coords = node->getCoords();
        if (std::abs(coords[0] - 0.0) < 1e-9) problem->addDirichletBC(node->getId(), 100.0);
        if (std::abs(coords[0] - 1.0) < 1e-9) problem->addDirichletBC(node->getId(), 0.0);
    }

    problem->assemble();
    problem->applyBCs();
    problem->solve();

    const auto& solution = problem->getSolution();
    int center_node_id = (ny + 1) * (nx / 2) + (ny / 2);
    ASSERT_NEAR(solution(center_node_id), 50.0, 1e-9);
    FEM::IO::Exporter::write_vtk("heat_2d_results.vtk", *problem);
}

// 3D Test
TEST_F(HeatTransferTest, Solves3DProblem) {
    constexpr int dim = 3;
    constexpr int num_nodes_per_elem = 8;
    const int nx = 5, ny = 5, nz = 5;

    auto mesh = FEM::Mesh::create_uniform_3d_mesh(1.0, 1.0, 1.0, nx, ny, nz);
    auto physics = std::make_unique<FEM::HeatTransfer<dim>>();

    physics->addKernel(
        std::make_unique<FEM::HeatDiffusionKernel<dim, num_nodes_per_elem>>(*material)
    );

    auto problem = std::make_unique<FEM::Problem<dim>>(std::move(mesh), std::move(physics));

    const auto& nodes = problem->getMesh().getNodes();
    for(const auto& node : nodes) {
        const auto& coords = node->getCoords();
        if (std::abs(coords[0] - 0.0) < 1e-9) problem->addDirichletBC(node->getId(), 100.0);
        if (std::abs(coords[0] - 1.0) < 1e-9) problem->addDirichletBC(node->getId(), 0.0);
    }

    problem->assemble();
    problem->applyBCs();
    problem->solve();

    const auto& solution = problem->getSolution();
    int center_node_id = (nz/2)*(nx+1)*(ny+1) + (ny/2)*(nx+1) + (nx/2);
    ASSERT_NEAR(solution(center_node_id), 60.0, 1e-9); // 修正后的期望值
    FEM::IO::Exporter::write_vtk("heat_3d_results.vtk", *problem);
}