#include <gtest/gtest.h>
#include <iostream>
#include <cmath>
#include "core/Problem.hpp"
#include "materials/Material.hpp"
#include "kernels/ElectrostaticsKernel.hpp"
#include "io/Importer.hpp"
#include "physics/Electrostatics.hpp"

using namespace FEM;
using namespace FEM::IO;
#undef max
#undef min
class ImportedMeshElectrostaticsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 创建铜材料并设置电导率
        material = std::make_unique<Material>("Copper");
        material->setProperty("electrical_conductivity", 5.96e7); // S/m
    }

    std::unique_ptr<Material> material;
};

TEST_F(ImportedMeshElectrostaticsTest, SolveElectrostaticsOnImportedMesh) {
    // 读取网格文件
    std::unique_ptr<Mesh> mesh;
    try {
        mesh = Importer::read_comsol_mphtxt("data/electroThermalMesh_3D.mphtxt");
    } catch (const std::exception& e) {
        FAIL() << "Failed to read mesh file: " << e.what();
    }
    
    ASSERT_NE(mesh, nullptr) << "Mesh should not be null";
    ASSERT_GT(mesh->getNodes().size(), 0) << "Mesh should have nodes";
    ASSERT_GT(mesh->getElements().size(), 0) << "Mesh should have elements";
    
    std::cout << "Mesh has " << mesh->getNodes().size() << " nodes and " 
              << mesh->getElements().size() << " elements" << std::endl;

    // 创建静电场物理场
    constexpr int dim = 3;
    auto physics = std::make_unique<Electrostatics<dim>>();

    // 添加静电场内核 (使用四面体单元，因为COMSOL网格文件中是四面体单元)
    constexpr int num_nodes_per_elem = 4;
    physics->addKernel(
        std::make_unique<ElectrostaticsKernel<dim, num_nodes_per_elem>>(*material)
    );

    // 创建问题实例
    auto problem = std::make_unique<Problem<dim>>(std::move(mesh), std::move(physics), SolverType::ConjugateGradient);

    // 设置边界条件
    // 左侧电势为0.1V，右侧电势为0V（接地）
    // 其他面为绝缘边界条件（自然边界条件，不需要显式设置）
    const auto& nodes = problem->getMesh().getNodes();
    
    int left_bcs = 0;
    int right_bcs = 0;

    double min_x = nodes[0]->getCoords()[0];
    double max_x = min_x;
    for (const auto& node : nodes) {
        const auto& coords = node->getCoords();
        min_x = std::min(min_x, coords[0]);
        max_x = std::max(max_x, coords[0]);
    }
    
    double x_range = max_x - min_x;
    double tolerance = x_range * 1e-3; // 使用相对容差
    
    std::cout << "X range: [" << min_x << ", " << max_x << "], tolerance: " << tolerance << std::endl;
    
    for (const auto& node : nodes) {
        const auto& coords = node->getCoords();
        // 左侧边界 (x ≈ min_x)
        if (std::abs(coords[0] - min_x) < tolerance) {
            problem->addDirichletBC(node->getId(), 0.1);  // 0.1V
            left_bcs++;
        }
        // 右侧边界 (x ≈ max_x)
        else if (std::abs(coords[0] - max_x) < tolerance) {
            problem->addDirichletBC(node->getId(), 0.0);  // 0V (接地)
            right_bcs++;
        }
    }
    
    ASSERT_GT(left_bcs, 0) << "No left boundary conditions set";
    ASSERT_GT(right_bcs, 0) << "No right boundary conditions set";
    
    std::cout << "Left boundary conditions: " << left_bcs << std::endl;
    std::cout << "Right boundary conditions: " << right_bcs << std::endl;

    // 组装和求解
    EXPECT_NO_THROW(problem->assemble()) << "Assembly should not throw";
    EXPECT_NO_THROW(problem->applyBCs()) << "Applying BCs should not throw";
    EXPECT_NO_THROW(problem->solve()) << "Solving should not throw";

    // 读取参考数据 - 直接读取电势数据
    std::unique_ptr<Mesh> ref_mesh;
    std::vector<double> ref_potential_data;
    try {
        auto result = Importer::read_vtu_point_data_field("data/electroThermalResults_3D.vtu", "&#x7535;&#x52bf;");
        ref_mesh = std::move(result.first);
        ref_potential_data = std::move(result.second);
    } catch (const std::exception& e) {
        FAIL() << "Failed to read reference data: " << e.what();
    }
    
    ASSERT_FALSE(ref_potential_data.empty()) << "Reference data should not be empty";
    ASSERT_EQ(ref_potential_data.size(), nodes.size()) 
        << "Reference data size should match node count";

    // 比较计算结果与参考数据
    const auto& solution = problem->getSolution();
    const auto& problem_nodes = problem->getMesh().getNodes();
    
    // 确保节点数量一致
    ASSERT_EQ(problem_nodes.size(), nodes.size()) 
        << "Number of nodes in solution should match mesh nodes";

    // 计算误差
    double max_error = 0.0;
    double rms_error = 0.0;
    int error_count = 0;
    int finite_count = 0;
    
    for (size_t i = 0; i < problem_nodes.size(); ++i) {
        int dof_index = problem->getDofManager().getNodeDof(problem_nodes[i]->getId(), 0);
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
                EXPECT_NEAR(computed_value, reference_value, 0.05) 
                    << "Potential at node " << i << " differs significantly";
            }
        } else {
            error_count++;
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
    
    std::cout << "Max error: " << max_error << std::endl;
    std::cout << "RMS error: " << rms_error << std::endl;
    std::cout << "Finite values: " << finite_count << "/" << problem_nodes.size() << std::endl;
    
    // 整体误差应该在合理范围内
    EXPECT_LT(max_error, 0.1) << "Maximum error should be less than 0.1V";
    EXPECT_LT(rms_error, 0.02) << "RMS error should be less than 0.02V";
    
    // 确保大部分节点都有有效解
    EXPECT_LT(static_cast<double>(error_count) / problem_nodes.size(), 0.01) 
        << "More than 1% of nodes have non-finite values";
}