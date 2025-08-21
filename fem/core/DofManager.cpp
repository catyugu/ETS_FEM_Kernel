#include "DofManager.hpp"
#include <stdexcept>
#include <algorithm>
#include <set>

namespace FEM {

    DofManager::DofManager(const Mesh& mesh) : mesh_(mesh), dofs_per_entity_(1), 
        dof_type_(DofType::NODE), total_dofs_(0) {
    }

    void DofManager::addVariable(const std::string& name, DofType type, int components) {
        if (variables_.find(name) != variables_.end()) {
            throw std::runtime_error("Variable '" + name + "' already exists");
        }
        
        Variable var;
        var.name = name;
        var.type = type;
        var.components = components;
        var.start_dof_index = 0; // 将在finalize中设置
        variables_[name] = var;
        variable_names_.push_back(name);
    }

    void DofManager::finalize() {
        total_dofs_ = 0;
        
        // 为每个变量分配起始索引并构建自由度映射
        for (auto& pair : variables_) {
            Variable& var = pair.second;
            var.start_dof_index = total_dofs_;
            buildDofMapForVariable(var);
        }
    }

    void DofManager::buildDofMapForVariable(Variable& var) {
        switch (var.type) {
            case DofType::NODE: {
                const auto& nodes = mesh_.getNodes();
                var.dof_map_.clear();
                for (size_t i = 0; i < nodes.size(); ++i) {
                    var.dof_map_[nodes[i]->getId()] = static_cast<int>(i * var.components);
                }
                total_dofs_ += nodes.size() * var.components;
                break;
            }
            case DofType::EDGE: {
                // 使用Mesh类提供的边信息
                const auto& edges = mesh_.getEdges();
                var.dof_map_.clear();
                for (size_t i = 0; i < edges.size(); ++i) {
                    var.dof_map_[edges[i]->id] = static_cast<int>(i * var.components);
                }
                total_dofs_ += edges.size() * var.components;
                break;
            }
            case DofType::FACE: {
                // 使用Mesh类提供的面信息
                const auto& faces = mesh_.getFaces();
                var.dof_map_.clear();
                for (size_t i = 0; i < faces.size(); ++i) {
                    var.dof_map_[faces[i]->id] = static_cast<int>(i * var.components);
                }
                total_dofs_ += faces.size() * var.components;
                break;
            }
            case DofType::VOLUME: {
                // 实现体自由度映射
                const auto& elements = mesh_.getElements();
                var.dof_map_.clear();
                for (size_t i = 0; i < elements.size(); ++i) {
                    var.dof_map_[elements[i]->getId()] = static_cast<int>(i * var.components);
                }
                total_dofs_ += elements.size() * var.components;
                break;
            }
        }
    }

    void DofManager::buildDofMap(int dofs_per_entity, DofType dof_type) {
        addVariable("default", dof_type, dofs_per_entity);
        finalize();
    }

    int DofManager::getNodeDof(const std::string& var_name, int node_id, int component) const {
        auto it = variables_.find(var_name);
        if (it != variables_.end()) {
            const Variable& var = it->second;
            if (var.type == DofType::NODE) {
                auto dof_it = var.dof_map_.find(node_id);
                if (dof_it != var.dof_map_.end()) {
                    return static_cast<int>(var.start_dof_index) + dof_it->second + component;
                }
            }
        }
        return -1; // 未找到
    }

    int DofManager::getEdgeDof(const std::string& var_name, int edge_id, int component) const {
        auto it = variables_.find(var_name);
        if (it != variables_.end()) {
            const Variable& var = it->second;
            if (var.type == DofType::EDGE) {
                auto dof_it = var.dof_map_.find(edge_id);
                if (dof_it != var.dof_map_.end()) {
                    return static_cast<int>(var.start_dof_index) + dof_it->second + component;
                }
            }
        }
        return -1; // 未找到
    }

    int DofManager::getFaceDof(const std::string& var_name, int face_id, int component) const {
        auto it = variables_.find(var_name);
        if (it != variables_.end()) {
            const Variable& var = it->second;
            if (var.type == DofType::FACE) {
                auto dof_it = var.dof_map_.find(face_id);
                if (dof_it != var.dof_map_.end()) {
                    return static_cast<int>(var.start_dof_index) + dof_it->second + component;
                }
            }
        }
        return -1; // 未找到
    }

    int DofManager::getVolumeDof(const std::string& var_name, int volume_id, int component) const {
        auto it = variables_.find(var_name);
        if (it != variables_.end()) {
            const Variable& var = it->second;
            if (var.type == DofType::VOLUME) {
                auto dof_it = var.dof_map_.find(volume_id);
                if (dof_it != var.dof_map_.end()) {
                    return static_cast<int>(var.start_dof_index) + dof_it->second + component;
                }
            }
        }
        return -1; // 未找到
    }

    int DofManager::getNodeDof(int node_id, int dof_component) const {
        return getNodeDof("default", node_id, dof_component);
    }

    int DofManager::getEdgeDof(int edge_id, int dof_component) const {
        return getEdgeDof("default", edge_id, dof_component);
    }

    int DofManager::getFaceDof(int face_id, int dof_component) const {
        return getFaceDof("default", face_id, dof_component);
    }

    int DofManager::getVolumeDof(int volume_id, int dof_component) const {
        return getVolumeDof("default", volume_id, dof_component);
    }

    size_t DofManager::getNumDofs() const { 
        return total_dofs_; 
    }
    
    size_t DofManager::getNumDofs(const std::string& var_name) const {
        auto it = variables_.find(var_name);
        if (it != variables_.end()) {
            const Variable& var = it->second;
            return var.dof_map_.size() * var.components;
        }
        return 0;
    }

    void DofManager::getElementDofs(const Element* elem, std::vector<int>& dofs) const {
        dofs.clear();
        
        // 遍历所有变量
        for (const auto& pair : variables_) {
            const Variable& var = pair.second;
            std::vector<int> var_dofs;
            
            // 根据变量类型获取自由度索引
            switch (var.type) {
                case DofType::NODE: {
                    const auto& nodes = elem->getNodes();
                    var_dofs.reserve(nodes.size() * var.components);

                    for (const auto& node : nodes) {
                        for (int i = 0; i < var.components; ++i) {
                            int dof = getNodeDof(var.name, node->getId(), i);
                            if (dof >= 0) var_dofs.push_back(dof);
                        }
                    }
                    break;
                }
                case DofType::EDGE: {
                    // 对于边自由度，使用Mesh类提供的边信息
                    const auto& edges = mesh_.getEdges();
                    var_dofs.reserve(edges.size() * var.components);
                    
                    // 查找与当前单元相关的边
                    const auto& elem_nodes = elem->getNodes();
                    std::set<int> elem_node_ids;
                    for (const auto& node : elem_nodes) {
                        elem_node_ids.insert(node->getId());
                    }
                    
                    for (const auto& edge : edges) {
                        // 检查边的两个节点是否都在当前单元中
                        if (elem_node_ids.count(edge->node_ids.first) && 
                            elem_node_ids.count(edge->node_ids.second)) {
                            for (int j = 0; j < var.components; ++j) {
                                int dof = getEdgeDof(var.name, edge->id, j);
                                if (dof >= 0) var_dofs.push_back(dof);
                            }
                        }
                    }
                    break;
                }
                case DofType::FACE: {
                    // 对于面自由度，使用Mesh类提供的面信息
                    const auto& faces = mesh_.getFaces();
                    var_dofs.reserve(faces.size() * var.components);
                    
                    // 查找与当前单元相关的面
                    const auto& elem_nodes = elem->getNodes();
                    std::set<int> elem_node_ids;
                    for (const auto& node : elem_nodes) {
                        elem_node_ids.insert(node->getId());
                    }
                    
                    for (const auto& face : faces) {
                        // 检查面的所有节点是否都在当前单元中
                        bool all_nodes_in_elem = true;
                        for (int node_id : face->node_ids) {
                            if (elem_node_ids.find(node_id) == elem_node_ids.end()) {
                                all_nodes_in_elem = false;
                                break;
                            }
                        }
                        
                        if (all_nodes_in_elem) {
                            for (int i = 0; i < var.components; ++i) {
                                int dof = getFaceDof(var.name, face->id, i);
                                if (dof >= 0) var_dofs.push_back(dof);
                            }
                        }
                    }
                    break;
                }
                case DofType::VOLUME: {
                    const auto& nodes = elem->getNodes();
                    var_dofs.reserve(nodes.size() * var.components);

                    // 对于体自由度，使用单元ID作为体标识
                    int volume_id = elem->getId();
                    
                    for (int i = 0; i < var.components; ++i) {
                        int dof = getVolumeDof(var.name, volume_id, i);
                        if (dof >= 0) var_dofs.push_back(dof);
                    }
                    break;
                }
            }
            
            // 将变量的自由度添加到总自由度列表中
            dofs.insert(dofs.end(), var_dofs.begin(), var_dofs.end());
        }
    }

    std::vector<std::pair<int, int>> DofManager::computeSparsityPattern(const Mesh& mesh) const {
        std::set<std::pair<int, int>> sparsity_pattern;

        // 遍历所有单元
        for (const auto& elem : mesh.getElements()) {
            std::vector<int> dofs;
            getElementDofs(elem.get(), dofs); // 获取该单元上所有变量的自由度

            // 对于每个自由度对，添加到稀疏模式中
            for (size_t i = 0; i < dofs.size(); ++i) {
                for (size_t j = 0; j < dofs.size(); ++j) {
                    sparsity_pattern.insert({dofs[i], dofs[j]});
                }
            }
        }

        // 转换为向量返回
        return std::vector<std::pair<int, int>>(sparsity_pattern.begin(), sparsity_pattern.end());
    }

    void DofManager::buildNodeDofMap() {
        const auto& nodes = mesh_.getNodes();
        total_dofs_ = nodes.size() * dofs_per_entity_;
        
        for (size_t i = 0; i < nodes.size(); ++i) {
            node_dof_map_[nodes[i]->getId()] = static_cast<int>(i * dofs_per_entity_);
        }
    }

    void DofManager::buildEdgeDofMap() {
        // 使用Mesh类提供的边信息
        const auto& edges = mesh_.getEdges();
        total_dofs_ = edges.size() * dofs_per_entity_;
        
        for (size_t i = 0; i < edges.size(); ++i) {
            edge_dof_map_[edges[i]->id] = static_cast<int>(i * dofs_per_entity_);
        }
    }

    void DofManager::buildFaceDofMap() {
        // 使用Mesh类提供的面信息
        const auto& faces = mesh_.getFaces();
        total_dofs_ = faces.size() * dofs_per_entity_;
        
        for (size_t i = 0; i < faces.size(); ++i) {
            face_dof_map_[faces[i]->id] = static_cast<int>(i * dofs_per_entity_);
        }
    }

    void DofManager::buildVolumeDofMap() {
        // 实现体自由度映射
        const auto& elements = mesh_.getElements();
        total_dofs_ = elements.size() * dofs_per_entity_;
        
        for (size_t i = 0; i < elements.size(); ++i) {
            volume_dof_map_[elements[i]->getId()] = static_cast<int>(i * dofs_per_entity_);
        }
    }

} // namespace FEM