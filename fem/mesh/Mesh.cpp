#include "Mesh.hpp"
#include "Geometry.hpp"
#include "Boundary.hpp"
#include "Element.hpp"
#include <stdexcept>
#include <algorithm>
#include <set>

namespace FEM {

    // 析构函数现在为空，因为智能指针会自动管理内存
    // Mesh::~Mesh() {
    //     for (auto p : nodes_) delete p;
    //     for (auto p : elements_) delete p;
    // }

    void Mesh::addNode(std::unique_ptr<Node> node) {
        node_map_[node->getId()] = node.get(); // 填充 map
        nodes_.push_back(std::move(node));
    }

    void Mesh::addElement(std::unique_ptr<Element> element) {
        elements_.push_back(std::move(element));
    }

    Node* Mesh::getNodeById(int id) const {
        auto it = node_map_.find(id);
        if (it != node_map_.end()) {
            return it->second;
        }
        return nullptr;
    }

    void Mesh::buildTopology() {
        // 清空现有的边和面
        edges_.clear();
        faces_.clear();
        
        // 用于确保边和面唯一性的集合
        std::set<std::pair<int, int>> edge_set;
        std::set<std::set<int>> face_set;
        
        int edge_id_counter = 0;
        int face_id_counter = 0;
        
        // 遍历所有单元以识别边和面
        for (const auto& elem : elements_) {
            const auto& nodes = elem->getNodes();
            
            // 根据单元类型确定边和面
            switch (elem->getType()) {
                case ElementType::Line:
                    // 线单元本身就是一条边
                    if (nodes.size() >= 2) {
                        std::pair<int, int> edge(nodes[0]->getId(), nodes[1]->getId());
                        if (edge.first > edge.second) std::swap(edge.first, edge.second);
                        if (edge_set.find(edge) == edge_set.end()) {
                            edge_set.insert(edge);
                            edges_.push_back(std::make_unique<Edge>(edge_id_counter++, edge.first, edge.second));
                        }
                    }
                    break;
                    
                case ElementType::Triangle:
                    // 三角形单元有3条边
                    for (size_t i = 0; i < 3; ++i) {
                        size_t next = (i + 1) % 3;
                        std::pair<int, int> edge(nodes[i]->getId(), nodes[next]->getId());
                        if (edge.first > edge.second) std::swap(edge.first, edge.second);
                        if (edge_set.find(edge) == edge_set.end()) {
                            edge_set.insert(edge);
                            edges_.push_back(std::make_unique<Edge>(edge_id_counter++, edge.first, edge.second));
                        }
                    }
                    // 三角形单元本身是一个面
                    {
                        std::set<int> face_nodes;
                        for (const auto& node : nodes) {
                            face_nodes.insert(node->getId());
                        }
                        if (face_set.find(face_nodes) == face_set.end()) {
                            face_set.insert(face_nodes);
                            std::vector<int> node_ids(face_nodes.begin(), face_nodes.end());
                            faces_.push_back(std::make_unique<Face>(face_id_counter++, node_ids));
                        }
                    }
                    break;
                    
                case ElementType::Quadrilateral:
                    // 四边形单元有4条边
                    for (size_t i = 0; i < 4; ++i) {
                        size_t next = (i + 1) % 4;
                        std::pair<int, int> edge(nodes[i]->getId(), nodes[next]->getId());
                        if (edge.first > edge.second) std::swap(edge.first, edge.second);
                        if (edge_set.find(edge) == edge_set.end()) {
                            edge_set.insert(edge);
                            edges_.push_back(std::make_unique<Edge>(edge_id_counter++, edge.first, edge.second));
                        }
                    }
                    // 四边形单元本身是一个面
                    {
                        std::set<int> face_nodes;
                        for (const auto& node : nodes) {
                            face_nodes.insert(node->getId());
                        }
                        if (face_set.find(face_nodes) == face_set.end()) {
                            face_set.insert(face_nodes);
                            std::vector<int> node_ids(face_nodes.begin(), face_nodes.end());
                            faces_.push_back(std::make_unique<Face>(face_id_counter++, node_ids));
                        }
                    }
                    break;
                    
                case ElementType::Tetrahedron:
                    // 四面体单元有6条边
                    for (size_t i = 0; i < 4; ++i) {
                        for (size_t j = i + 1; j < 4; ++j) {
                            std::pair<int, int> edge(nodes[i]->getId(), nodes[j]->getId());
                            if (edge.first > edge.second) std::swap(edge.first, edge.second);
                            if (edge_set.find(edge) == edge_set.end()) {
                                edge_set.insert(edge);
                                edges_.push_back(std::make_unique<Edge>(edge_id_counter++, edge.first, edge.second));
                            }
                        }
                    }
                    // 四面体单元有4个面
                    // 面0: 节点 0,1,2
                    {
                        std::set<int> face_nodes = {nodes[0]->getId(), nodes[1]->getId(), nodes[2]->getId()};
                        if (face_set.find(face_nodes) == face_set.end()) {
                            face_set.insert(face_nodes);
                            std::vector<int> node_ids(face_nodes.begin(), face_nodes.end());
                            faces_.push_back(std::make_unique<Face>(face_id_counter++, node_ids));
                        }
                    }
                    // 面1: 节点 0,1,3
                    {
                        std::set<int> face_nodes = {nodes[0]->getId(), nodes[1]->getId(), nodes[3]->getId()};
                        if (face_set.find(face_nodes) == face_set.end()) {
                            face_set.insert(face_nodes);
                            std::vector<int> node_ids(face_nodes.begin(), face_nodes.end());
                            faces_.push_back(std::make_unique<Face>(face_id_counter++, node_ids));
                        }
                    }
                    // 面2: 节点 0,2,3
                    {
                        std::set<int> face_nodes = {nodes[0]->getId(), nodes[2]->getId(), nodes[3]->getId()};
                        if (face_set.find(face_nodes) == face_set.end()) {
                            face_set.insert(face_nodes);
                            std::vector<int> node_ids(face_nodes.begin(), face_nodes.end());
                            faces_.push_back(std::make_unique<Face>(face_id_counter++, node_ids));
                        }
                    }
                    // 面3: 节点 1,2,3
                    {
                        std::set<int> face_nodes = {nodes[1]->getId(), nodes[2]->getId(), nodes[3]->getId()};
                        if (face_set.find(face_nodes) == face_set.end()) {
                            face_set.insert(face_nodes);
                            std::vector<int> node_ids(face_nodes.begin(), face_nodes.end());
                            faces_.push_back(std::make_unique<Face>(face_id_counter++, node_ids));
                        }
                    }
                    break;
                    
                case ElementType::Hexahedron:
                    // 六面体单元有12条边
                    // 底面4条边
                    for (size_t i = 0; i < 4; ++i) {
                        size_t next = (i + 1) % 4;
                        std::pair<int, int> edge(nodes[i]->getId(), nodes[next]->getId());
                        if (edge.first > edge.second) std::swap(edge.first, edge.second);
                        if (edge_set.find(edge) == edge_set.end()) {
                            edge_set.insert(edge);
                            edges_.push_back(std::make_unique<Edge>(edge_id_counter++, edge.first, edge.second));
                        }
                    }
                    // 顶面4条边
                    for (size_t i = 4; i < 8; ++i) {
                        size_t next = (i + 1) % 4 + 4;
                        std::pair<int, int> edge(nodes[i]->getId(), nodes[next]->getId());
                        if (edge.first > edge.second) std::swap(edge.first, edge.second);
                        if (edge_set.find(edge) == edge_set.end()) {
                            edge_set.insert(edge);
                            edges_.push_back(std::make_unique<Edge>(edge_id_counter++, edge.first, edge.second));
                        }
                    }
                    // 垂直边4条
                    for (size_t i = 0; i < 4; ++i) {
                        std::pair<int, int> edge(nodes[i]->getId(), nodes[i+4]->getId());
                        if (edge.first > edge.second) std::swap(edge.first, edge.second);
                        if (edge_set.find(edge) == edge_set.end()) {
                            edge_set.insert(edge);
                            edges_.push_back(std::make_unique<Edge>(edge_id_counter++, edge.first, edge.second));
                        }
                    }
                    // 六面体单元有6个面
                    // 底面: 节点 0,1,2,3
                    {
                        std::set<int> face_nodes = {nodes[0]->getId(), nodes[1]->getId(), nodes[2]->getId(), nodes[3]->getId()};
                        if (face_set.find(face_nodes) == face_set.end()) {
                            face_set.insert(face_nodes);
                            std::vector<int> node_ids(face_nodes.begin(), face_nodes.end());
                            faces_.push_back(std::make_unique<Face>(face_id_counter++, node_ids));
                        }
                    }
                    // 顶面: 节点 4,5,6,7
                    {
                        std::set<int> face_nodes = {nodes[4]->getId(), nodes[5]->getId(), nodes[6]->getId(), nodes[7]->getId()};
                        if (face_set.find(face_nodes) == face_set.end()) {
                            face_set.insert(face_nodes);
                            std::vector<int> node_ids(face_nodes.begin(), face_nodes.end());
                            faces_.push_back(std::make_unique<Face>(face_id_counter++, node_ids));
                        }
                    }
                    // 前面: 节点 0,1,5,4
                    {
                        std::set<int> face_nodes = {nodes[0]->getId(), nodes[1]->getId(), nodes[5]->getId(), nodes[4]->getId()};
                        if (face_set.find(face_nodes) == face_set.end()) {
                            face_set.insert(face_nodes);
                            std::vector<int> node_ids(face_nodes.begin(), face_nodes.end());
                            faces_.push_back(std::make_unique<Face>(face_id_counter++, node_ids));
                        }
                    }
                    // 后面: 节点 3,2,6,7
                    {
                        std::set<int> face_nodes = {nodes[3]->getId(), nodes[2]->getId(), nodes[6]->getId(), nodes[7]->getId()};
                        if (face_set.find(face_nodes) == face_set.end()) {
                            face_set.insert(face_nodes);
                            std::vector<int> node_ids(face_nodes.begin(), face_nodes.end());
                            faces_.push_back(std::make_unique<Face>(face_id_counter++, node_ids));
                        }
                    }
                    // 左面: 节点 0,3,7,4
                    {
                        std::set<int> face_nodes = {nodes[0]->getId(), nodes[3]->getId(), nodes[7]->getId(), nodes[4]->getId()};
                        if (face_set.find(face_nodes) == face_set.end()) {
                            face_set.insert(face_nodes);
                            std::vector<int> node_ids(face_nodes.begin(), face_nodes.end());
                            faces_.push_back(std::make_unique<Face>(face_id_counter++, node_ids));
                        }
                    }
                    // 右面: 节点 1,2,6,5
                    {
                        std::set<int> face_nodes = {nodes[1]->getId(), nodes[2]->getId(), nodes[6]->getId(), nodes[5]->getId()};
                        if (face_set.find(face_nodes) == face_set.end()) {
                            face_set.insert(face_nodes);
                            std::vector<int> node_ids(face_nodes.begin(), face_nodes.end());
                            faces_.push_back(std::make_unique<Face>(face_id_counter++, node_ids));
                        }
                    }
                    break;
                    
                default:
                    break;
            }
        }
    }

    std::unique_ptr<Geometry> Mesh::create_uniform_1d_mesh(double length, int num_elements) {
        auto mesh = std::make_unique<Mesh>();
        double h = length / num_elements;
        for (int i = 0; i <= num_elements; ++i) {
            mesh->addNode(std::make_unique<Node>(i, std::vector<double>{i * h}));
        }
        for (int i = 0; i < num_elements; ++i) {
            mesh->addElement(std::make_unique<LineElement>(i, std::vector<Node*>{mesh->getNodeById(i), mesh->getNodeById(i + 1)}));
        }
        
        auto geometry = std::make_unique<Geometry>(std::move(mesh));

        // 添加边界定义
        auto left_bnd = std::make_unique<Boundary>("left");
        auto left_node = geometry->getMesh().getNodeById(0);
        if (left_node) {
            left_bnd->addElement(std::make_unique<PointElement>(0, std::vector<Node*>{left_node}));
        }
        geometry->addBoundary(std::move(left_bnd));

        auto right_bnd = std::make_unique<Boundary>("right");
        auto right_node = geometry->getMesh().getNodeById(num_elements);
        if (right_node) {
            right_bnd->addElement(std::make_unique<PointElement>(1, std::vector<Node*>{right_node}));
        }
        geometry->addBoundary(std::move(right_bnd));
        
        geometry->getMesh().buildTopology();
        
        return geometry;
    }

    // --- 以下是缺失的函数实现 ---

    std::unique_ptr<Geometry> Mesh::create_uniform_2d_mesh(double width, double height, int nx, int ny) {
        auto mesh = std::make_unique<Mesh>();
        double dx = width / nx;
        double dy = height / ny;
        int node_id_counter = 0;
        for (int j = 0; j <= ny; ++j) {
            for (int i = 0; i <= nx; ++i) {
                mesh->addNode(std::make_unique<Node>(node_id_counter++, std::vector<double>{i * dx, j * dy}));
            }
        }
        int elem_id_counter = 0;
        for (int j = 0; j < ny; ++j) {
            for (int i = 0; i < nx; ++i) {
                int n0_id = j * (nx + 1) + i;
                int n1_id = j * (nx + 1) + i + 1;
                int n2_id = (j + 1) * (nx + 1) + i + 1;
                int n3_id = (j + 1) * (nx + 1) + i;
                mesh->addElement(std::make_unique<QuadElement>(elem_id_counter++, std::vector<Node*>{
                    mesh->getNodeById(n0_id), mesh->getNodeById(n1_id),
                    mesh->getNodeById(n2_id), mesh->getNodeById(n3_id)
                }));
            }
        }
        
        auto geometry = std::make_unique<Geometry>(std::move(mesh));
        
        // 添加边界定义
        // 底边 (y=0)
        auto bottom_bnd = std::make_unique<Boundary>("bottom");
        for (int i = 0; i <= nx; ++i) {
            auto node = geometry->getMesh().getNodeById(i);
            if (node) {
                bottom_bnd->addElement(std::make_unique<PointElement>(i, std::vector<Node*>{node}));
            }
        }
        geometry->addBoundary(std::move(bottom_bnd));
        
        // 顶边 (y=height)
        auto top_bnd = std::make_unique<Boundary>("top");
        for (int i = 0; i <= nx; ++i) {
            auto node = geometry->getMesh().getNodeById(ny * (nx + 1) + i);
            if (node) {
                top_bnd->addElement(std::make_unique<PointElement>(ny * (nx + 1) + i, std::vector<Node*>{node}));
            }
        }
        geometry->addBoundary(std::move(top_bnd));
        
        // 左边 (x=0)
        auto left_bnd = std::make_unique<Boundary>("left");
        for (int j = 0; j <= ny; ++j) {
            auto node = geometry->getMesh().getNodeById(j * (nx + 1));
            if (node) {
                left_bnd->addElement(std::make_unique<PointElement>(j * (nx + 1), std::vector<Node*>{node}));
            }
        }
        geometry->addBoundary(std::move(left_bnd));
        
        // 右边 (x=width)
        auto right_bnd = std::make_unique<Boundary>("right");
        for (int j = 0; j <= ny; ++j) {
            auto node = geometry->getMesh().getNodeById(j * (nx + 1) + nx);
            if (node) {
                right_bnd->addElement(std::make_unique<PointElement>(j * (nx + 1) + nx, std::vector<Node*>{node}));
            }
        }
        geometry->addBoundary(std::move(right_bnd));
        
        // 构建拓扑结构
        geometry->getMesh().buildTopology();
        
        return geometry;
    }

    std::unique_ptr<Geometry> Mesh::create_uniform_3d_mesh(double width, double height, double depth, int nx, int ny, int nz) {
        auto mesh = std::make_unique<Mesh>();
        double dx = width / nx;
        double dy = height / ny;
        double dz = depth / nz;
        int node_id_counter = 0;
        for(int k=0; k<=nz; ++k) {
            for(int j=0; j<=ny; ++j) {
                for(int i=0; i<=nx; ++i) {
                    mesh->addNode(std::make_unique<Node>(node_id_counter++, std::vector<double>{i * dx, j * dy, k * dz}));
                }
            }
        }
        int elem_id_counter = 0;
        for(int k=0; k<nz; ++k) {
            for(int j=0; j<ny; ++j) {
                for(int i=0; i<nx; ++i) {
                    int n_ids[8];
                    n_ids[0] = k * (nx + 1) * (ny + 1) + j * (nx + 1) + i;
                    n_ids[1] = k * (nx + 1) * (ny + 1) + j * (nx + 1) + i + 1;
                    n_ids[2] = k * (nx + 1) * (ny + 1) + (j + 1) * (nx + 1) + i + 1;
                    n_ids[3] = k * (nx + 1) * (ny + 1) + (j + 1) * (nx + 1) + i;
                    n_ids[4] = (k + 1) * (nx + 1) * (ny + 1) + j * (nx + 1) + i;
                    n_ids[5] = (k + 1) * (nx + 1) * (ny + 1) + j * (nx + 1) + i + 1;
                    n_ids[6] = (k + 1) * (nx + 1) * (ny + 1) + (j + 1) * (nx + 1) + i + 1;
                    n_ids[7] = (k + 1) * (nx + 1) * (ny + 1) + (j + 1) * (nx + 1) + i;

                    mesh->addElement(std::make_unique<HexaElement>(elem_id_counter++, std::vector<Node*>{
                        mesh->getNodeById(n_ids[0]), mesh->getNodeById(n_ids[1]),
                        mesh->getNodeById(n_ids[2]), mesh->getNodeById(n_ids[3]),
                        mesh->getNodeById(n_ids[4]), mesh->getNodeById(n_ids[5]),
                        mesh->getNodeById(n_ids[6]), mesh->getNodeById(n_ids[7])
                    }));
                }
            }
        }
        
        auto geometry = std::make_unique<Geometry>(std::move(mesh));
        
        // 添加边界定义
        // x方向的两个面
        auto left_bnd = std::make_unique<Boundary>("left"); // x=0面
        auto right_bnd = std::make_unique<Boundary>("right"); // x=width面
        for (int k = 0; k <= nz; ++k) {
            for (int j = 0; j <= ny; ++j) {
                auto left_node = geometry->getMesh().getNodeById(k * (nx + 1) * (ny + 1) + j * (nx + 1));
                if (left_node) {
                    left_bnd->addElement(std::make_unique<PointElement>(k * (nx + 1) * (ny + 1) + j * (nx + 1), std::vector<Node*>{left_node}));
                }
                
                auto right_node = geometry->getMesh().getNodeById(k * (nx + 1) * (ny + 1) + j * (nx + 1) + nx);
                if (right_node) {
                    right_bnd->addElement(std::make_unique<PointElement>(k * (nx + 1) * (ny + 1) + j * (nx + 1) + nx, std::vector<Node*>{right_node}));
                }
            }
        }
        geometry->addBoundary(std::move(left_bnd));
        geometry->addBoundary(std::move(right_bnd));
        
        // y方向的两个面
        auto front_bnd = std::make_unique<Boundary>("front"); // y=0面
        auto back_bnd = std::make_unique<Boundary>("back"); // y=height面
        for (int k = 0; k <= nz; ++k) {
            for (int i = 0; i <= nx; ++i) {
                auto front_node = geometry->getMesh().getNodeById(k * (nx + 1) * (ny + 1) + i);
                if (front_node) {
                    front_bnd->addElement(std::make_unique<PointElement>(k * (nx + 1) * (ny + 1) + i, std::vector<Node*>{front_node}));
                }
                
                auto back_node = geometry->getMesh().getNodeById(k * (nx + 1) * (ny + 1) + ny * (nx + 1) + i);
                if (back_node) {
                    back_bnd->addElement(std::make_unique<PointElement>(k * (nx + 1) * (ny + 1) + ny * (nx + 1) + i, std::vector<Node*>{back_node}));
                }
            }
        }
        geometry->addBoundary(std::move(front_bnd));
        geometry->addBoundary(std::move(back_bnd));
        
        // z方向的两个面
        auto bottom_bnd = std::make_unique<Boundary>("bottom"); // z=0面
        auto top_bnd = std::make_unique<Boundary>("top"); // z=depth面
        for (int j = 0; j <= ny; ++j) {
            for (int i = 0; i <= nx; ++i) {
                auto bottom_node = geometry->getMesh().getNodeById(j * (nx + 1) + i);
                if (bottom_node) {
                    bottom_bnd->addElement(std::make_unique<PointElement>(j * (nx + 1) + i, std::vector<Node*>{bottom_node}));
                }
                
                auto top_node = geometry->getMesh().getNodeById(nz * (nx + 1) * (ny + 1) + j * (nx + 1) + i);
                if (top_node) {
                    top_bnd->addElement(std::make_unique<PointElement>(nz * (nx + 1) * (ny + 1) + j * (nx + 1) + i, std::vector<Node*>{top_node}));
                }
            }
        }
        geometry->addBoundary(std::move(bottom_bnd));
        geometry->addBoundary(std::move(top_bnd));
        
        // 构建拓扑结构
        geometry->getMesh().buildTopology();
        
        return geometry;
    }

} // namespace FEM