#include <gtest/gtest.h>
#include <algorithm>
#include <iostream>
#include "../fem/io/Importer.hpp"
#include "../fem/mesh/Element.hpp"

using namespace FEM;
using namespace FEM::IO;


class VTUImportTest : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(VTUImportTest, ReadVTUFile) {
    auto [mesh, point_data] = Importer::read_vtu("data/electroThermalResults_3D.vtu");

    ASSERT_NE(mesh, nullptr) << "Mesh should not be null";

    EXPECT_GT(mesh->getNodes().size(), 0) << "Mesh should have nodes";
    std::cout << "Number of nodes: " << mesh->getNodes().size() << std::endl;

    EXPECT_GT(mesh->getElements().size(), 0) << "Mesh should have elements";
    std::cout << "Number of elements: " << mesh->getElements().size() << std::endl;

    const auto& nodes = mesh->getNodes();
    for (size_t i = 0; i < std::min(size_t(5), nodes.size()); ++i) {
        const auto& node = nodes[i];
        const auto& coords = node->getCoords();
        std::cout << "Node " << node->getId() << " coordinates: (";
        for (size_t j = 0; j < coords.size(); ++j) {
            if (j > 0) std::cout << ", ";
            std::cout << coords[j];
        }
        std::cout << ")" << std::endl;
    }

    const auto& elements = mesh->getElements();
    for (size_t i = 0; i < std::min(size_t(5), elements.size()); ++i) {
        const auto& element = elements[i];
        std::cout << "Element " << element->getId() << " (Type: ";

        switch (element->getType()) {
            case ElementType::Line:
                std::cout << "Line";
                break;
            case ElementType::Triangle:
                std::cout << "Triangle";
                break;
            case ElementType::Quadrilateral:
                std::cout << "Quadrilateral";
                break;
            case ElementType::Tetrahedron:
                std::cout << "Tetrahedron";
                break;
            case ElementType::Hexahedron:
                std::cout << "Hexahedron";
                break;
        }
        
        std::cout << ", Nodes: " << element->getNumNodes() << "): ";

        const auto& element_nodes = element->getNodes();
        for (size_t j = 0; j < element_nodes.size(); ++j) {
            if (j > 0) std::cout << ", ";
            std::cout << element_nodes[j]->getId();
        }
        std::cout << std::endl;
    }
    
    // 检查点数据
    if (!point_data.empty()) {
        std::cout << "Point data size: " << point_data.size() << std::endl;
        for (size_t i = 0; i < std::min(size_t(5), point_data.size()); ++i) {  // 只检查前5个数据
            std::cout << "Point " << i << " data: " << point_data[i] << std::endl;
        }
    }
}

// 测试读取不存在的VTU文件
TEST_F(VTUImportTest, ReadNonExistentVTUFile) {
    ASSERT_THROW(Importer::read_vtu("nonexistent.vtu"), std::runtime_error);
}