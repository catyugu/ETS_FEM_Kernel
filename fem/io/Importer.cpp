#include "Importer.hpp"
#include "../mesh/Element.hpp" // 需要包含具体的单元类型
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <stdexcept>

namespace FEM::IO {
    std::unique_ptr<Mesh> Importer::read_comsol_mphtxt(const std::string &filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open mesh file: " + filename);
        }

        auto mesh = std::make_unique<Mesh>();
        std::string line;
        int num_nodes = 0;

        // 首先找到节点数量
        while (std::getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty() || line[0] == '%') continue;
            
            if (line.find("# number of mesh vertices") != std::string::npos) {
                // 找到节点数量所在的行
                std::stringstream ss(line);
                ss >> num_nodes; // 读取节点数量
                break;
            }
        }
        
        // 重新定位到文件开始，重新读取
        file.clear();
        file.seekg(0, std::ios::beg);
        
        bool in_vertex_coordinates = false;
        int nodes_read = 0;

        // 解析节点坐标
        while (std::getline(file, line) && nodes_read < num_nodes) {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty() || line[0] == '%') continue;
            
            // 检查是否是节点坐标部分开始
            if (line.find("# Mesh vertex coordinates") != std::string::npos) {
                in_vertex_coordinates = true;
                continue;
            }
            
            // 如果在节点坐标部分，读取坐标
            if (in_vertex_coordinates) {
                std::stringstream ss(line);
                double x, y, z;
                if (ss >> x >> y >> z) {
                    mesh->addNode(new Node(nodes_read, {x, y, z}));
                    nodes_read++;
                }
            }
        }

        // 重新定位到文件开始，解析单元
        file.clear();
        file.seekg(0, std::ios::beg);
        
        bool in_elements_section = false;
        int vertices_per_element = 0;
        int num_elements = 0;
        std::string element_type;
        
        while (std::getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);
            
            if (line.empty() || line[0] == '%') continue;
            
            // 检查是否进入单元部分
            if (line.find("# number of element types") != std::string::npos) {
                in_elements_section = true;
                continue;
            }
            
            // 检查单元类型
            if (in_elements_section) {
                // 查找单元类型名称
                if (line.find("edg") != std::string::npos) {
                    element_type = "edg";
                    vertices_per_element = 2; // 边单元
                    continue;
                } else if (line.find("tri") != std::string::npos) {
                    element_type = "tri";
                    vertices_per_element = 3; // 三角形单元
                    continue;
                } else if (line.find("tet") != std::string::npos) {
                    element_type = "tet";
                    vertices_per_element = 4; // 四面体单元
                    continue;
                } else if (line.find("vtx") != std::string::npos) {
                    element_type = "vtx";
                    vertices_per_element = 1; // 顶点单元
                    continue;
                }
                
                // 读取每个单元的顶点数
                if (line.find("# number of vertices per element") != std::string::npos) {
                    continue;
                }
                
                // 读取单元数量
                if (line.find("# number of elements") != std::string::npos) {
                    std::stringstream ss(line);
                    ss >> num_elements;
                    continue;
                }
                
                // 处理单元索引
                if (num_elements > 0 && vertices_per_element > 0 && line.find("#") == std::string::npos && !line.empty()) {
                    std::stringstream ss(line);
                    std::vector<int> node_indices(vertices_per_element);
                    bool valid = true;
                    
                    // 读取节点索引
                    for (int i = 0; i < vertices_per_element; i++) {
                        if (!(ss >> node_indices[i])) {
                            valid = false;
                            break;
                        }
                    }
                    
                    if (valid) {
                        bool indices_valid = true;
                        for (int i = 0; i < vertices_per_element; i++) {
                            if (node_indices[i] >= static_cast<int>(mesh->getNodes().size()) || 
                                node_indices[i] < 0) {
                                indices_valid = false;
                                break;
                            }
                        }
                        
                        if (indices_valid) {
                            // 创建相应类型的单元
                            if (vertices_per_element == 1) {
                                // 顶点单元 - 在此简单网格中我们不创建单元
                            } else if (vertices_per_element == 2) {
                                // 边单元
                                mesh->addElement(new LineElement(mesh->getElements().size(), {
                                    mesh->getNodes()[node_indices[0]], 
                                    mesh->getNodes()[node_indices[1]]
                                }));
                            } else if (vertices_per_element == 3) {
                                // 三角形单元
                                mesh->addElement(new TriElement(mesh->getElements().size(), {
                                    mesh->getNodes()[node_indices[0]], 
                                    mesh->getNodes()[node_indices[1]], 
                                    mesh->getNodes()[node_indices[2]]
                                }));
                            } else if (vertices_per_element == 4) {
                                // 四面体单元
                                mesh->addElement(new TetraElement(mesh->getElements().size(), {
                                    mesh->getNodes()[node_indices[0]], 
                                    mesh->getNodes()[node_indices[1]], 
                                    mesh->getNodes()[node_indices[2]], 
                                    mesh->getNodes()[node_indices[3]]
                                }));
                            }
                        }
                    }
                    
                    // 减少剩余需要读取的单元数
                    num_elements--;
                    if (num_elements <= 0) {
                        vertices_per_element = 0;
                    }
                }
            }
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
                    mesh->addNode(new Node(points_read, {x, y, z}));
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
                        mesh->addElement(new TriElement(i, {
                                                            mesh->getNodes()[connectivity[conn_index]],
                                                            mesh->getNodes()[connectivity[conn_index + 1]],
                                                            mesh->getNodes()[connectivity[conn_index + 2]]
                                                        }));
                        conn_index += 3;
                    }
                    break;
                case 10: // tetrahedron
                    if (offset == 4) {
                        mesh->addElement(new TetraElement(i, {
                                                              mesh->getNodes()[connectivity[conn_index]],
                                                              mesh->getNodes()[connectivity[conn_index + 1]],
                                                              mesh->getNodes()[connectivity[conn_index + 2]],
                                                              mesh->getNodes()[connectivity[conn_index + 3]]
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
                    mesh->addNode(new Node(points_read, {x, y, z}));
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
                        mesh->addElement(new TriElement(i, {
                                                            mesh->getNodes()[connectivity[conn_index]],
                                                            mesh->getNodes()[connectivity[conn_index + 1]],
                                                            mesh->getNodes()[connectivity[conn_index + 2]]
                                                        }));
                        conn_index += 3;
                    }
                    break;
                case 10: // tetrahedron
                    if (offset == 4) {
                        mesh->addElement(new TetraElement(i, {
                                                              mesh->getNodes()[connectivity[conn_index]],
                                                              mesh->getNodes()[connectivity[conn_index + 1]],
                                                              mesh->getNodes()[connectivity[conn_index + 2]],
                                                              mesh->getNodes()[connectivity[conn_index + 3]]
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