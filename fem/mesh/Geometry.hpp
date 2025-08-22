#pragma once

#include "Mesh.hpp"
#include "BoundaryDefinition.hpp"
#include <map>
#include <string>
#include <memory>

namespace FEM {
    class Mesh;

    class Geometry {
    public:
        Geometry(std::unique_ptr<Mesh> mesh) : mesh_(std::move(mesh)) {}

        Mesh& getMesh() { return *mesh_; }
        const Mesh& getMesh() const { return *mesh_; }

        void addBoundary(std::unique_ptr<BoundaryDefinition> boundary) {
            boundaries_[boundary->getName()] = std::move(boundary);
        }

        const BoundaryDefinition& getBoundary(const std::string& name) const {
            auto it = boundaries_.find(name);
            if (it == boundaries_.end()) {
                throw std::runtime_error("Boundary with name '" + name + "' not found.");
            }
            return *it->second;
        }

    private:
        std::unique_ptr<Mesh> mesh_;
        std::map<std::string, std::unique_ptr<BoundaryDefinition>> boundaries_;
    };
}