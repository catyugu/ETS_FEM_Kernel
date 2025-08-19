#pragma once

#include "../mesh/Mesh.hpp"
#include <vector>
#include <map>
#include <set>

namespace FEM {

    class DofManager {
    public:
        // 构造函数，需要一个网格的引用来初始化
        explicit DofManager(const Mesh& mesh);

        // 构建自由度映射表
        // dofs_per_node: 每个节点的自由度数量 (例如，热学问题为1)
        void buildDofMap(int dofs_per_node);

        // 获取一个单元所有节点的自由度索引
        std::vector<int> getElementDofs(const Element& element) const;

        // 获取指定节点的特定自由度索引
        int getNodeDof(int node_id, int component) const;

        // 获取总的自由度数量
        size_t getNumDofs() const;

        // 计算稀疏模式，为矩阵预分配做准备
        std::vector<std::pair<int, int>> computeSparsityPattern(const Mesh& mesh) const;

    private:
        const Mesh& mesh_;
        int dofs_per_node_ = 0;
        size_t total_dofs_ = 0;
        
        // 存储每个节点ID对应的第一个自由度的索引
        // 例如，node_dof_map_[node_id] = 5，且每个节点有2个自由度，
        // 则该节点的自由度索引为 5 和 6。
        std::map<int, int> node_dof_map_;
    };

} // namespace FEM