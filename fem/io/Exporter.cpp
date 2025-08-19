#include "Exporter.hpp"
#include <fstream>
#include <stdexcept>

// --- 关键修正：包含 Problem 类的完整定义 ---
#include "../core/Problem.hpp"

namespace FEM::IO {

template<int TDim>
void Exporter::write_vtk(const std::string& filename, const Problem<TDim>& problem) {
    std::ofstream vtk_file(filename);
    if (!vtk_file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filename);
    }

    // 现在编译器知道了 Problem 类的完整定义，可以安全地调用这些方法
    const auto& mesh = problem.getMesh();
    const auto& solution = problem.getSolution();
    const auto& nodes = mesh.getNodes();
    const auto& elements = mesh.getElements();

    // 1. VTK Header
    vtk_file << "# vtk DataFile Version 3.0\n";
    vtk_file << "FEM Simulation Results\n";
    vtk_file << "ASCII\n";
    vtk_file << "DATASET UNSTRUCTURED_GRID\n";

    // 2. Points (Node Coordinates)
    vtk_file << "POINTS " << nodes.size() << " double\n";
    for (const auto& node : nodes) {
        const auto& coords = node->getCoords();
        vtk_file << coords[0] << " " << (coords.size() > 1 ? coords[1] : 0.0) << " " << (coords.size() > 2 ? coords[2] : 0.0) << "\n";
    }

    // 3. Cells (Element Connectivity)
    size_t total_cell_list_size = 0;
    for (const auto& elem : elements) {
        total_cell_list_size += elem->getNumNodes() + 1;
    }
    vtk_file << "CELLS " << elements.size() << " " << total_cell_list_size << "\n";
    for (const auto& elem : elements) {
        vtk_file << elem->getNumNodes();
        for (const auto& node : elem->getNodes()) {
            vtk_file << " " << node->getId();
        }
        vtk_file << "\n";
    }

    // 4. Cell Types
    vtk_file << "CELL_TYPES " << elements.size() << "\n";
    for (const auto& elem : elements) {
        // VTK Cell Type codes: 3=Line, 5=Triangle, 10=Tetrahedron
        if (elem->getNumNodes() == 2) vtk_file << "3\n";
        else if (elem->getNumNodes() == 3) vtk_file << "5\n";
        else if (elem->getNumNodes() == 4) vtk_file << "10\n";
        else vtk_file << "1\n"; // Vertex for unknown
    }

    // 5. Point Data (Nodal Solution)
    vtk_file << "POINT_DATA " << nodes.size() << "\n";
    vtk_file << "SCALARS Temperature double 1\n";
    vtk_file << "LOOKUP_TABLE default\n";
    for (size_t i = 0; i < nodes.size(); ++i) {
        // 假设解向量的索引与节点ID是一致的
        vtk_file << solution(i) << "\n";
    }
}

// 显式实例化模板函数，以避免链接错误
template void Exporter::write_vtk<1>(const std::string&, const Problem<1>&);
template void Exporter::write_vtk<2>(const std::string&, const Problem<2>&);
template void Exporter::write_vtk<3>(const std::string&, const Problem<3>&);

} // namespace FEM::IO