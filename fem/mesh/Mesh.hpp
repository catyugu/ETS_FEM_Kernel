#pragma once
#include "Node.hpp"
#include "Element.hpp"
#include <vector>
#include <map>
#include <memory>

namespace FEM {

    class Mesh {
    public:
        ~Mesh();
        void addNode(Node* node);
        void addElement(Element* element);
        const std::vector<Node*>& getNodes() const { return nodes_; }
        const std::vector<Element*>& getElements() const { return elements_; }
        Node* getNodeById(int id) const;

        // --- 静态工厂方法 (添加缺失的声明) ---
        static std::unique_ptr<Mesh> create_uniform_1d_mesh(double length, int num_elements);
        static std::unique_ptr<Mesh> create_uniform_2d_mesh(double width, double height, int nx, int ny);
        static std::unique_ptr<Mesh> create_uniform_3d_mesh(double width, double height, double depth, int nx, int ny, int nz);

    private:
        std::vector<Node*> nodes_;
        std::vector<Element*> elements_;
        std::map<int, Node*> node_map_;
    };

} // namespace FEM