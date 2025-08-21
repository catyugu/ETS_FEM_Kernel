#pragma once

#include "../mesh/Mesh.hpp"
#include <vector>
#include <map>
#include <set>
#include <Eigen/Sparse>

namespace FEM {

    // 自由度类型枚举
    enum class DofType {
        NODE,
        EDGE,
        FACE,
        VOLUME
    };

    /**
     * @brief 自由度管理器
     * 
     * 管理网格中各个几何实体（节点、边、面、体）上的自由度，
     * 支持多种自由度类型以适应不同的单元类型（如边缘元）
     */
    class DofManager {
    public:
        /**
         * @brief 构造函数
         * @param mesh 网格对象的引用
         */
        explicit DofManager(const Mesh& mesh);

        /**
         * @brief 构建自由度映射
         * @param dofs_per_entity 每个几何实体上的自由度数量
         * @param dof_type 自由度类型（默认为节点自由度）
         */
        void buildDofMap(int dofs_per_entity, DofType dof_type = DofType::NODE);

        /**
         * @brief 获取节点自由度索引
         * @param node_id 节点ID
         * @param dof_component 自由度分量（对于矢量问题）
         * @return 全局自由度索引
         */
        int getNodeDof(int node_id, int dof_component = 0) const;

        /**
         * @brief 获取边自由度索引
         * @param edge_id 边ID
         * @param dof_component 自由度分量
         * @return 全局自由度索引
         */
        int getEdgeDof(int edge_id, int dof_component = 0) const;

        /**
         * @brief 获取面自由度索引
         * @param face_id 面ID
         * @param dof_component 自由度分量
         * @return 全局自由度索引
         */
        int getFaceDof(int face_id, int dof_component = 0) const;

        /**
         * @brief 获取体自由度索引
         * @param volume_id 体ID
         * @param dof_component 自由度分量
         * @return 全局自由度索引
         */
        int getVolumeDof(int volume_id, int dof_component = 0) const;

        /**
         * @brief 获取自由度总数
         * @return 自由度总数
         */
        size_t getNumDofs() const;

        /**
         * @brief 计算稀疏模式
         * @param mesh 网格对象
         * @return 稀疏模式（非零元素位置的集合）
         */
        std::vector<std::pair<int, int>> computeSparsityPattern(const Mesh& mesh) const;

    private:
        /**
         * @brief 构建节点自由度映射
         */
        void buildNodeDofMap();

        /**
         * @brief 构建边自由度映射
         */
        void buildEdgeDofMap();

        /**
         * @brief 构建面自由度映射
         */
        void buildFaceDofMap();

        /**
         * @brief 构建体自由度映射
         */
        void buildVolumeDofMap();

        const Mesh& mesh_;
        int dofs_per_entity_;
        DofType dof_type_;
        size_t total_dofs_;
        
        // 各种类型的自由度映射
        std::map<int, int> node_dof_map_;
        std::map<int, int> edge_dof_map_;
        std::map<int, int> face_dof_map_;
        std::map<int, int> volume_dof_map_;
    };

} // namespace FEM