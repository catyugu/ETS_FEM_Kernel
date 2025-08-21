#pragma once
#include <Eigen/Dense>
#include "../mesh/Element.hpp"
#include <complex> // 包含 complex 头文件

namespace FEM {
    template<int TDim, typename TScalar = double>
    class Kernel {
    public:
        // 返回类型变为动态大小的矩阵
        using MatrixType = Eigen::Matrix<TScalar, Eigen::Dynamic, Eigen::Dynamic>;

        virtual ~Kernel() = default;
        virtual MatrixType compute_element_matrix(const Element& element) = 0;
    };
}