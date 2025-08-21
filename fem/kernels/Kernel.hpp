#pragma once
#include <Eigen/Dense>
#include "../mesh/Element.hpp"
#include <complex> // 包含 complex 头文件

namespace FEM {
    template<int TDim, int TNumNodes_, typename TScalar = double>
    class Kernel {
    public:
        // 新增：静态成员，允许外部代码查询模板参数
        static constexpr int NumNodes = TNumNodes_;
        using MatrixType = Eigen::Matrix<TScalar, TNumNodes_, TNumNodes_>;

        virtual ~Kernel() = default;
        virtual MatrixType compute_element_matrix(const Element& element) = 0;
    };
}