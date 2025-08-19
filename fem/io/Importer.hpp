#pragma once

#include <string>
#include <memory>
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
    };

} // namespace FEM::IO