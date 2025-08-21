#include "Mesh.hpp"
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

    std::unique_ptr<Mesh> Mesh::create_uniform_1d_mesh(double length, int num_elements) {
        auto mesh = std::make_unique<Mesh>();
        double h = length / num_elements;
        for (int i = 0; i <= num_elements; ++i) {
            mesh->addNode(std::make_unique<Node>(i, std::vector<double>{i * h}));
        }
        for (int i = 0; i < num_elements; ++i) {
            mesh->addElement(std::make_unique<LineElement>(i, std::vector<Node*>{mesh->getNodeById(i), mesh->getNodeById(i + 1)}));
        }
        
        // 添加边界信息
        mesh->addBoundaryNode("left", 0);
        mesh->addBoundaryNode("right", num_elements);
        
        // 构建拓扑结构
        mesh->buildTopology();
        
        return mesh;
    }

    // --- 以下是缺失的函数实现 ---

    std::unique_ptr<Mesh> Mesh::create_uniform_2d_mesh(double width, double height, int nx, int ny) {
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
        
        // 添加边界信息 - 完善边界命名
        // 底边 (y=0)
        for (int i = 0; i <= nx; ++i) {
            mesh->addBoundaryNode("bottom", i);
        }
        // 顶边 (y=height)
        for (int i = 0; i <= nx; ++i) {
            mesh->addBoundaryNode("top", ny * (nx + 1) + i);
        }
        // 左边 (x=0)
        for (int j = 0; j <= ny; ++j) {
            mesh->addBoundaryNode("left", j * (nx + 1));
        }
        // 右边 (x=width)
        for (int j = 0; j <= ny; ++j) {
            mesh->addBoundaryNode("right", j * (nx + 1) + nx);
        }
        
        // 构建拓扑结构
        mesh->buildTopology();
        
        return mesh;
    }

    std::unique_ptr<Mesh> Mesh::create_uniform_3d_mesh(double width, double height, double depth, int nx, int ny, int nz) {
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
        
        // 添加边界信息 - 完善边界命名
        // x方向的两个面
        for (int k = 0; k <= nz; ++k) {
            for (int j = 0; j <= ny; ++j) {
                mesh->addBoundaryNode("left", k * (nx + 1) * (ny + 1) + j * (nx + 1)); // x=0面
                mesh->addBoundaryNode("right", k * (nx + 1) * (ny + 1) + j * (nx + 1) + nx); // x=width面
            }
        }
        // y方向的两个面
        for (int k = 0; k <= nz; ++k) {
            for (int i = 0; i <= nx; ++i) {
                mesh->addBoundaryNode("front", k * (nx + 1) * (ny + 1) + i); // y=0面
                mesh->addBoundaryNode("back", k * (nx + 1) * (ny + 1) + ny * (nx + 1) + i); // y=height面
            }
        }
        // z方向的两个面
        for (int j = 0; j <= ny; ++j) {
            for (int i = 0; i <= nx; ++i) {
                mesh->addBoundaryNode("bottom", j * (nx + 1) + i); // z=0面
                mesh->addBoundaryNode("top", nz * (nx + 1) * (ny + 1) + j * (nx + 1) + i); // z=depth面
            }
        }
        
        // 构建拓扑结构
        mesh->buildTopology();
        
        return mesh;
    }

} // namespace FEM