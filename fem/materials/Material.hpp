#pragma once

#include <string>
#include <map>
#include <stdexcept>

namespace FEM {

    class Material {
    public:
        // 使用 C-style 字符串作为 name 参数，避免在构造函数体之前进行不必要的 std::string 构造
        explicit Material(const char* name) : name_(name) {}

        // 设置材料属性，值为 double 类型
        void setProperty(const std::string& property_name, double value);

        // 获取材料属性
        double getProperty(const std::string& property_name) const;

        // 获取材料名称
        const std::string& getName() const;

    private:
        std::string name_;
        std::map<std::string, double> properties_;
    };

} // namespace FEM