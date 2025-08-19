#include "Material.hpp"

namespace FEM {

    void Material::setProperty(const std::string& property_name, double value) {
        properties_[property_name] = value;
    }

    double Material::getProperty(const std::string& property_name) const {
        // 使用 .at() 会在键不存在时抛出 std::out_of_range 异常，比 [] 更安全
        try {
            return properties_.at(property_name);
        } catch (const std::out_of_range& e) {
            // 抛出更具描述性的异常信息
            throw std::runtime_error("Material property '" + property_name + "' not found in material '" + name_ + "'.");
        }
    }

    const std::string& Material::getName() const {
        return name_;
    }

} // namespace FEM