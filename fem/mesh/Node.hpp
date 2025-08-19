#pragma once
#include <vector>

namespace FEM {
    class Node {
    public:
        Node(int id, const std::vector<double>& coords) : id_(id), coords_(coords) {}
        int getId() const { return id_; }
        const std::vector<double>& getCoords() const { return coords_; }
    private:
        int id_;
        std::vector<double> coords_;
    };
}