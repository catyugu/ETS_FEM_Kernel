#pragma once

#include <string>
#include <memory>
#include <map>
#include "../mesh/Mesh.hpp"

namespace FEM::IO {

    class Importer {
    public:
        /**
         * @brief 从 COMSOL .mphtxt 文本文件读取网格。
         * @param filename 输入文件的路径。
         * @return 指向新创建的 Mesh 对象的 unique_ptr，失败则为空指针。
         */
        static std::unique_ptr<Mesh> read_comsol_mphtxt(const std::string& filename);

        static std::pair<std::unique_ptr<Mesh>, std::vector<double>> read_vtu(const std::string& filename);
        
        /**
         * @brief 从VTU文件读取指定名称的点数据字段
         * @param filename VTU文件路径
         * @param field_name 要读取的字段名称
         * @return pair包含网格和指定字段的数据
         */
        static std::pair<std::unique_ptr<Mesh>, std::vector<double>> read_vtu_point_data_field(
            const std::string& filename, const std::string& field_name);
    };

} // namespace FEM::IO