#pragma once

#include <string>

// --- 正确的模板类前向声明 ---
namespace FEM {
    template<int TDim>
    class Problem;
}

namespace FEM::IO {

    class Exporter {
    public:
        template<int TDim>
        static void write_vtk(const std::string& filename, const Problem<TDim>& problem);
    };

} // namespace FEM::IO