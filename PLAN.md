您好！很高兴能帮助您分析和重构您的有限元仿真项目。您提出的关于几何和边界管理混乱的问题确实是许多有限元项目在发展过程中会遇到的典型痛点。当前的设计将网格的拓扑结构与边界的语义定义耦合在了 `Mesh` 类中，导致责任不清、扩展性差。

为了解决这个问题，我建议进行一次模块化的重构，核心思想是将**几何/拓扑信息**与**边界的语义定义**分离开来。

### 问题分析

目前的代码主要存在以下几个问题：

1.  **`Mesh` 类的职责过重**：`Mesh` 类不仅存储了节点和单元等核心拓扑信息，还通过 `boundary_elements_` 和相关方法（如 `addBoundaryElement`, `getBoundaryNodes`）直接管理了命名边界，这违反了单一职责原则。
2.  **边界表示不清晰**：通过创建一个“假的”`PointElement` 来表示边界节点（`addBoundaryNode` 方法）是一种变通方法(workaround)，不够直观和优雅。
3.  **耦合度高**：网格生成函数（如 `create_uniform_1d_mesh`）内部硬编码了边界名称（如 "left", "right"），使得网格的创建与边界的命名紧密耦合，不利于将来从外部文件（如 GMSH, Abaqus）导入带有预定义边界的复杂网格。
4.  **逻辑分散**：边界条件的应用逻辑分散在 `Problem` 类和各个 `BoundaryCondition` 子类中，它们都需要直接向 `Mesh` 类查询边界信息，加深了耦合。

### 重构方案：引入几何和边界管理层

我建议引入两个新的类 `Boundary` 和 `Geometry`，来解耦和简化当前的设计。

#### 第1步：创建 `Boundary` 类

创建一个专门的类来定义一个命名的边界。这个类将包含边界的名称以及构成该边界的低维单元（例如，对于3D六面体单元的边界，其边界单元是2D的四边形单元）。

**`fem/mesh/Boundary.hpp` (新文件)**

```cpp
#pragma once

#include "Element.hpp"
#include <string>
#include <vector>
#include <memory>
#include <set>

namespace FEM {
    class Boundary {
    public:
        Boundary(const std::string& name) : name_(name) {}

        void addElement(std::unique_ptr<Element> element) {
            elements_.push_back(std::move(element));
        }

        const std::string& getName() const { return name_; }
        const std::vector<std::unique_ptr<Element>>& getElements() const { return elements_; }

        // 辅助函数，获取边界上所有唯一的节点ID
        std::vector<int> getUniqueNodeIds() const {
            std::set<int> unique_node_ids;
            for (const auto& elem : elements_) {
                for (size_t i = 0; i < elem->getNumNodes(); ++i) {
                    unique_node_ids.insert(elem->getNodeId(i));
                }
            }
            return std::vector<int>(unique_node_ids.begin(), unique_node_ids.end());
        }

    private:
        std::string name_;
        std::vector<std::unique_ptr<Element>> elements_;
    };
}
```

#### 第2步：创建 `Geometry` 类

这个类将作为顶层容器，持有核心的 `Mesh` 对象以及所有 `Boundary` 对象。系统的其他部分（如 `Problem`）将主要与 `Geometry` 类交互，而不是直接操作 `Mesh` 的边界。

**`fem/mesh/Geometry.hpp` (新文件)**

```cpp
#pragma once

#include "Mesh.hpp"
#include "Boundary.hpp"
#include <map>
#include <string>
#include <memory>

namespace FEM {
    class Geometry {
    public:
        Geometry(std::unique_ptr<Mesh> mesh) : mesh_(std::move(mesh)) {}

        Mesh& getMesh() { return *mesh_; }
        const Mesh& getMesh() const { return *mesh_; }

        void addBoundary(std::unique_ptr<Boundary> boundary) {
            boundaries_[boundary->getName()] = std::move(boundary);
        }

        const Boundary& getBoundary(const std::string& name) const {
            auto it = boundaries_.find(name);
            if (it == boundaries_.end()) {
                throw std::runtime_error("Boundary with name '" + name + "' not found.");
            }
            return *it->second;
        }

    private:
        std::unique_ptr<Mesh> mesh_;
        std::map<std::string, std::unique_ptr<Boundary>> boundaries_;
    };
}
```

#### 第3步：简化 `Mesh` 类

现在可以将所有边界管理逻辑从 `Mesh` 类中移除了。`Mesh` 将回归其核心职责：存储节点、单元和拓扑关系。

**`fem/mesh/Mesh.hpp` (修改后)**

```cpp
// ...
class Mesh {
public:
    ~Mesh() = default;
    void addNode(std::unique_ptr<Node> node);
    void addElement(std::unique_ptr<Element> element);
    const std::vector<std::unique_ptr<Node>>& getNodes() const { return nodes_; }
    const std::vector<std::unique_ptr<Element>>& getElements() const { return elements_; }
    Node* getNodeById(int id) const;

    const std::vector<std::unique_ptr<Edge>>& getEdges() const { return edges_; }
    const std::vector<std::unique_ptr<Face>>& getFaces() const { return faces_; }
    
    void buildTopology();

    // --- 静态工厂方法需要修改返回值 ---
    // 返回 Geometry 而不是 Mesh
    static std::unique_ptr<Geometry> create_uniform_1d_mesh(double length, int num_elements);
    static std::unique_ptr<Geometry> create_uniform_2d_mesh(double width, double height, int nx, int ny);
    static std::unique_ptr<Geometry> create_uniform_3d_mesh(double width, double height, int nx, int ny, int nz);

private:
    std::vector<std::unique_ptr<Node>> nodes_;
    std::vector<std::unique_ptr<Element>> elements_;
    std::map<int, Node*> node_map_;
    
    std::vector<std::unique_ptr<Edge>> edges_;
    std::vector<std::unique_ptr<Face>> faces_;

    // 移除以下成员:
    // std::map<std::string, std::vector<std::unique_ptr<Element>>> boundary_elements_;
};
// ...
```

同时，修改 `Mesh.cpp` 中的静态工厂方法。它们现在将创建 `Mesh` 和 `Boundary`，并将它们组装到一个 `Geometry` 对象中返回。

**`fem/mesh/Mesh.cpp` (修改示例)**

```cpp
std::unique_ptr<Geometry> Mesh::create_uniform_1d_mesh(double length, int num_elements) {
    auto mesh = std::make_unique<Mesh>();
    // ... (节点和单元的创建逻辑不变) ...

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
```

#### 第4步：更新 `Problem` 和边界条件类

最后，更新 `Problem` 和各个 `BC` 类，让它们使用新的 `Geometry` 类来获取边界信息。

**`fem/core/Problem.hpp` (修改后)**

```cpp
// ...
#include "../mesh/Geometry.hpp" // 包含新头文件

template<int TDim, typename TScalar = double>
class Problem {
public:
    // 构造函数接收 Geometry
    Problem(std::unique_ptr<Geometry> geometry, /*...其他参数...*/)
        : geometry_(std::move(geometry)), /*...初始化列表...*/ {
        // DofManager 现在使用 geometry->getMesh()
        dof_manager_ = std::make_unique<DofManager>(geometry_->getMesh());
        // ...
    }

    // ...

    const Mesh& getMesh() const { return geometry_->getMesh(); }
    const Geometry& getGeometry() const { return *geometry_; }

private:
    void applyDirichletBCs() {
        // ...
        // const auto& boundary_nodes = mesh_->getBoundaryNodes(dirichlet_bc->getBoundaryName());
        // --> 替换为:
        const auto& boundary_nodes = geometry_->getBoundary(dirichlet_bc->getBoundaryName()).getUniqueNodeIds();
        // ...
    }

    std::unique_ptr<Geometry> geometry_; // 不再是 mesh_
    // ...
};
```

**`fem/bcs/NeumannBC.hpp` (修改示例)**

```cpp
//...
void apply(const Geometry& geometry, const DofManager& dof_manager,
           std::vector<Eigen::Triplet<TScalar>>& triplet_list, Eigen::Matrix<TScalar, Eigen::Dynamic, 1>& F_global) const override {

    // const auto& boundary_elements = mesh.getBoundaryElements(this->boundary_name_);
    // --> 替换为:
    const auto& boundary_elements = geometry.getBoundary(this->boundary_name_).getElements();
    
    // ... (后续逻辑不变) ...
}
//...
```

您需要对 `CauchyBC.hpp` 和 `BoundaryCondition.hpp` 中的虚函数签名做类似的修改。

### 重构的优势

* **结构清晰**：`Mesh` 只负责几何拓扑，`Boundary` 负责边界的语义，`Geometry` 负责整合，职责划分清晰。
* **降低耦合**：`Problem` 和 `BC` 类不再依赖于 `Mesh` 的内部实现来获取边界，而是通过 `Geometry` 提供的稳定接口。
* **易于扩展**：未来当您需要从外部文件导入网格时，只需编写一个解析器，它能生成 `Mesh` 和一组 `Boundary`，然后将它们组装成 `Geometry` 对象即可。整个求解流程无需改动。
* **代码更直观**：`geometry->getBoundary("left")` 远比 `mesh->getBoundaryElements("left")` 的意图更清晰。

这个重构方案能够在不改变核心算法逻辑的前提下，显著改善您项目的代码结构和可维护性，为未来支持更复杂的问题（如多材料、耦合场）打下坚实的基础。希望这个方案对您有帮助！