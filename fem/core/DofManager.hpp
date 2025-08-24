#pragma once

#include "../mesh/Mesh.hpp"
#include <vector>
#include <map>
#include <string>
#include <set>
#include <Eigen/Sparse>

namespace FEM {

    /**
     * @brief 自由度类型枚举
     */
    enum class DofType {
        NODE,   ///< 节点自由度
        EDGE,   ///< 边自由度
        FACE,   ///< 面自由度
        VOLUME  ///< 体自由度
    };

    /**
     * @brief 自由度管理器类
     * 
     * 管理有限元网格中各种类型的自由度，支持节点、边、面和体自由度。
     * 支持多物理场变量，每个变量可以有不同的自由度类型和分量数。
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

        const Mesh& mesh_;
        size_t total_dofs_;
        
        // 新增：多变量支持
        std::map<std::string, Variable> variables_;
        std::vector<std::string> variable_names_; // 保持变量添加的顺序
    };

} // namespace FEM