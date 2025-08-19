#include <gtest/gtest.h>
#include "materials/Material.hpp"
#include <functional> // For std::function

// 测试套件 (Test Suite) 的名称是 MaterialTest
// 测试用例 (Test Case) 的名称是 SetAndGetProperty

TEST(MaterialTest, SetAndGetConstantProperty) {
    // 准备 (Arrange)
    FEM::Material test_mat("TestMaterial");
    const std::string prop_name = "thermal_conductivity";
    const double prop_value = 385.0;

    // 操作 (Act)
    test_mat.setProperty(prop_name, prop_value);
    // --- 关键修正：先获取属性对象，再求值 ---
    const FEM::MaterialProperty& retrieved_prop = test_mat.getProperty(prop_name);
    double retrieved_value = retrieved_prop.evaluate();

    // 断言 (Assert)
    ASSERT_DOUBLE_EQ(retrieved_value, prop_value);
}

TEST(MaterialTest, SetAndGetFunctionalProperty) {
    FEM::Material test_mat("TestMaterial");
    const std::string prop_name = "conductivity_func";
    auto conductivity_func = [](const std::map<std::string, double>& vars) {
        double T = vars.at("Temperature");
        return 100.0 * (1.0 + 0.1 * (T - 300.0));
    };
    test_mat.setProperty(prop_name, conductivity_func);
    const FEM::MaterialProperty& retrieved_prop = test_mat.getProperty(prop_name);
    double value_at_300K = retrieved_prop.evaluate({{"Temperature", 300.0}});
    double value_at_400K = retrieved_prop.evaluate({{"Temperature", 400.0}});

    ASSERT_DOUBLE_EQ(value_at_300K, 100.0); // 100 * (1 + 0.1 * 0)

    ASSERT_DOUBLE_EQ(value_at_400K, 1100.0); // 100 * (1 + 0.1 * 100)
}


TEST(MaterialTest, GetNonExistentProperty) {
    // 准备 (Arrange)
    FEM::Material test_mat("TestMaterial");

    // 操作与断言 (Act & Assert)
    ASSERT_THROW(test_mat.getProperty("non_existent_prop"), std::runtime_error);
}