### **重构目标：实现基于RAII原则的、安全自动的网格内存管理**

1.  **安全性**：消除手动 `new` 和 `delete` 带来的内存泄漏风险。
2.  **现代化**：使用 `std::unique_ptr` 来明确对象的所有权，使 `Mesh` 类成为其节点和单元的唯一所有者。
3.  **代码清晰**：简化 `Mesh` 类的析构函数，让代码意图更清晰。
4.  **异常安全**：确保即使在构造或修改网格的过程中发生异常，已分配的内存也能被正确释放。

-----

### **重构步骤详解**

#### **第1步：审查并确认基类的虚析构函数**

在处理多态基类时，必须确保基类有虚析构函数。

* **文件**: `fem/mesh/Element.hpp`
* **操作**: 检查 `Element` 类。它已经有 `virtual ~Element() = default;`，这是正确的，无需修改。
* **文件**: `fem/mesh/Node.hpp`
* **操作**: `Node` 不是基类，但添加默认的析构函数是良好实践。

#### **第2步：修改 `Mesh` 类的数据成员和接口 (`fem/mesh/Mesh.hpp`)**

这是本次重构的核心。我们将把存储裸指针的容器换成存储智能指针的容器。

```cpp
// file: fem/mesh/Mesh.hpp
#pragma once
#include "Node.hpp"
#include "Element.hpp"
#include <vector>
#include <map>
#include <memory> // 包含 <memory> 头文件
#include <set>
#include <stdexcept>
#include <string>

namespace FEM {

    class Mesh {
    public:
        // ~Mesh() 现在可以设为 default，因为智能指针会自动管理内存
        ~Mesh() = default; 

        // 修改点 1: addNode/addElement 接受 std::unique_ptr
        void addNode(std::unique_ptr<Node> node);
        void addElement(std::unique_ptr<Element> element);

        // 修改点 2: getNodes/getElements 返回对容器的常量引用
        const std::vector<std::unique_ptr<Node>>& getNodes() const { return nodes_; }
        const std::vector<std::unique_ptr<Element>>& getElements() const { return elements_; }
        
        // getNodeById 保持不变，返回一个裸指针，表示非所有权的访问
        Node* getNodeById(int id) const;

        // ... 静态工厂方法声明保持不变 ...
        static std::unique_ptr<Mesh> create_uniform_1d_mesh(double length, int num_elements);
        // ... 其他工厂方法 ...

        // ... 边界相关方法保持不变 ...
        void addBoundaryElement(const std::string& boundary_name, std::unique_ptr<Element> element);
        const std::vector<std::unique_ptr<Element>>& getBoundaryElements(const std::string& boundary_name) const;
        // ...

    private:
        // 修改点 3: 容器类型改变
        std::vector<std::unique_ptr<Node>> nodes_;
        std::vector<std::unique_ptr<Element>> elements_;
        
        // node_map_ 仍然存储裸指针，作为非所有权的观察者/缓存
        std::map<int, Node*> node_map_;
        
        std::map<std::string, std::vector<std::unique_ptr<Element>>> boundary_elements_;
    };

} // namespace FEM
```

#### **第3步：更新 `Mesh` 类的实现 (`fem/mesh/Mesh.cpp`)**

现在我们需要更新 `.cpp` 文件以匹配头文件中的新接口。

```cpp
// file: fem/mesh/Mesh.cpp
#include "Mesh.hpp"
// ...

namespace FEM {

    // 析构函数现在为空，或直接在 .hpp 中 default
    // Mesh::~Mesh() { ... } // <--- 移除整个手动 delete 的析构函数

    void Mesh::addNode(std::unique_ptr<Node> node) {
        // 使用 get() 方法获取裸指针以放入观察者 map 中
        node_map_[node->getId()] = node.get(); 
        // 使用 std::move 将所有权转移给 vector
        nodes_.push_back(std::move(node));
    }

    void Mesh::addElement(std::unique_ptr<Element> element) {
        // 使用 std::move 将所有权转移给 vector
        elements_.push_back(std::move(element));
    }

    Node* Mesh::getNodeById(int id) const {
        auto it = node_map_.find(id);
        if (it != node_map_.end()) {
            return it->second;
        }
        return nullptr;
    }

    // --- 修改静态工厂方法 ---
    std::unique_ptr<Mesh> Mesh::create_uniform_1d_mesh(double length, int num_elements) {
        auto mesh = std::make_unique<Mesh>();
        // ...
        for (int i = 0; i <= num_elements; ++i) {
            // 使用 std::make_unique 创建节点
            auto node = std::make_unique<Node>(i, i * dx, 0, 0);
            mesh->addNode(std::move(node)); // 转移所有权
        }
        
        for (int i = 0; i < num_elements; ++i) {
            Node* n1 = mesh->getNodeById(i);
            Node* n2 = mesh->getNodeById(i + 1);
            // 使用 std::make_unique 创建单元
            auto element = std::make_unique<LineElement>(i, std::vector<Node*>{n1, n2});
            mesh->addElement(std::move(element)); // 转移所有权
        }
        
        // ... 边界命名逻辑不变 ...
        return mesh;
    }

    // 对 create_uniform_2d_mesh 和 create_uniform_3d_mesh 进行类似的修改...
    
} // namespace FEM
```

**关键点**：`std::unique_ptr` 的所有权必须通过 `std::move` 来转移。一旦转移，原来的指针就变为空。`node_map_` 作为非所有权的缓存，存储从 `unique_ptr` 获取的裸指针是安全且高效的。

#### **第4步：创建新的测试文件 (`tests/test_mesh_refactor.cpp`)**

我们不修改 `main.cpp`，而是创建一个新的单元测试来验证重构后的 `Mesh` 类。

### **重构后的优势**

完成这次重构后，您的 `Mesh` 模块将：

1.  **完全消除了内存泄漏的风险**，因为 `std::unique_ptr` 会在 `Mesh` 对象生命周期结束时自动、安全地释放其拥有的所有对象。
2.  **代码更简洁、意图更明确**，所有权关系通过类型系统（`std::unique_ptr`）清晰地表达了出来。
3.  **为未来的功能打下更坚实的基础**。一个内存安全的网格系统是开发任何复杂求解器功能的先决条件。