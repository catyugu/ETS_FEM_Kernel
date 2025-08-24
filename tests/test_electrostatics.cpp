#include <gtest/gtest.h>
#include <iostream>
#include <cmath>
#include <limits>
#include <io/Exporter.hpp>
#include <kernels/ElectrostaticsKernel.hpp>
#include <physics/Electrostatics.hpp>
#include <utils/Profiler.hpp>

#include "core/Problem.hpp"
#include "materials/Material.hpp"
#include "kernels/HeatDiffusionKernel.hpp"
#include "io/Importer.hpp"
#include "bcs/DirichletBC.hpp"
#include "mesh/Boundary.hpp"
#include "test_utils.hpp"

using namespace FEM;
using namespace FEM::IO;
#undef max
#undef min

class TestElectrostatics : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建铜材料并设置电导率
        material = std::make_unique<Material>("Copper");
        material->setProperty("permittivity", 5.96e7); // S/m
        ::Utils::Profiler::instance().setEnabled(true);
        ::Utils::Profiler::instance().reset();
    }

    std::unique_ptr<Material> material;
};

TEST_F(TestElectrostatics, SolveElectrostaticsOnImportedMesh) {

    {

        ::Utils::Profiler::instance().begin("Test::Input");
        std::unique_ptr<Mesh> mesh;
        try {
            mesh = Importer::read_comsol_mphtxt("data/electroOnlyMesh_3D.mphtxt");
        } catch (const std::exception& e) {
            FAIL() << "Failed to read mesh file: " << e.what();
        }

        ASSERT_NE(mesh, nullptr) << "Mesh should not be null";
        ASSERT_GT(mesh->getNodes().size(), 0) << "Mesh should have nodes";
        ASSERT_GT(mesh->getElements().size(), 0) << "Mesh should have elements";

        std::cout << "Mesh has " << mesh->getNodes().size() << " nodes and "
                  << mesh->getElements().size() << " elements" << std::endl;

        ::Utils::Profiler::instance().end();

        ::Utils::Profiler::instance().begin("Test::SetupPhysics");
        // 创建静电场物理场
        constexpr int dim = 3;
        auto physics = std::make_unique<Electrostatics<dim>>();

        physics->addKernel(
            std::make_unique<ElectrostaticsKernel<dim>>(*material)
        );

        // 使用基于坐标的边界条件设置
        const auto& nodes = mesh->getNodes();

        int left_bcs = 0;
        int right_bcs = 0;

        double min_x = nodes[0]->getCoords()[0];
        double max_x = min_x;
        for (const auto& node : nodes) {
            const auto& coords = node->getCoords();
            min_x = std::min(min_x, coords[0]);
            max_x = std::max(max_x, coords[0]);
        }

        double tolerance = 1e-8; // 使用相对容差

        std::cout << "X range: [" << min_x << ", " << max_x << "], tolerance: " << tolerance << std::endl;

        // 创建临时存储边界节点的容器
        std::vector<int> left_boundary_nodes, right_boundary_nodes;
        
        for (const auto& node : nodes) {
            const auto& coords = node->getCoords();
            // 左侧边界 (x ≈ min_x)
            if (std::abs(coords[0] - min_x) < tolerance) {
                left_boundary_nodes.push_back(node->getId());
                left_bcs++;
            }
            // 右侧边界 (x ≈ max_x)
            else if (std::abs(coords[0] - max_x) < tolerance) {
                right_boundary_nodes.push_back(node->getId());
                right_bcs++;
            }
        }

        // 创建Geometry对象并添加边界定义
        auto geometry = std::make_unique<Geometry>(std::move(mesh));
        
        // 添加边界定义到几何对象
        auto left_bnd = std::make_unique<Boundary>("left_boundary");
        for (int node_id : left_boundary_nodes) {
            auto node = geometry->getMesh().getNodeById(node_id);
            if (node) {
                left_bnd->addElement(std::make_unique<PointElement>(node_id, std::vector<Node*>{node}));
            }
        }
        geometry->addBoundary(std::move(left_bnd));
        
        auto right_bnd = std::make_unique<Boundary>("right_boundary");
        for (int node_id : right_boundary_nodes) {
            auto node = geometry->getMesh().getNodeById(node_id);
            if (node) {
                right_bnd->addElement(std::make_unique<PointElement>(node_id, std::vector<Node*>{node}));
            }
        }
        geometry->addBoundary(std::move(right_bnd));

        // 添加边界条件到物理场
        physics->addBoundaryCondition(
            std::make_unique<DirichletBC<dim>>(physics->getVariableName(), "left_boundary", 0.1)
        );
        physics->addBoundaryCondition(
            std::make_unique<DirichletBC<dim>>(physics->getVariableName(), "right_boundary", 0.0)
        );

        ASSERT_GT(left_bcs, 0) << "No left boundary conditions set";
        ASSERT_GT(right_bcs, 0) << "No right boundary conditions set";

        std::cout << "Left boundary conditions: " << left_bcs << std::endl;
        std::cout << "Right boundary conditions: " << right_bcs << std::endl;

        // 创建问题实例
        auto problem = std::make_unique<Problem<dim>>(std::move(geometry), std::move(physics), SolverType::SparseLU);

        ::Utils::Profiler::instance().end();
        // 组装和求解
        EXPECT_NO_THROW(problem->assemble()) << "Assembly should not throw";
        EXPECT_NO_THROW(problem->solve()) << "Solving should not throw";


        ::Utils::Profiler::instance().begin("Test::PostProcessing");
        // 读取参考数据 - 直接读取电势数据
        std::vector<double> ref_potential_data;
        std::unique_ptr<Mesh> ref_mesh;
        try {
            auto result = Importer::read_vtu_point_data_field("data/electroOnlyResults_3D.vtu", "&#x7535;&#x52bf;");
            ref_mesh = std::move(result.first);
            ref_potential_data = std::move(result.second);
        } catch (const std::exception& e) {
            FAIL() << "Failed to read reference data: " << e.what();
        }

        ASSERT_FALSE(ref_potential_data.empty()) << "Reference data should not be empty";
        ASSERT_EQ(ref_potential_data.size(), ref_mesh->getNodes().size())
            << "Reference data size should match reference mesh node count";

        // 比较计算结果与参考数据
        const auto& solution = problem->getSolution();
        const auto& problem_nodes = problem->getMesh().getNodes();
        const auto& reference_nodes = ref_mesh->getNodes();

        // 确保节点数量一致
        ASSERT_EQ(reference_nodes.size(), ref_potential_data.size())
            << "Number of nodes in reference solution should match reference data";

        // 计算误差
        double max_error = 0.0;
        double rms_error = 0.0;
        int error_count = 0;
        int finite_count = 0;
        int matched_count = 0;

        // 为每个参考节点找到对应的计算节点并比较结果
        for (size_t i = 0; i < reference_nodes.size(); ++i) {
            const auto& ref_coords = reference_nodes[i]->getCoords();
            // 在计算网格中找到最接近的节点
            int matched_index = findClosestNode(problem_nodes, ref_coords);

            if (matched_index != -1) {
                matched_count++;
                int dof_index = problem->getDofManager().getNodeDof("Voltage", problem_nodes[matched_index]->getId());
                double computed_value = solution(dof_index);
                double reference_value = ref_potential_data[i];

                // 检查是否是有效数值
                if (std::isfinite(computed_value) && std::isfinite(reference_value)) {
                    finite_count++;
                    double error = std::abs(computed_value - reference_value);
                    max_error = std::max(max_error, error);
                    rms_error += error * error;

                    // 检查个别点的值是否在合理范围内
                    if (i % 50 == 0) { // 每50个点检查一次
                        EXPECT_NEAR(computed_value, reference_value, 0.02)
                            << "Potential at node " << i << " differs significantly";
                    }
                } else {
                    error_count++;
                }
            }
        }

        if (error_count > 0) {
            std::cout << "Warning: " << error_count << " nodes have non-finite values" << std::endl;
        }

        if (finite_count > 0) {
            rms_error = std::sqrt(rms_error / finite_count);
        } else {
            rms_error = std::numeric_limits<double>::infinity();
        }

        std::cout << "Matched nodes: " << matched_count << "/" << reference_nodes.size() << std::endl;
        std::cout << "Max error: " << max_error << std::endl;
        std::cout << "RMS error: " << rms_error << std::endl;
        std::cout << "Finite values: " << finite_count << "/" << matched_count << std::endl;

        // 整体误差应该在合理范围内
        EXPECT_LT(max_error, 0.03) << "Maximum error should be less than 0.03V";
        EXPECT_LT(rms_error, 0.01) << "RMS error should be less than 0.01V";

        // 确保大部分节点都有有效解
        EXPECT_GT(matched_count, reference_nodes.size() * 0.95)
            << "Less than 95% of reference nodes were matched";

        ::Utils::Profiler::instance().end();
    }
    //  打印Profiler分析报告
    std::cout<<::Utils::Profiler::instance().getReport();
}