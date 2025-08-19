#include "Importer.hpp"
#include "../mesh/Element.hpp" // 需要包含具体的单元类型
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

namespace FEM::IO {

std::unique_ptr<Mesh> Importer::read_comsol_mphtxt(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open mesh file: " + filename);
    }

    auto mesh = std::make_unique<Mesh>();
    std::string line;
    int num_nodes = 0;
    int num_elements = 0;
    std::string section;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '%') continue;
        if (line[0] == '#') {
            std::stringstream ss(line);
            std::string token;
            ss >> token; // consume '#'
            section = token;
            continue;
        }

        if (section == "Mesh" && num_nodes == 0) { // Simple way to find node count line
            std::stringstream ss(line);
            ss >> num_nodes;
        } else if (section == "Mesh" && num_nodes > 0 && num_elements == 0) { // element count line
             std::stringstream ss(line);
             // This line contains multiple numbers, we are interested in the first one
             ss >> num_elements;
        } else if (section == "Vertex" && line.find("coordinates") != std::string::npos) {
            // This is the header for vertex coordinates, skip to the data
            for (int i = 0; i < num_nodes; ++i) {
                if (!std::getline(file, line)) break;
                std::stringstream ss(line);
                double x, y, z=0.0;
                ss >> x >> y;
                if (ss >> z) {
                    mesh->addNode(new Node(i, {x, y, z}));
                } else {
                    mesh->addNode(new Node(i, {x, y}));
                }
            }
        } else if (section == "Elements" && line.find("indices") != std::string::npos) {
            // This is the header for element indices, skip to the data
            for (int i = 0; i < num_elements; ++i) {
                if (!std::getline(file, line)) break;
                std::stringstream ss(line);
                int n1, n2, n3;
                ss >> n1 >> n2 >> n3; // 0-indexed
                mesh->addElement(new TriElement(i, {mesh->getNodes()[n1], mesh->getNodes()[n2], mesh->getNodes()[n3]}));
            }
        }
    }

    return mesh;
}

} // namespace FEM::IO