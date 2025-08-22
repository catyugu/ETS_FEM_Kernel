#pragma once

#include <Eigen/Dense>

namespace FEM {
    // 前向声明
    class FEValues;

    class QuadraturePoint {
    public:
        // 通过 FEValues 访问私有数据
        friend class FEValues;

        const Eigen::VectorXd& N() const;       // 形函数值 N
        const Eigen::MatrixXd& dN_dx() const;   // 形函数在物理坐标系下的导数 dN/dx
        double JxW() const;                     // 雅可比行列式 * 积分权重

    private:
        // 私有构造函数，只能由 FEValues 创建
        QuadraturePoint(const FEValues& fe_values, size_t q_index)
            : fe_values_(fe_values), q_index_(q_index) {}

        const FEValues& fe_values_;
        size_t q_index_;
    };
}