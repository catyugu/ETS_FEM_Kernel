#pragma once
#include <vector>
#include <memory>
#include <limits>
#include <cmath>
#include "../fem/mesh/Node.hpp"
#undef min
#undef max

inline int findClosestNode(const std::vector<std::unique_ptr<FEM::Node>>& nodes, const std::vector<double>& target_coords, double tolerance = 1e-10) {
    int closest_index = -1;
    double min_distance = std::numeric_limits<double>::max();

    for (size_t i = 0; i < nodes.size(); ++i) {
        const auto& coords = nodes[i]->getCoords();

        double distance = 0.0;
        for (size_t j = 0; j < coords.size(); ++j) {
            distance += (coords[j] - target_coords[j]) * (coords[j] - target_coords[j]);
        }
        distance = std::sqrt(distance);

        if (distance < min_distance) {
            min_distance = distance;
            closest_index = static_cast<int>(i);
        }
    }

    // 如果最近节点距离在容差范围内，则认为是匹配的
    if (min_distance <= tolerance) {
        return closest_index;
    }

    return -1; // 没有找到足够接近的节点
}
inline void ASSERT_MATRIX_NEAR(const Eigen::MatrixXd& actual, const Eigen::MatrixXd& expected, double tolerance) {
    ASSERT_EQ(actual.rows(), expected.rows());
    ASSERT_EQ(actual.cols(), expected.cols());
    for (int i = 0; i < actual.rows(); ++i) {
        for (int j = 0; j < actual.cols(); ++j) {
            ASSERT_NEAR(actual(i, j), expected(i, j), tolerance)
                << "Mismatch at (" << i << ", " << j << ")";
        }
    }
}