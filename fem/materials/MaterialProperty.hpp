#pragma once

#include <string>
#include <functional>
#include <map>
#include <stdexcept>
#include <utility>

namespace FEM {

    class MaterialProperty {
    public:
        // 构造一个常数属性
        MaterialProperty(double constant_value)
            : is_constant_(true), constant_value_(constant_value) {}

        // 构造一个函数属性
        MaterialProperty(std::function<double(const std::map<std::string, double>&)> func)
            : is_constant_(false), function_(std::move(func)) {}

        // 评估属性值
        double evaluate(const std::map<std::string, double>& variables = {}) const {
            if (is_constant_) {
                return constant_value_;
            }
            if (function_) {
                return function_(variables);
            }
            throw std::runtime_error("Material property is not properly defined.");
        }

    private:
        bool is_constant_;
        double constant_value_ = 0.0;
        std::function<double(const std::map<std::string, double>&)> function_;
    };
}