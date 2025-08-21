#pragma once

#include <algorithm>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include "Node.hpp"
#include "Element.hpp"

namespace FEM {

    // 前向声明
    class Element;

    // 定义边结构
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

        // --- 静态工厂方法 (添加缺失的声明) ---
        static std::unique_ptr<Mesh> create_uniform_1d_mesh(double length, int num_elements);
        static std::unique_ptr<Mesh> create_uniform_2d_mesh(double width, double height, int nx, int ny);
        static std::unique_ptr<Mesh> create_uniform_3d_mesh(double width, double height, double depth, int nx, int ny, int nz);

        // 添加边界元素支持
        void addBoundaryElement(const std::string& boundary_name, std::unique_ptr<Element> element) {
            boundary_elements_[boundary_name].push_back(std::move(element));
        }

        // 获取指定名称的边界单元集合
        const std::vector<std::unique_ptr<Element>>& getBoundaryElements(const std::string& boundary_name) const {
            auto it = boundary_elements_.find(boundary_name);
            if (it == boundary_elements_.end()) {
                throw std::runtime_error("Boundary with name '" + boundary_name + "' not found.");
            }
            return it->second;
        }

        // 获取指定名称边界上的所有唯一节点ID
        std::vector<int> getBoundaryNodes(const std::string& boundary_name) const {
            std::vector<int> node_ids;
            const auto& b_elements = getBoundaryElements(boundary_name);
            
            std::set<int> unique_node_ids;
            for (const auto& elem : b_elements) {
                for (size_t i = 0; i < elem->getNumNodes(); ++i) {
                    unique_node_ids.insert(elem->getNodeId(i));
                }
            }
            
            node_ids.assign(unique_node_ids.begin(), unique_node_ids.end());
            return node_ids;
        }

        // 为程序生成的网格添加边界节点的辅助方法
        void addBoundaryNode(const std::string& boundary_name, int node_id) {
            // 创建一个假的点单元来表示边界节点
            auto node = getNodeById(node_id);
            if (node) {
                auto point_element = std::make_unique<PointElement>(node_id, std::vector<Node*>{node});
                addBoundaryElement(boundary_name, std::move(point_element));
            }
        }

    private:
        std::vector<std::unique_ptr<Node>> nodes_;
        std::vector<std::unique_ptr<Element>> elements_;
        std::map<int, Node*> node_map_;
        
        // 新增：边和面的存储
        std::vector<std::unique_ptr<Edge>> edges_;
        std::vector<std::unique_ptr<Face>> faces_;
        
        // key 是边界名称, value 是构成该边界的单元列表
        std::map<std::string, std::vector<std::unique_ptr<Element>>> boundary_elements_;
    };

} // namespace FEM