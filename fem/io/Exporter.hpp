#pragma once

#include <string>
#include <set>
#include <fstream>
#include <stdexcept>
#include "../core/Problem.hpp"
#include <complex>
#include <iomanip>

namespace FEM {
    namespace IO {
        class Exporter {
        public:
            /**
             * @brief 将仿真结果写入VTK文件，可选择指定要导出的变量。
             * @tparam TDim 维度
             * @tparam TScalar 数据类型
             * @tparam Args 变量名参数包
             * @param filename 输出文件名
             * @param problem Problem对象的引用
             * @param var_names 要导出的一个或多个变量的名称。如果未提供，则导出所有变量。
             */
            template<int TDim, typename TScalar, typename... Args>
            static void write_vtk(const std::string& filename, const Problem<TDim, TScalar>& problem, Args... var_names) {
                // 将所有传入的变量名参数收集到一个 set 中，方便查找
                const std::set<std::string> variables_to_export = {var_names...};

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
                
                // 调用新的、带导出列表的 write_solution_data
                write_solution_data(file, problem, variables_to_export);
            }
            
            // 向后兼容的重载版本 - 导出所有变量
            template<int TDim, typename TScalar>
            static void write_vtk(const std::string& filename, const Problem<TDim, TScalar>& problem) {
                write_vtk<TDim, TScalar>(filename, problem); // 调用可变参数版本，但不传递任何变量名
            }
            
        private:
            template<typename TScalar>
            static std::string getTypeName() {
                if (std::is_same_v<TScalar, float>) return "float";
                if (std::is_same_v<TScalar, double>) return "double";
                if (std::is_same_v<TScalar, int>) return "int";
                return "unknown";
            }
            
            template<int TDim>
            static int getVtkCellType(ElementType type) {
                switch (type) {
                    case ElementType::Line: return 3;
                    case ElementType::Triangle: return 5;
                    case ElementType::Quadrilateral: return 9;
                    case ElementType::Tetrahedron: return 10;
                    case ElementType::Hexahedron: return 12;
                    default: return 1; // VTK_VERTEX
                }
            }

            template<int TDim, typename TScalar>
            static void write_mesh_data(std::ofstream& file, const Problem<TDim, TScalar>& problem) {
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
            
            /**
             * @brief 将指定的解数据写入文件。
             * @param file 输出文件流
             * @param problem Problem对象的引用
             * @param variables_to_export 一个包含要导出变量名的集合。如果为空，则导出所有变量。
             */
            template<int TDim, typename TScalar>
            static void write_solution_data(std::ofstream& file, const Problem<TDim, TScalar>& problem, 
                                 const std::set<std::string>& variables_to_export) {
                const auto& mesh = problem.getMesh();
                const auto& dof_manager = problem.getDofManager();
                const size_t num_nodes = mesh.getNodes().size();

                // 写入 POINT_DATA 头
                file << "\nPOINT_DATA " << num_nodes << "\n";

                // 遍历 Problem 中的每一个物理场
                for (size_t i = 0; i < problem.getNumPhysicsFields(); ++i) {
                    const auto& physics = problem.getPhysicsField(i);
                    const std::string& var_name = physics.getVariableName();

                    // 核心逻辑: 检查此变量是否需要导出
                    // 1. 如果导出列表为空 (用户未指定)，则导出该变量。
                    // 2. 如果导出列表不为空，则仅当该变量名在列表中时才导出。
                    if (!variables_to_export.empty() && variables_to_export.find(var_name) == variables_to_export.end()) {
                        continue; // 跳过这个变量
                    }

                    // 为每个需要导出的变量写入一组标量数据
                    file << "SCALARS " << var_name << " " << getTypeName<TScalar>() << " 1\n";
                    file << "LOOKUP_TABLE default\n";

                    for (const auto& node : mesh.getNodes()) {
                        int node_id = node->getId();
                        // 使用新的、带变量名的接口获取自由度索引
                        int dof_index = dof_manager.getNodeDof(var_name, node_id);
                        if (dof_index != -1) {
                            if constexpr (std::is_same_v<TScalar, std::complex<double>>) {
                                // 对于复数，输出模
                                file << std::setprecision(10) << std::abs(problem.getSolution()(dof_index)) << "\n";
                            } else {
                                file << std::setprecision(10) << problem.getSolution()(dof_index) << "\n";
                            }
                        } else {
                            file << "0\n"; // 或者其他默认值，例如 nan
                        }
                    }
                }
            }
        };
    }
}