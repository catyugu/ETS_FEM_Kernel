#include "Importer.hpp"
#include "../mesh/Element.hpp" // 需要包含具体的单元类型
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <stdexcept>

void trim(std::string &s) {
    s.erase(0, s.find_first_not_of(" \t\n\r"));
    s.erase(s.find_last_not_of(" \t\n\r") + 1);
}

namespace FEM::IO {
    std::unique_ptr<Mesh> Importer::read_comsol_mphtxt(const std::string &filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open mesh file: " + filename);
        }

        auto mesh = std::make_unique<Mesh>();
        std::string line;
        int num_nodes = 0;

        const std::map<std::string, int> vertices_per_element_map = {
            {"vtx", 1}, {"edg", 2}, {"tri", 3}, {"tet", 4},
        };

        // --- Parser state flags ---
        bool reading_nodes = false;
        bool reading_elements = false;
        bool next_line_is_element_type = false; // Flag to handle two-line element type definition

        std::string current_element_type_str;
        int current_vertices_per_element = 0;
        int num_elements_to_read = 0;

        while (std::getline(file, line)) {
            trim(line);
            if (line.empty()) continue;

            // --- State machine for parsing element blocks ---
            if (next_line_is_element_type) {
                std::stringstream ss(line);
                int dummy_prefix;
                std::string type_string_with_comment;

                ss >> dummy_prefix >> type_string_with_comment; // Reads "3", then "vtx"

                // Remove trailing comment from the type name if it exists
                size_t comment_pos = type_string_with_comment.find('#');
                if (comment_pos != std::string::npos) {
                    current_element_type_str = type_string_with_comment.substr(0, comment_pos);
                    trim(current_element_type_str);
                } else {
                    current_element_type_str = type_string_with_comment;
                }

                auto it = vertices_per_element_map.find(current_element_type_str);
                if (it != vertices_per_element_map.end()) {
                    current_vertices_per_element = it->second;
                } else {
                    current_element_type_str.clear(); // Unsupported type
                }
                next_line_is_element_type = false;
                continue;
            }

            // --- Check for section headers and markers ---
            if (line.find("# number of mesh vertices") != std::string::npos) {
                std::stringstream ss(line);
                ss >> num_nodes;
                continue;
            }
            if (line.find("# Mesh vertex coordinates") != std::string::npos) {
                if (num_nodes == 0) {
                    throw std::runtime_error("Vertex coordinates section found before number of vertices was defined.");
                }
                reading_nodes = true;
                continue;
            }
            if (line.find("# Type #") != std::string::npos) {
                // This line is a marker. The next non-empty line contains the element type name.
                next_line_is_element_type = true;
                reading_elements = false;
                num_elements_to_read = 0;
                current_vertices_per_element = 0;
                current_element_type_str.clear();
                continue;
            }
            if (line.find("# number of elements") != std::string::npos) {
                if (current_element_type_str.empty()) continue; // Skip if we don't have a valid type
                std::stringstream ss(line);
                ss >> num_elements_to_read;
                continue;
            }
            if (line.find("# Elements") != std::string::npos) {
                // This is the header right before the element index data starts.
                if (num_elements_to_read > 0) {
                    reading_elements = true;
                }
                continue;
            }

            // Ignore any other lines that are just comments
            if (line[0] == '#') {
                continue;
            }

            // --- Read data based on the current state ---
            if (reading_nodes) {
                if (mesh->getNodes().size() >= static_cast<size_t>(num_nodes)) {
                    reading_nodes = false;
                    continue;
                }
                std::stringstream ss(line);
                double x, y, z;
                int dummy_prefix;

                // Handle both "x y z" and "prefix x y z" formats
                std::streampos original_pos = ss.tellg();
                if (ss >> dummy_prefix >> x >> y >> z) {
                    // Successfully read "prefix x y z"
                } else {
                    ss.clear();
                    ss.seekg(original_pos);
                    ss >> x >> y >> z; // Retry reading "x y z"
                }
                mesh->addNode(std::make_unique<Node>(mesh->getNodes().size(), std::vector<double>{x, y, z}));
            } else if (reading_elements) {
                if (num_elements_to_read <= 0) {
                    reading_elements = false;
                    continue;
                }

                std::stringstream ss(line);
                std::vector<int> node_indices(current_vertices_per_element);
                for (int i = 0; i < current_vertices_per_element; ++i) {
                    if (!(ss >> node_indices[i])) {
                        throw std::runtime_error("Error reading element node indices.");
                    }
                }

                std::vector<Node *> element_nodes;
                element_nodes.reserve(current_vertices_per_element);
                for (int idx: node_indices) {
                    if (idx >= num_nodes || idx < 0) {
                        throw std::runtime_error("Invalid node index found in element definition.");
                    }
                    element_nodes.push_back(mesh->getNodes()[idx].get());
                }

                if (current_element_type_str == "vtx") {
                    mesh->addElement(std::make_unique<PointElement>(mesh->getElements().size(), element_nodes));
                } else if (current_element_type_str == "edg") {
                    mesh->addElement(std::make_unique<LineElement>(mesh->getElements().size(), element_nodes));
                } else if (current_element_type_str == "tri") {
                    mesh->addElement(std::make_unique<TriElement>(mesh->getElements().size(), element_nodes));
                } else if (current_element_type_str == "tet") {
                    mesh->addElement(std::make_unique<TetraElement>(mesh->getElements().size(), element_nodes));
                }
                num_elements_to_read--;
            }
        }

        if (mesh->getNodes().size() != static_cast<size_t>(num_nodes)) {
            std::cerr << "Warning: Expected " << num_nodes << " nodes, but read " << mesh->getNodes().size() << "." <<
                    std::endl;
        }

        return mesh;
    }

    std::pair<std::unique_ptr<Mesh>, std::vector<double> > Importer::read_vtu(const std::string &filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open VTU file: " + filename);
        }

        auto mesh = std::make_unique<Mesh>();
        std::vector<double> point_data;

        std::string line;
        int num_points = 0;
        int num_cells = 0;

        // 存储解析过程中的数据
        std::vector<int> connectivity;
        std::vector<int> offsets;
        std::vector<int> types;
        bool in_point_data = false;
        bool in_points_section = false;
        bool in_cells_section = false;
        bool in_connectivity = false;
        bool in_offsets = false;
        bool in_types = false;
        bool in_cell_data = false;

        // 简化的VTU解析器
        while (std::getline(file, line)) {
            // 移除空白字符
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            if (line.empty()) continue;

            // 解析Piece元素获取点和单元数量
            if (line.find("<Piece") != std::string::npos) {
                // 提取NumberOfPoints和NumberOfCells属性
                size_t pos = line.find("NumberOfPoints=");
                if (pos != std::string::npos) {
                    size_t start = line.find('"', pos) + 1;
                    size_t end = line.find('"', start);
                    std::string num_points_str = line.substr(start, end - start);
                    num_points = std::stoi(num_points_str);
                }

                pos = line.find("NumberOfCells=");
                if (pos != std::string::npos) {
                    size_t start = line.find('"', pos) + 1;
                    size_t end = line.find('"', start);
                    std::string num_cells_str = line.substr(start, end - start);
                    num_cells = std::stoi(num_cells_str);
                }
            }
            // 开始解析点坐标部分
            else if (line.find("<Points>") != std::string::npos) {
                in_points_section = true;
                continue;
            } else if (line.find("</Points>") != std::string::npos) {
                in_points_section = false;
                continue;
            }
            // 开始解析单元部分
            else if (line.find("<Cells>") != std::string::npos) {
                in_cells_section = true;
                continue;
            } else if (line.find("</Cells>") != std::string::npos) {
                in_cells_section = false;
                continue;
            }
            // 开始解析点数据部分
            else if (line.find("<PointData") != std::string::npos) {
                in_point_data = true;
                continue;
            } else if (line.find("</PointData>") != std::string::npos) {
                in_point_data = false;
                continue;
            }
            // 开始解析单元数据部分
            else if (line.find("<CellData") != std::string::npos) {
                in_cell_data = true;
                continue;
            } else if (line.find("</CellData>") != std::string::npos) {
                in_cell_data = false;
                continue;
            }
            // 在点坐标部分解析数据
            else if (in_points_section && line.find("<DataArray") != std::string::npos) {
                // 读取所有点坐标数据
                std::stringstream data_ss;
                std::string data_line;
                int total_coords_read = 0;

                // 读取直到遇到</DataArray>
                while (std::getline(file, data_line)) {
                    // 移除空白字符
                    data_line.erase(0, data_line.find_first_not_of(" \t"));
                    data_line.erase(data_line.find_last_not_of(" \t") + 1);

                    if (data_line.find("</DataArray>") != std::string::npos) {
                        break;
                    }
                    data_ss << data_line << " ";
                }

                // 解析所有坐标
                double x, y, z;
                int points_read = 0;
                while (data_ss >> x >> y >> z && points_read < num_points) {
                    mesh->addNode(std::make_unique<Node>(points_read, std::vector<double>{x, y, z}));
                    points_read++;
                }

                if (points_read != num_points) {
                    throw std::runtime_error("Error parsing point coordinates: expected " +
                                             std::to_string(num_points) + " points, but read " +
                                             std::to_string(points_read));
                }
            }
            // 在点数据部分解析数据
            else if (in_point_data && line.find("<DataArray") != std::string::npos) {
                // 检查组件数量（如果有）
                int num_components = 1;
                size_t component_pos = line.find("NumberOfComponents=");
                if (component_pos != std::string::npos) {
                    size_t start = line.find('"', component_pos) + 1;
                    size_t end = line.find('"', start);
                    std::string comp_str = line.substr(start, end - start);
                    num_components = std::stoi(comp_str);
                }

                // 计算预期的总数据点数量
                int expected_values = num_points * num_components;

                // 读取所有点数据
                std::stringstream data_ss;
                std::string data_line;

                // 读取直到遇到</DataArray>
                while (std::getline(file, data_line)) {
                    // 移除空白字符
                    data_line.erase(0, data_line.find_first_not_of(" \t"));
                    data_line.erase(data_line.find_last_not_of(" \t") + 1);

                    if (data_line.find("</DataArray>") != std::string::npos) {
                        break;
                    }
                    data_ss << data_line << " ";
                }

                // 解析所有点数据
                double value;
                int data_read = 0;
                while (data_ss >> value && data_read < expected_values) {
                    point_data.push_back(value);
                    data_read++;
                }

                if (data_read != expected_values) {
                    throw std::runtime_error("Error parsing point data: expected " +
                                             std::to_string(expected_values) + " data values (" +
                                             std::to_string(num_points) + " points with " +
                                             std::to_string(num_components) + " components), but read " +
                                             std::to_string(data_read));
                }
            }
            // 在单元部分解析connectivity数据
            else if (in_cells_section && line.find("<DataArray") != std::string::npos && line.find(
                         "Name=\"connectivity\"") != std::string::npos) {
                // 读取所有connectivity数据
                std::stringstream data_ss;
                std::string data_line;

                // 读取直到遇到</DataArray>
                while (std::getline(file, data_line)) {
                    // 移除空白字符
                    data_line.erase(0, data_line.find_first_not_of(" \t"));
                    data_line.erase(data_line.find_last_not_of(" \t") + 1);

                    if (data_line.find("</DataArray>") != std::string::npos) {
                        break;
                    }
                    data_ss << data_line << " ";
                }

                // 解析所有connectivity数据
                int value;
                while (data_ss >> value) {
                    connectivity.push_back(value);
                }
            }
            // 在单元部分解析offsets数据
            else if (in_cells_section && line.find("<DataArray") != std::string::npos && line.find("Name=\"offsets\"")
                     != std::string::npos) {
                // 读取所有offsets数据
                std::stringstream data_ss;
                std::string data_line;

                // 读取直到遇到</DataArray>
                while (std::getline(file, data_line)) {
                    // 移除空白字符
                    data_line.erase(0, data_line.find_first_not_of(" \t"));
                    data_line.erase(data_line.find_last_not_of(" \t") + 1);

                    if (data_line.find("</DataArray>") != std::string::npos) {
                        break;
                    }
                    data_ss << data_line << " ";
                }

                // 解析所有offsets数据
                int value;
                while (data_ss >> value) {
                    offsets.push_back(value);
                }
            }
            // 在单元部分解析types数据
            else if (in_cells_section && line.find("<DataArray") != std::string::npos && line.find("Name=\"types\"") !=
                     std::string::npos) {
                // 读取所有types数据
                std::stringstream data_ss;
                std::string data_line;

                // 读取直到遇到</DataArray>
                while (std::getline(file, data_line)) {
                    // 移除空白字符
                    data_line.erase(0, data_line.find_first_not_of(" \t"));
                    data_line.erase(data_line.find_last_not_of(" \t") + 1);

                    if (data_line.find("</DataArray>") != std::string::npos) {
                        break;
                    }
                    data_ss << data_line << " ";
                }

                // 解析所有types数据
                int value;
                while (data_ss >> value) {
                    types.push_back(value);
                }
            }
        }

        // 根据VTK单元类型创建单元
        int conn_index = 0;
        for (int i = 0; i < num_cells; ++i) {
            int type = types[i];
            int offset = offsets[i] - (i > 0 ? offsets[i - 1] : 0); // 计算当前单元的节点数

            // 根据VTK单元类型创建对应的FEM单元
            switch (type) {
                case 5: // triangle
                    if (offset == 3) {
                        mesh->addElement(std::make_unique<TriElement>(i, std::vector<Node *>{
                                                                          mesh->getNodes()[connectivity[conn_index]].
                                                                          get(),
                                                                          mesh->getNodes()[connectivity[conn_index + 1]]
                                                                          .get(),
                                                                          mesh->getNodes()[connectivity[conn_index + 2]]
                                                                          .get()
                                                                      }));
                        conn_index += 3;
                    }
                    break;
                case 10: // tetrahedron
                    if (offset == 4) {
                        mesh->addElement(std::make_unique<TetraElement>(i, std::vector<Node *>{
                                                                            mesh->getNodes()[connectivity[conn_index]].
                                                                            get(),
                                                                            mesh->getNodes()[connectivity[
                                                                                conn_index + 1]].get(),
                                                                            mesh->getNodes()[connectivity[
                                                                                conn_index + 2]].get(),
                                                                            mesh->getNodes()[connectivity[
                                                                                conn_index + 3]].get()
                                                                        }));
                        conn_index += 4;
                    }
                    break;
                default:
                    // 跳过不支持的单元类型
                    conn_index += offset;
                    break;
            }
        }

        return std::make_pair(std::move(mesh), std::move(point_data));
    }

    std::pair<std::unique_ptr<Mesh>, std::vector<double> > Importer::read_vtu_point_data_field(
        const std::string &filename, const std::string &field_name) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open VTU file: " + filename);
        }

        auto mesh = std::make_unique<Mesh>();
        std::vector<double> point_data;

        std::string line;
        int num_points = 0;
        int num_cells = 0;

        // 存储解析过程中的数据
        std::vector<int> connectivity;
        std::vector<int> offsets;
        std::vector<int> types;
        bool in_point_data = false;
        bool in_points_section = false;
        bool in_cells_section = false;
        bool in_connectivity = false;
        bool in_offsets = false;
        bool in_types = false;
        bool in_cell_data = false;
        bool field_found = false;

        // 简化的VTU解析器
        while (std::getline(file, line)) {
            // 移除空白字符
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            if (line.empty()) continue;

            // 解析Piece元素获取点和单元数量
            if (line.find("<Piece") != std::string::npos) {
                // 提取NumberOfPoints和NumberOfCells属性
                size_t pos = line.find("NumberOfPoints=");
                if (pos != std::string::npos) {
                    size_t start = line.find('"', pos) + 1;
                    size_t end = line.find('"', start);
                    std::string num_points_str = line.substr(start, end - start);
                    num_points = std::stoi(num_points_str);
                }

                pos = line.find("NumberOfCells=");
                if (pos != std::string::npos) {
                    size_t start = line.find('"', pos) + 1;
                    size_t end = line.find('"', start);
                    std::string num_cells_str = line.substr(start, end - start);
                    num_cells = std::stoi(num_cells_str);
                }
            }
            // 开始解析点坐标部分
            else if (line.find("<Points>") != std::string::npos) {
                in_points_section = true;
                continue;
            } else if (line.find("</Points>") != std::string::npos) {
                in_points_section = false;
                continue;
            }
            // 开始解析单元部分
            else if (line.find("<Cells>") != std::string::npos) {
                in_cells_section = true;
                continue;
            } else if (line.find("</Cells>") != std::string::npos) {
                in_cells_section = false;
                continue;
            }
            // 开始解析点数据部分
            else if (line.find("<PointData") != std::string::npos) {
                in_point_data = true;
                continue;
            } else if (line.find("</PointData>") != std::string::npos) {
                in_point_data = false;
                continue;
            }
            // 开始解析单元数据部分
            else if (line.find("<CellData") != std::string::npos) {
                in_cell_data = true;
                continue;
            } else if (line.find("</CellData>") != std::string::npos) {
                in_cell_data = false;
                continue;
            }
            // 在点坐标部分解析数据
            else if (in_points_section && line.find("<DataArray") != std::string::npos) {
                // 读取所有点坐标数据
                std::stringstream data_ss;
                std::string data_line;
                int total_coords_read = 0;

                // 读取直到遇到</DataArray>
                while (std::getline(file, data_line)) {
                    // 移除空白字符
                    data_line.erase(0, data_line.find_first_not_of(" \t"));
                    data_line.erase(data_line.find_last_not_of(" \t") + 1);

                    if (data_line.find("</DataArray>") != std::string::npos) {
                        break;
                    }
                    data_ss << data_line << " ";
                }

                // 解析所有坐标
                double x, y, z;
                int points_read = 0;
                while (data_ss >> x >> y >> z && points_read < num_points) {
                    mesh->addNode(std::make_unique<Node>(points_read, std::vector<double>{x, y, z}));
                    points_read++;
                }

                if (points_read != num_points) {
                    throw std::runtime_error("Error parsing point coordinates: expected " +
                                             std::to_string(num_points) + " points, but read " +
                                             std::to_string(points_read));
                }
            }
            // 在点数据部分解析数据
            else if (in_point_data && line.find("<DataArray") != std::string::npos) {
                // 检查这个数据字段是否是我们要找的字段
                if (line.find("Name=\"" + field_name + "\"") != std::string::npos) {
                    field_found = true;

                    // 检查组件数量（如果有）
                    int num_components = 1;
                    size_t component_pos = line.find("NumberOfComponents=");
                    if (component_pos != std::string::npos) {
                        size_t start = line.find('"', component_pos) + 1;
                        size_t end = line.find('"', start);
                        std::string comp_str = line.substr(start, end - start);
                        num_components = std::stoi(comp_str);
                    }

                    // 计算预期的总数据点数量
                    int expected_values = num_points * num_components;

                    // 读取所有点数据
                    std::stringstream data_ss;
                    std::string data_line;

                    // 读取直到遇到</DataArray>
                    while (std::getline(file, data_line)) {
                        // 移除空白字符
                        data_line.erase(0, data_line.find_first_not_of(" \t"));
                        data_line.erase(data_line.find_last_not_of(" \t") + 1);

                        if (data_line.find("</DataArray>") != std::string::npos) {
                            break;
                        }
                        data_ss << data_line << " ";
                    }

                    // 解析所有点数据
                    double value;
                    int data_read = 0;
                    while (data_ss >> value && data_read < expected_values) {
                        point_data.push_back(value);
                        data_read++;
                    }

                    if (data_read != expected_values) {
                        throw std::runtime_error("Error parsing point data field '" + field_name + "': expected " +
                                                 std::to_string(expected_values) + " data values (" +
                                                 std::to_string(num_points) + " points with " +
                                                 std::to_string(num_components) + " components), but read " +
                                                 std::to_string(data_read));
                    }
                }
            }
            // 在单元部分解析connectivity数据
            else if (in_cells_section && line.find("<DataArray") != std::string::npos && line.find(
                         "Name=\"connectivity\"") != std::string::npos) {
                // 读取所有connectivity数据
                std::stringstream data_ss;
                std::string data_line;

                // 读取直到遇到</DataArray>
                while (std::getline(file, data_line)) {
                    // 移除空白字符
                    data_line.erase(0, data_line.find_first_not_of(" \t"));
                    data_line.erase(data_line.find_last_not_of(" \t") + 1);

                    if (data_line.find("</DataArray>") != std::string::npos) {
                        break;
                    }
                    data_ss << data_line << " ";
                }

                // 解析所有connectivity数据
                int value;
                while (data_ss >> value) {
                    connectivity.push_back(value);
                }
            }
            // 在单元部分解析offsets数据
            else if (in_cells_section && line.find("<DataArray") != std::string::npos && line.find("Name=\"offsets\"")
                     != std::string::npos) {
                // 读取所有offsets数据
                std::stringstream data_ss;
                std::string data_line;

                // 读取直到遇到</DataArray>
                while (std::getline(file, data_line)) {
                    // 移除空白字符
                    data_line.erase(0, data_line.find_first_not_of(" \t"));
                    data_line.erase(data_line.find_last_not_of(" \t") + 1);

                    if (data_line.find("</DataArray>") != std::string::npos) {
                        break;
                    }
                    data_ss << data_line << " ";
                }

                // 解析所有offsets数据
                int value;
                while (data_ss >> value) {
                    offsets.push_back(value);
                }
            }
            // 在单元部分解析types数据
            else if (in_cells_section && line.find("<DataArray") != std::string::npos && line.find("Name=\"types\"") !=
                     std::string::npos) {
                // 读取所有types数据
                std::stringstream data_ss;
                std::string data_line;

                // 读取直到遇到</DataArray>
                while (std::getline(file, data_line)) {
                    // 移除空白字符
                    data_line.erase(0, data_line.find_first_not_of(" \t"));
                    data_line.erase(data_line.find_last_not_of(" \t") + 1);

                    if (data_line.find("</DataArray>") != std::string::npos) {
                        break;
                    }
                    data_ss << data_line << " ";
                }

                // 解析所有types数据
                int value;
                while (data_ss >> value) {
                    types.push_back(value);
                }
            }
        }

        // 如果没有找到指定的字段，抛出异常
        if (!field_found) {
            throw std::runtime_error("Field '" + field_name + "' not found in VTU file");
        }

        // 根据VTK单元类型创建单元
        int conn_index = 0;
        for (int i = 0; i < num_cells; ++i) {
            int type = types[i];
            int offset = offsets[i] - (i > 0 ? offsets[i - 1] : 0); // 计算当前单元的节点数

            // 根据VTK单元类型创建对应的FEM单元
            switch (type) {
                case 5: // triangle
                    if (offset == 3) {
                        mesh->addElement(std::make_unique<TriElement>(i, std::vector<Node *>{
                                                                          mesh->getNodes()[connectivity[conn_index]].
                                                                          get(),
                                                                          mesh->getNodes()[connectivity[conn_index + 1]]
                                                                          .get(),
                                                                          mesh->getNodes()[connectivity[conn_index + 2]]
                                                                          .get()
                                                                      }));
                        conn_index += 3;
                    }
                    break;
                case 10: // tetrahedron
                    if (offset == 4) {
                        mesh->addElement(std::make_unique<TetraElement>(i, std::vector<Node *>{
                                                                            mesh->getNodes()[connectivity[conn_index]].
                                                                            get(),
                                                                            mesh->getNodes()[connectivity[
                                                                                conn_index + 1]].get(),
                                                                            mesh->getNodes()[connectivity[
                                                                                conn_index + 2]].get(),
                                                                            mesh->getNodes()[connectivity[
                                                                                conn_index + 3]].get()
                                                                        }));
                        conn_index += 4;
                    }
                    break;
                default:
                    // 跳过不支持的单元类型
                    conn_index += offset;
                    break;
            }
        }

        return std::make_pair(std::move(mesh), std::move(point_data));
    }
} // namespace FEM::IO
