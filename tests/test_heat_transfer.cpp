#include <gtest/gtest.h>
#include <iostream>
#include <cmath>
#include <limits>
#include <io/Exporter.hpp>
#include <kernels/HeatDiffusionKernel.hpp>
#include <physics/HeatTransfer.hpp>
#include <utils/Profiler.hpp>

#include "core/Problem.hpp"
#include "materials/Material.hpp"
#include "kernels/ElectrostaticsKernel.hpp"
#include "io/Importer.hpp"
#include "physics/Electrostatics.hpp"
// 添加边界条件头文件
#include "bcs/DirichletBC.hpp"

using namespace FEM;
using namespace FEM::IO;
#undef max
#undef min
extern int findClosestNode(const std::vector<Node*>& nodes, const std::vector<double>& target_coords, double tolerance = 1e-10);
// 添加一个辅助函数，用于根据坐标找到最接近的节点
// int findClosestNode(const std::vector<Node*>& nodes, const std::vector<double>& target_coords, double tolerance = 1e-10) {
//     int closest_index = -1;
//     double min_distance = std::numeric_limits<double>::max();
//
//     for (size_t i = 0; i < nodes.size(); ++i) {
//         const auto& coords = nodes[i]->getCoords();
//
//         double distance = 0.0;
//         for (size_t j = 0; j < coords.size(); ++j) {
//             distance += (coords[j] - target_coords[j]) * (coords[j] - target_coords[j]);
//         }
//
//         if (distance < min_distance) {
//             min_distance = distance;
//             closest_index = i;
//         }
//     }
//
//     if (min_distance < tolerance) {
//         return closest_index;
//     } else {
//         return -1;
//     }
// }

class TestHeatTransfer : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建铜材料并设置热导率
        material = std::make_unique<Material>("Copper");
        material->setProperty("thermal_conductivity", 401.0);
    }

    std::unique_ptr<Material> material;
};

TEST_F(TestHeatTransfer, SolveHeatTransferOnImportedMesh) {

    {

        ::Utils::Profiler::instance().begin("Input");
        std::unique_ptr<Mesh> mesh;
        try {
            mesh = Importer::read_comsol_mphtxt("data/thermalOnlyMesh_3D.mphtxt");
        } catch (const std::exception& e) {
            FAIL() << "Failed to read mesh file: " << e.what();
        }

        ASSERT_NE(mesh, nullptr) << "Mesh should not be null";
        ASSERT_GT(mesh->getNodes().size(), 0) << "Mesh should have nodes";
        ASSERT_GT(mesh->getElements().size(), 0) << "Mesh should have elements";

        std::cout << "Mesh has " << mesh->getNodes().size() << " nodes and "
                  << mesh->getElements().size() << " elements" << std::endl;

        ::Utils::Profiler::instance().end();

        constexpr int dim = 3;
        auto physics = std::make_unique<HeatTransfer< dim>>();

        constexpr int num_nodes_per_elem = 4;
        physics->addKernel(
            std::make_unique<HeatDiffusionKernel<dim, num_nodes_per_elem>>(*material)
        );

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

        double tolerance = 1e-7; // 使用相对容差

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

        // 为导入的网格添加边界信息
        for (int node_id : left_boundary_nodes) {
            mesh->addBoundaryNode("left_boundary", node_id);
        }
        for (int node_id : right_boundary_nodes) {
            mesh->addBoundaryNode("right_boundary", node_id);
        }

        // 添加边界条件到物理场
        physics->addBoundaryCondition(
            std::make_unique<DirichletBC<dim>>("left_boundary", 323.15)
        );
        physics->addBoundaryCondition(
            std::make_unique<DirichletBC<dim>>("right_boundary", 263.15)
        );

        ASSERT_GT(left_bcs, 0) << "No left boundary conditions set";
        ASSERT_GT(right_bcs, 0) << "No right boundary conditions set";

        std::cout << "Left boundary conditions: " << left_bcs << std::endl;
        std::cout << "Right boundary conditions: " << right_bcs << std::endl;

        // 创建问题实例
        auto problem = std::make_unique<Problem<dim>>(std::move(mesh), std::move(physics), SolverType::SparseLU);

        // 组装和求解
        EXPECT_NO_THROW(problem->assemble()) << "Assembly should not throw";
        EXPECT_NO_THROW(problem->solve()) << "Solving should not throw";

        // 读取参考数据 - 直接读取温度数据
        std::vector<double> ref_temperature_data;
        std::unique_ptr<Mesh> ref_mesh;
        try {
            auto result = Importer::read_vtu_point_data_field("data/thermalOnlyResults_3D.vtu", "&#x6e29;&#x5ea6;");
            ref_mesh = std::move(result.first);
            ref_temperature_data = std::move(result.second);
        } catch (const std::exception& e) {
            FAIL() << "Failed to read reference data: " << e.what();
        }

        ASSERT_FALSE(ref_temperature_data.empty()) << "Reference data should not be empty";
        ASSERT_EQ(ref_temperature_data.size(), ref_mesh->getNodes().size())
            << "Reference data size should match reference mesh node count";

        // 比较计算结果与参考数据
        const auto& solution = problem->getSolution();
        const auto& problem_nodes = problem->getMesh().getNodes();
        const auto& reference_nodes = ref_mesh->getNodes();

        // 确保节点数量一致
        ASSERT_EQ(reference_nodes.size(), ref_temperature_data.size())
            << "Number of nodes in reference solution should match reference data";

        // 计算误差
        double max_error = 0.0;
        double rms_error = 0.0;
        int error_count = 0;
        int finite_count = 0;
        int matched_count = 0;

        // 为每个参考节点找到对应的计算节点并比较结果
        for (size_t i = 0; i < reference_nodes.size(); ++i) {
            PROFILE_SCOPE("FindClosestNode");
            const auto& ref_coords = reference_nodes[i]->getCoords();
            // 在计算网格中找到最接近的节点
            int matched_index = findClosestNode(problem_nodes, ref_coords);

            if (matched_index != -1) {
                matched_count++;
                int dof_index = problem->getDofManager().getNodeDof(problem_nodes[matched_index]->getId(), 0);
                double computed_value = solution(dof_index);
                double reference_value = ref_temperature_data[i];

                // 检查是否是有效数值
                if (std::isfinite(computed_value) && std::isfinite(reference_value)) {
                    finite_count++;
                    double error = std::abs(computed_value - reference_value);
                    max_error = std::max(max_error, error);
                    rms_error += error * error;

                    // 检查个别点的值是否在合理范围内
                    if (i % 50 == 0) { // 每50个点检查一次
                        EXPECT_NEAR(computed_value, reference_value, 3)
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
        EXPECT_LT(max_error, 3) << "Maximum error should be less than 3K";
        EXPECT_LT(rms_error, 1) << "RMS error should be less than 1K";

        // 确保大部分节点都有有效解
        EXPECT_GT(matched_count, reference_nodes.size() * 0.95)
            << "Less than 95% of reference nodes were matched";

        FEM::IO::Exporter::write_vtk("test_heat_transfer.vtk", *problem);
    }
    //  打印Profiler分析报告
    std::cout<<::Utils::Profiler::instance().getReport();
}