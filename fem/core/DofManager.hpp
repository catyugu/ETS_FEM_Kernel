#pragma once

#include "../mesh/Mesh.hpp"
#include <vector>
#include <map>
#include <set>
#include <string>
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
     * 支持多物理场变量管理
     */
    class DofManager {
    public:
        /**
         * @brief 构造函数
         * @param mesh 网格对象的引用
         */
        explicit DofManager(const Mesh& mesh);

        // --- 新增接口 ---
        /**
         * @brief 添加一个自由度变量 (例如 "Voltage" 或 "Temperature")
         * @param name 变量名称
         * @param type 自由度类型 (NODE, EDGE, etc.)
         * @param components 每个实体的分量数 (例如矢量为2或3)
         */
        void addVariable(const std::string& name, DofType type, int components = 1);

        /**
         * @brief 根据已添加的变量构建所有自由度映射
         */
        void finalize();

        // --- 兼容旧接口 (为了不破坏现有测试) ---
        /**
         * @brief 构建自由度映射 (旧版，内部将创建一个名为 "default" 的变量)
         * @deprecated 推荐使用 addVariable 和 finalize 接口
         */
        void buildDofMap(int dofs_per_entity, DofType dof_type = DofType::NODE) [[deprecated("Use addVariable + finalize instead")]];

        // --- 修改/重载自由度获取函数 ---
        int getNodeDof(const std::string& var_name, int node_id, int component = 0) const;
        int getEdgeDof(const std::string& var_name, int edge_id, int component = 0) const;
        int getFaceDof(const std::string& var_name, int face_id, int component = 0) const;
        int getVolumeDof(const std::string& var_name, int volume_id, int component = 0) const;

        // --- 兼容旧接口 ---
        int getNodeDof(int node_id, int dof_component = 0) const;
        int getEdgeDof(int edge_id, int dof_component = 0) const;
        int getFaceDof(int face_id, int dof_component = 0) const;
        int getVolumeDof(int volume_id, int dof_component = 0) const;

        /**
         * @brief 获取自由度总数
         * @return 自由度总数
         */
        size_t getNumDofs() const;
        
        /**
         * @brief 获取特定变量的自由度数
         * @param var_name 变量名称
         * @return 特定变量的自由度数
         */
        size_t getNumDofs(const std::string& var_name) const;

        /**
         * @brief 计算稀疏模式
         * @param mesh 网格对象
         * @return 稀疏模式（非零元素位置的集合）
         */
        std::vector<std::pair<int, int>> computeSparsityPattern(const Mesh& mesh) const;

    private:
        struct Variable {
            std::string name;
            DofType type;
            int components;
            size_t start_dof_index; // 该变量在全局自由度中的起始索引
            std::map<int, int> dof_map_; // 实体ID -> 自由度偏移
        };

        /**
         * @brief 为特定变量构建自由度映射
         * @param var 变量引用
         */
        void buildDofMapForVariable(Variable& var);

        /**
         * @brief 获取单元上所有变量的自由度
         * @param elem 单元指针
         * @param dofs 输出的自由度向量
         */
        void getElementDofs(const Element* elem, std::vector<int>& dofs) const;

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
        size_t total_dofs_;
        
        // 新增：多变量支持
        std::map<std::string, Variable> variables_;
        std::vector<std::string> variable_names_; // 保持变量添加的顺序
        
        // 旧接口兼容性成员
        int dofs_per_entity_;
        DofType dof_type_;
        std::map<int, int> node_dof_map_;
        std::map<int, int> edge_dof_map_;
        std::map<int, int> face_dof_map_;
        std::map<int, int> volume_dof_map_;
    };

} // namespace FEM