#pragma once

#include "MaterialProperty.hpp" // 包含新头文件
#include <string>
#include <map>
#include <memory>

namespace FEM {

    class Material {
    public:
        explicit Material(const char* name) : name_(name) {}

        // 设置一个常数属性
        void setProperty(const std::string& name, double value) {
            properties_[name] = std::make_shared<MaterialProperty>(value);
        }

        // 设置一个函数属性
        void setProperty(const std::string& name, const std::function<double(const std::map<std::string, double>&)>& func) {
            properties_[name] = std::make_shared<MaterialProperty>(func);
        }

        // 获取属性对象
        const MaterialProperty& getProperty(const std::string& name) const {
            if (auto it = properties_.find(name); it != properties_.end()) {
                return *it->second;
            }
            throw std::runtime_error("Material property '" + name + "' not found.");
        }

        const std::string& getName() const { return name_; }

    private:
        std::string name_;
        std::map<std::string, std::shared_ptr<MaterialProperty>> properties_;
    };
}