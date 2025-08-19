#pragma once
#include <Eigen/Dense>
#include "../mesh/Element.hpp"

namespace FEM {
    template<int TDim, int TNumNodes_>
    class Kernel {
    public:
        // 新增：静态成员，允许外部代码查询模板参数
        static constexpr int NumNodes = TNumNodes_;

        virtual ~Kernel() = default;
        virtual Eigen::Matrix<double, TNumNodes_, TNumNodes_>
        compute_element_matrix(const Element& element) = 0;
    };
}