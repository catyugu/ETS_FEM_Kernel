#include "DofManager.hpp"
#include <stdexcept>

namespace FEM {

    DofManager::DofManager(const Mesh& mesh) : mesh_(mesh) {}

    void DofManager::buildDofMap(int dofs_per_node) {
        if (dofs_per_node <= 0) {
            throw std::invalid_argument("Degrees of freedom per node must be positive.");
        }
        
        dofs_per_node_ = dofs_per_node;
        node_dof_map_.clear();
        int dof_counter = 0;

        // 遍历网格中的所有节点，为它们分配自由度索引
        for (const auto& node : mesh_.getNodes()) {
            node_dof_map_[node->getId()] = dof_counter;
            dof_counter += dofs_per_node_;
        }
        total_dofs_ = dof_counter;
    }

    std::vector<int> DofManager::getElementDofs(const Element& element) const {
        std::vector<int> dofs;
        const auto& nodes = element.getNodes();
        dofs.reserve(nodes.size() * dofs_per_node_);

        for (const auto& node : nodes) {
            int start_dof = node_dof_map_.at(node->getId());
            for (int i = 0; i < dofs_per_node_; ++i) {
                dofs.push_back(start_dof + i);
            }
        }
        return dofs;
    }
    
    int DofManager::getNodeDof(int node_id, int component) const {
        if (component < 0 || component >= dofs_per_node_) {
            throw std::out_of_range("DOF component is out of range.");
        }
        return node_dof_map_.at(node_id) + component;
    }

    size_t DofManager::getNumDofs() const {
        return total_dofs_;
    }

} // namespace FEM