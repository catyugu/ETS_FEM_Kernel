#pragma once

#include <algorithm>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include "Node.hpp"
#include "Element.hpp"
#include "Boundary.hpp"  // 添加Boundary头文件包含

namespace FEM {
    class Geometry;

    // Forward declarations
    class Element;

    // Define edge structure
    struct Edge {
        int id;
        std::pair<int, int> node_ids;
        
        Edge(int edge_id, int node1_id, int node2_id) : id(edge_id) {
            // 保证边的方向一致性（较小ID在前）
            if (node1_id < node2_id) {
                node_ids = std::make_pair(node1_id, node2_id);
            } else {
                node_ids = std::make_pair(node2_id, node1_id);
            }
        }
    };

    // 定义面结构
    struct Face {
        int id;
        std::vector<int> node_ids;
        
        Face(int face_id, const std::vector<int>& nodes) : id(face_id), node_ids(nodes) {
            // 对节点ID进行排序以确保唯一性
            std::sort(node_ids.begin(), node_ids.end());
        }
    };

    class Mesh {
    public:
        ~Mesh() = default; // 使用默认析构函数，智能指针会自动管理内存
        void addNode(std::unique_ptr<Node> node);
        void addElement(std::unique_ptr<Element> element);
        const std::vector<std::unique_ptr<Node>>& getNodes() const { return nodes_; }
        const std::vector<std::unique_ptr<Element>>& getElements() const { return elements_; }
        Node* getNodeById(int id) const;

        // 新增：获取边和面的方法
        const std::vector<std::unique_ptr<Edge>>& getEdges() const { return edges_; }
        const std::vector<std::unique_ptr<Face>>& getFaces() const { return faces_; }
        
        // 新增：构建拓扑结构的方法
        void buildTopology();

        // --- Static factory methods (modified return type) ---
        static std::unique_ptr<Geometry> create_uniform_1d_mesh(double length, int num_elements);
        static std::unique_ptr<Geometry> create_uniform_2d_mesh(double width, double height, int nx, int ny);
        static std::unique_ptr<Geometry> create_uniform_3d_mesh(double width, double height, double depth, int nx, int ny, int nz);
        
    private:
        std::vector<std::unique_ptr<Node>> nodes_;
        std::vector<std::unique_ptr<Element>> elements_;
        std::map<int, Node*> node_map_;
        
        // 新增：边和面的存储
        std::vector<std::unique_ptr<Edge>> edges_;
        std::vector<std::unique_ptr<Face>> faces_;

    };

} // namespace FEM