#include "gtest/gtest.h"
#include "../fem/mesh/Mesh.hpp"
#include "../fem/mesh/Element.hpp"
#include "../fem/mesh/Node.hpp"
#include "../fem/materials/Material.hpp"
#include "../fem/kernels/HeatDiffusionKernel.hpp"
#include <Eigen/Dense>
#include <memory>
#include <vector>

void ASSERT_MATRIX_NEAR(const Eigen::MatrixXd& actual, const Eigen::MatrixXd& expected, double tolerance) {
    ASSERT_EQ(actual.rows(), expected.rows());
    ASSERT_EQ(actual.cols(), expected.cols());
    for (int i = 0; i < actual.rows(); ++i) {
        for (int j = 0; j < actual.cols(); ++j) {
            ASSERT_NEAR(actual(i, j), expected(i, j), tolerance)
                << "Mismatch at (" << i << ", " << j << ")";
        }
    }
}

class TetrahedronStiffnessMatrixTest : public ::testing::Test {
protected:
    std::unique_ptr<FEM::Mesh> mesh;
    std::unique_ptr<FEM::Element> tetElement;
    std::vector<FEM::Node*> elementNodes;

    void SetUp() override {
        // 1. 创建一个单元为标准四面体的网格
        // 节点位于: (0,0,0), (1,0,0), (0,1,0), (0,0,1)
        mesh = std::make_unique<FEM::Mesh>();
        std::vector<std::unique_ptr<FEM::Node>> nodes;
        nodes.push_back(std::make_unique<FEM::Node>(0, std::vector<double>{0.0, 0.0, 0.0}));
        nodes.push_back(std::make_unique<FEM::Node>(1, std::vector<double>{1.0, 0.0, 0.0}));
        nodes.push_back(std::make_unique<FEM::Node>(2, std::vector<double>{0.0, 1.0, 0.0}));
        nodes.push_back(std::make_unique<FEM::Node>(3, std::vector<double>{0.0, 0.0, 1.0}));

        // 将节点添加到网格中
        for (auto& node : nodes) {
            elementNodes.push_back(node.get()); // 保存原始指针给单元使用
            mesh->addNode(std::move(node));
        }

        // 创建一个四面体单元
        std::vector<int> node_ids = {0, 1, 2, 3};
        tetElement = std::make_unique<FEM::TetraElement>(0, elementNodes);

        // 2. 定义材料

    }
};

// --- 测试开始 ---
TEST_F(TetrahedronStiffnessMatrixTest, CompareWithAnalyticalSolution) {
    auto testMaterial = FEM::Material("TestMaterial");
    testMaterial.setProperty("thermal_conductivity", 1.0); // 设置热导率为1.0以简化计算
    FEM::HeatDiffusionKernel<3> kernel(testMaterial);

    // 4. 计算单元刚度矩阵
    Eigen::MatrixXd K_computed = kernel.compute_element_matrix(*tetElement);

    // 5. 定义标准四面体单元的理论刚度矩阵 (热导率为1)
    // 这个矩阵是基于标准有限元理论推导出来的。
    // 对于体积为 1/6 的标准四面体，其刚度矩阵具有以下形式。
    Eigen::MatrixXd K_expected(4, 4);
    K_expected <<
         3, -1, -1, -1,
        -1,  1,  0,  0,
        -1,  0,  1,  0,
        -1,  0,  0,  1;

    K_expected *= (1.0 / 6.0);

    // 6. 比较计算结果和理论结果
    ASSERT_EQ(K_computed.rows(), 4);
    ASSERT_EQ(K_computed.cols(), 4);

    // 打印矩阵以便调试
    std::cout << "Computed Stiffness Matrix (Tetrahedron):\n" << K_computed << std::endl;
    std::cout << "Expected Stiffness Matrix (Tetrahedron):\n" << K_expected << std::endl;

    ASSERT_MATRIX_NEAR(K_computed, K_expected, 1e-9);
}