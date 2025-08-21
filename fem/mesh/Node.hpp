#pragma once
#include <vector>

namespace FEM {
    class Node {
    public:
        Node(int id, const std::vector<double>& coords) : id_(id), coords_(coords) {}
        int getId() const { return id_; }
        const std::vector<double>& getCoords() const { return coords_; }
        
        // 添加获取各维度坐标的方法
        double getX() const { return coords_.size() > 0 ? coords_[0] : 0.0; }
        double getY() const { return coords_.size() > 1 ? coords_[1] : 0.0; }
        double getZ() const { return coords_.size() > 2 ? coords_[2] : 0.0; }
        
    private:
        int id_;
        std::vector<double> coords_;
    };
}