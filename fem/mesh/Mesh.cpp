#include "Mesh.hpp"
#include "Element.hpp"
#include <stdexcept>

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
        
        return mesh;
    }

} // namespace FEM