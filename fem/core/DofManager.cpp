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
                // 实现边自由度映射
                // 需要遍历所有单元并识别唯一的边
                std::set<std::pair<int, int>> edges;
                
                for (const auto& elem : mesh_.getElements()) {
                    const auto& nodes = elem->getNodes();
                    // 根据单元类型确定边
                    switch (elem->getType()) {
                        case ElementType::Line:
                            // 线单元本身就是一条边
                            if (nodes.size() >= 2) {
                                std::pair<int, int> edge(nodes[0]->getId(), nodes[1]->getId());
                                if (edge.first > edge.second) std::swap(edge.first, edge.second);
                                edges.insert(edge);
                            }
                            break;
                        case ElementType::Triangle:
                            // 三角形单元有3条边
                            for (size_t i = 0; i < 3; ++i) {
                                size_t next = (i + 1) % 3;
                                std::pair<int, int> edge(nodes[i]->getId(), nodes[next]->getId());
                                if (edge.first > edge.second) std::swap(edge.first, edge.second);
                                edges.insert(edge);
                            }
                            break;
                        case ElementType::Quadrilateral:
                            // 四边形单元有4条边
                            for (size_t i = 0; i < 4; ++i) {
                                size_t next = (i + 1) % 4;
                                std::pair<int, int> edge(nodes[i]->getId(), nodes[next]->getId());
                                if (edge.first > edge.second) std::swap(edge.first, edge.second);
                                edges.insert(edge);
                            }
                            break;
                        case ElementType::Tetrahedron:
                            // 四面体单元有6条边
                            for (size_t i = 0; i < 4; ++i) {
                                for (size_t j = i + 1; j < 4; ++j) {
                                    std::pair<int, int> edge(nodes[i]->getId(), nodes[j]->getId());
                                    if (edge.first > edge.second) std::swap(edge.first, edge.second);
                                    edges.insert(edge);
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
                                edges.insert(edge);
                            }
                            // 顶面4条边
                            for (size_t i = 4; i < 8; ++i) {
                                size_t next = (i + 1) % 4 + 4;
                                std::pair<int, int> edge(nodes[i]->getId(), nodes[next]->getId());
                                if (edge.first > edge.second) std::swap(edge.first, edge.second);
                                edges.insert(edge);
                            }
                            // 垂直边4条
                            for (size_t i = 0; i < 4; ++i) {
                                std::pair<int, int> edge(nodes[i]->getId(), nodes[i+4]->getId());
                                if (edge.first > edge.second) std::swap(edge.first, edge.second);
                                edges.insert(edge);
                            }
                            break;
                        default:
                            break;
                    }
                }
                
                // 为每条边分配自由度
                int edge_index = 0;
                for (const auto& edge : edges) {
                    // 创建边ID（基于节点ID）
                    int edge_id = edge.first * 10000 + edge.second;
                    var.dof_map_[edge_id] = edge_index * var.components;
                    edge_index++;
                }
                total_dofs_ += edges.size() * var.components;
                break;
            }
            case DofType::FACE: {
                // 实现面自由度映射
                // 需要遍历所有单元并识别唯一的面
                std::set<std::set<int>> faces;
                
                for (const auto& elem : mesh_.getElements()) {
                    const auto& nodes = elem->getNodes();
                    // 根据单元类型确定面
                    switch (elem->getType()) {
                        case ElementType::Triangle:
                        case ElementType::Quadrilateral:
                            // 2D单元的面就是单元本身
                            {
                                std::set<int> face_nodes;
                                for (const auto& node : nodes) {
                                    face_nodes.insert(node->getId());
                                }
                                faces.insert(face_nodes);
                            }
                            break;
                        case ElementType::Tetrahedron:
                            // 四面体单元有4个面
                            // 面0: 节点 0,1,2
                            {
                                std::set<int> face_nodes = {nodes[0]->getId(), nodes[1]->getId(), nodes[2]->getId()};
                                faces.insert(face_nodes);
                            }
                            // 面1: 节点 0,1,3
                            {
                                std::set<int> face_nodes = {nodes[0]->getId(), nodes[1]->getId(), nodes[3]->getId()};
                                faces.insert(face_nodes);
                            }
                            // 面2: 节点 0,2,3
                            {
                                std::set<int> face_nodes = {nodes[0]->getId(), nodes[2]->getId(), nodes[3]->getId()};
                                faces.insert(face_nodes);
                            }
                            // 面3: 节点 1,2,3
                            {
                                std::set<int> face_nodes = {nodes[1]->getId(), nodes[2]->getId(), nodes[3]->getId()};
                                faces.insert(face_nodes);
                            }
                            break;
                        case ElementType::Hexahedron:
                            // 六面体单元有6个面
                            // 底面: 节点 0,1,2,3
                            {
                                std::set<int> face_nodes = {nodes[0]->getId(), nodes[1]->getId(), nodes[2]->getId(), nodes[3]->getId()};
                                faces.insert(face_nodes);
                            }
                            // 顶面: 节点 4,5,6,7
                            {
                                std::set<int> face_nodes = {nodes[4]->getId(), nodes[5]->getId(), nodes[6]->getId(), nodes[7]->getId()};
                                faces.insert(face_nodes);
                            }
                            // 前面: 节点 0,1,5,4
                            {
                                std::set<int> face_nodes = {nodes[0]->getId(), nodes[1]->getId(), nodes[5]->getId(), nodes[4]->getId()};
                                faces.insert(face_nodes);
                            }
                            // 后面: 节点 3,2,6,7
                            {
                                std::set<int> face_nodes = {nodes[3]->getId(), nodes[2]->getId(), nodes[6]->getId(), nodes[7]->getId()};
                                faces.insert(face_nodes);
                            }
                            // 左面: 节点 0,3,7,4
                            {
                                std::set<int> face_nodes = {nodes[0]->getId(), nodes[3]->getId(), nodes[7]->getId(), nodes[4]->getId()};
                                faces.insert(face_nodes);
                            }
                            // 右面: 节点 1,2,6,5
                            {
                                std::set<int> face_nodes = {nodes[1]->getId(), nodes[2]->getId(), nodes[6]->getId(), nodes[5]->getId()};
                                faces.insert(face_nodes);
                            }
                            break;
                        default:
                            break;
                    }
                }
                
                // 为每个面分配自由度
                int face_index = 0;
                for (const auto& face : faces) {
                    // 创建面ID（基于节点ID的哈希）
                    int face_id = 0;
                    for (int node_id : face) {
                        face_id = face_id * 12512515 + node_id;  // 简单的哈希算法
                    }
                    var.dof_map_[face_id] = face_index * var.components;
                    face_index++;
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
                    // 对于边自由度，需要确定单元的边
                    const auto& nodes = elem->getNodes();
                    var_dofs.reserve(nodes.size() * var.components);

                    // 这里需要根据单元类型确定边
                    // 简化实现：假定每两个相邻节点定义一条边
                    for (size_t i = 0; i < nodes.size(); ++i) {
                        size_t next = (i + 1) % nodes.size();
                        // 创建一个基于节点ID的边标识符
                        std::pair<int, int> edge_key(nodes[i]->getId(), nodes[next]->getId());
                        // 保证边的方向一致性（较小ID在前）
                        if (edge_key.first > edge_key.second) {
                            std::swap(edge_key.first, edge_key.second);
                        }
                        
                        // 将边标识符转换为整数ID（简化方法）
                        int edge_id = edge_key.first * 10000 + edge_key.second;
                        
                        for (int j = 0; j < var.components; ++j) {
                            int dof = getEdgeDof(var.name, edge_id, j);
                            if (dof >= 0) var_dofs.push_back(dof);
                        }
                    }
                    break;
                }
                case DofType::FACE: {
                    const auto& nodes = elem->getNodes();
                    var_dofs.reserve(nodes.size() * var.components);

                    // 对于面自由度，简化处理：使用所有节点ID来标识面
                    // 这是一种简化实现，实际应用中需要更精确的面标识方法
                    std::vector<int> node_ids;
                    for (const auto& node : nodes) {
                        node_ids.push_back(node->getId());
                    }
                    std::sort(node_ids.begin(), node_ids.end());
                    
                    // 创建面ID（简化方法：基于排序后的节点ID）
                    int face_id = 0;
                    for (size_t i = 0; i < std::min(node_ids.size(), size_t(4)); ++i) {
                        face_id = face_id * 1000 + node_ids[i];
                    }
                    
                    for (int i = 0; i < var.components; ++i) {
                        int dof = getFaceDof(var.name, face_id, i);
                        if (dof >= 0) var_dofs.push_back(dof);
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
        // 实现边自由度映射
        // 需要遍历所有单元并识别唯一的边
        std::set<std::pair<int, int>> edges;
        
        for (const auto& elem : mesh_.getElements()) {
            const auto& nodes = elem->getNodes();
            // 根据单元类型确定边
            switch (elem->getType()) {
                case ElementType::Line:
                    // 线单元本身就是一条边
                    if (nodes.size() >= 2) {
                        std::pair<int, int> edge(nodes[0]->getId(), nodes[1]->getId());
                        if (edge.first > edge.second) std::swap(edge.first, edge.second);
                        edges.insert(edge);
                    }
                    break;
                case ElementType::Triangle:
                    // 三角形单元有3条边
                    for (size_t i = 0; i < 3; ++i) {
                        size_t next = (i + 1) % 3;
                        std::pair<int, int> edge(nodes[i]->getId(), nodes[next]->getId());
                        if (edge.first > edge.second) std::swap(edge.first, edge.second);
                        edges.insert(edge);
                    }
                    break;
                case ElementType::Quadrilateral:
                    // 四边形单元有4条边
                    for (size_t i = 0; i < 4; ++i) {
                        size_t next = (i + 1) % 4;
                        std::pair<int, int> edge(nodes[i]->getId(), nodes[next]->getId());
                        if (edge.first > edge.second) std::swap(edge.first, edge.second);
                        edges.insert(edge);
                    }
                    break;
                case ElementType::Tetrahedron:
                    // 四面体单元有6条边
                    for (size_t i = 0; i < 4; ++i) {
                        for (size_t j = i + 1; j < 4; ++j) {
                            std::pair<int, int> edge(nodes[i]->getId(), nodes[j]->getId());
                            if (edge.first > edge.second) std::swap(edge.first, edge.second);
                            edges.insert(edge);
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
                        edges.insert(edge);
                    }
                    // 顶面4条边
                    for (size_t i = 4; i < 8; ++i) {
                        size_t next = (i + 1) % 4 + 4;
                        std::pair<int, int> edge(nodes[i]->getId(), nodes[next]->getId());
                        if (edge.first > edge.second) std::swap(edge.first, edge.second);
                        edges.insert(edge);
                    }
                    // 垂直边4条
                    for (size_t i = 0; i < 4; ++i) {
                        std::pair<int, int> edge(nodes[i]->getId(), nodes[i+4]->getId());
                        if (edge.first > edge.second) std::swap(edge.first, edge.second);
                        edges.insert(edge);
                    }
                    break;
            }
        }
        
        // 为每条边分配自由度
        total_dofs_ = edges.size() * dofs_per_entity_;
        int edge_index = 0;
        for (const auto& edge : edges) {
            // 创建边ID（基于节点ID）
            int edge_id = edge.first * 10000 + edge.second;
            edge_dof_map_[edge_id] = edge_index * dofs_per_entity_;
            edge_index++;
        }
    }

    void DofManager::buildFaceDofMap() {
        // 实现面自由度映射
        // 需要遍历所有单元并识别唯一的面
        std::set<std::set<int>> faces;
        
        for (const auto& elem : mesh_.getElements()) {
            const auto& nodes = elem->getNodes();
            // 根据单元类型确定面
            switch (elem->getType()) {
                case ElementType::Triangle:
                case ElementType::Quadrilateral:
                    // 2D单元的面就是单元本身
                    {
                        std::set<int> face_nodes;
                        for (const auto& node : nodes) {
                            face_nodes.insert(node->getId());
                        }
                        faces.insert(face_nodes);
                    }
                    break;
                case ElementType::Tetrahedron:
                    // 四面体单元有4个面
                    // 面0: 节点 0,1,2
                    {
                        std::set<int> face_nodes = {nodes[0]->getId(), nodes[1]->getId(), nodes[2]->getId()};
                        faces.insert(face_nodes);
                    }
                    // 面1: 节点 0,1,3
                    {
                        std::set<int> face_nodes = {nodes[0]->getId(), nodes[1]->getId(), nodes[3]->getId()};
                        faces.insert(face_nodes);
                    }
                    // 面2: 节点 0,2,3
                    {
                        std::set<int> face_nodes = {nodes[0]->getId(), nodes[2]->getId(), nodes[3]->getId()};
                        faces.insert(face_nodes);
                    }
                    // 面3: 节点 1,2,3
                    {
                        std::set<int> face_nodes = {nodes[1]->getId(), nodes[2]->getId(), nodes[3]->getId()};
                        faces.insert(face_nodes);
                    }
                    break;
                case ElementType::Hexahedron:
                    // 六面体单元有6个面
                    // 底面: 节点 0,1,2,3
                    {
                        std::set<int> face_nodes = {nodes[0]->getId(), nodes[1]->getId(), nodes[2]->getId(), nodes[3]->getId()};
                        faces.insert(face_nodes);
                    }
                    // 顶面: 节点 4,5,6,7
                    {
                        std::set<int> face_nodes = {nodes[4]->getId(), nodes[5]->getId(), nodes[6]->getId(), nodes[7]->getId()};
                        faces.insert(face_nodes);
                    }
                    // 前面: 节点 0,1,5,4
                    {
                        std::set<int> face_nodes = {nodes[0]->getId(), nodes[1]->getId(), nodes[5]->getId(), nodes[4]->getId()};
                        faces.insert(face_nodes);
                    }
                    // 后面: 节点 3,2,6,7
                    {
                        std::set<int> face_nodes = {nodes[3]->getId(), nodes[2]->getId(), nodes[6]->getId(), nodes[7]->getId()};
                        faces.insert(face_nodes);
                    }
                    // 左面: 节点 0,3,7,4
                    {
                        std::set<int> face_nodes = {nodes[0]->getId(), nodes[3]->getId(), nodes[7]->getId(), nodes[4]->getId()};
                        faces.insert(face_nodes);
                    }
                    // 右面: 节点 1,2,6,5
                    {
                        std::set<int> face_nodes = {nodes[1]->getId(), nodes[2]->getId(), nodes[6]->getId(), nodes[5]->getId()};
                        faces.insert(face_nodes);
                    }
                    break;
                default:
                    break;
            }
        }
        
        // 为每个面分配自由度
        total_dofs_ = faces.size() * dofs_per_entity_;
        int face_index = 0;
        for (const auto& face : faces) {
            // 创建面ID（基于节点ID的哈希）
            int face_id = 0;
            for (int node_id : face) {
                face_id = face_id * 12512515 + node_id;  // 简单的哈希算法
            }
            face_dof_map_[face_id] = face_index * dofs_per_entity_;
            face_index++;
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