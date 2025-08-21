#include "Exporter.hpp"
#include <iomanip>
#include <complex>

namespace FEM {
    namespace IO {
        
        template<int TDim, typename TScalar>
        void Exporter::write_vtk(const std::string& filename, const Problem<TDim, TScalar>& problem) {
            std::ofstream file(filename);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + filename);
            }

            // VTK 头部
            file << "# vtk DataFile Version 3.0\n";
            file << "FEM Solution\n";
            file << "ASCII\n";
            file << "DATASET UNSTRUCTURED_GRID\n";

            // 写入网格和解数据
            write_mesh_data(file, problem);
            write_solution_data(file, problem);
        }
        
        template<int TDim, typename TScalar>
        void Exporter::write_mesh_data(std::ofstream& file, const Problem<TDim, TScalar>& problem) {
            const auto& mesh = problem.getMesh();
            const auto& nodes = mesh.getNodes();
            const auto& elements = mesh.getElements();

            // 写入节点
            file << "POINTS " << nodes.size() << " double\n";
            for (const auto& node_ptr : nodes) {
                const auto& node = *node_ptr;
                if constexpr (TDim == 1) {
                    file << std::setprecision(10) << node.getX() << " 0.0 0.0\n";
                } else if constexpr (TDim == 2) {
                    file << std::setprecision(10) << node.getX() << " " << node.getY() << " 0.0\n";
                } else {
                    file << std::setprecision(10) << node.getX() << " " << node.getY() << " " << node.getZ() << "\n";
                }
            }

            // 写入单元
            file << "\nCELLS " << elements.size() << " " << elements.size() * (TDim + 2) << "\n";
            for (const auto& elem_ptr : elements) {
                const auto& elem = *elem_ptr;
                const auto& node_ids = elem.getNodeIds();
                file << (TDim + 1);
                for (const auto& node_id : node_ids) {
                    file << " " << node_id;
                }
                file << "\n";
            }

            // 写入单元类型
            file << "\nCELL_TYPES " << elements.size() << "\n";
            for (size_t i = 0; i < elements.size(); ++i) {
                // 5 = VTK_TRIANGLE, 10 = VTK_TETRA
                file << (TDim == 2 ? 5 : 10) << "\n";
            }
        }
        
        template<int TDim, typename TScalar>
        void Exporter::write_solution_data(std::ofstream& file, const Problem<TDim, TScalar>& problem) {
            const auto& mesh = problem.getMesh();
            const auto& solution = problem.getSolution();
            const auto& dof_manager = problem.getDofManager();
            const auto& nodes = mesh.getNodes();

            // 写入点数据
            file << "\nPOINT_DATA " << nodes.size() << "\n";

            // 写入标量解（温度或电势）
            file << "SCALARS solution double 1\n";
            file << "LOOKUP_TABLE default\n";
            for (size_t i = 0; i < nodes.size(); ++i) {
                int dof_index = dof_manager.getNodeDof(nodes[i]->getId(), 0);
                if constexpr (std::is_same_v<TScalar, std::complex<double>>) {
                    // 对于复数，输出模
                    file << std::setprecision(10) << std::abs(solution[dof_index]) << "\n";
                } else {
                    file << std::setprecision(10) << solution[dof_index] << "\n";
                }
            }
        }
        
        // 显式实例化定义
        template void Exporter::write_vtk<1, double>(const std::string&, const Problem<1, double>&);
        template void Exporter::write_vtk<2, double>(const std::string&, const Problem<2, double>&);
        template void Exporter::write_vtk<3, double>(const std::string&, const Problem<3, double>&);
        template void Exporter::write_vtk<1, std::complex<double>>(const std::string&, const Problem<1, std::complex<double>>&);
        template void Exporter::write_vtk<2, std::complex<double>>(const std::string&, const Problem<2, std::complex<double>>&);
        template void Exporter::write_vtk<3, std::complex<double>>(const std::string&, const Problem<3, std::complex<double>>&);
    }
}