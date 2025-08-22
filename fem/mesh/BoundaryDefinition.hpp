#pragma once

#include "Element.hpp"
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <set>

namespace FEM {
    class BoundaryDefinition {
    public:
        BoundaryDefinition(std::string  name) : name_(std::move(name)) {}

        void addElement(std::unique_ptr<Element> element) {
            elements_.push_back(std::move(element));
        }

        const std::string& getName() const { return name_; }
        const std::vector<std::unique_ptr<Element>>& getElements() const { return elements_; }

        // 辅助函数，获取边界上所有唯一的节点ID
        std::vector<int> getUniqueNodeIds() const {
            std::set<int> unique_node_ids;
            for (const auto& elem : elements_) {
                for (size_t i = 0; i < elem->getNumNodes(); ++i) {
                    unique_node_ids.insert(elem->getNodeId(i));
                }
            }
            return std::vector<int>(unique_node_ids.begin(), unique_node_ids.end());
        }

    private:
        std::string name_;
        std::vector<std::unique_ptr<Element>> elements_;
    };
}