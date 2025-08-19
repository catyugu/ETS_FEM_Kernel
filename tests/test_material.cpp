#include <gtest/gtest.h>
#include "materials/Material.hpp"

// 测试套件 (Test Suite) 的名称是 MaterialTest
// 测试用例 (Test Case) 的名称是 SetAndGetProperty

TEST(MaterialTest, SetAndGetProperty) {
    // 准备 (Arrange)
    FEM::Material test_mat("TestMaterial");
    const std::string prop_name = "thermal_conductivity";
    const double prop_value = 385.0;

    // 操作 (Act)
    test_mat.setProperty(prop_name, prop_value);
    double retrieved_value = test_mat.getProperty(prop_name);

    // 断言 (Assert)
    // 检查设置的值和取出的值是否相等
    ASSERT_DOUBLE_EQ(retrieved_value, prop_value);
}

TEST(MaterialTest, GetNonExistentProperty) {
    // 准备 (Arrange)
    FEM::Material test_mat("TestMaterial");

    // 操作与断言 (Act & Assert)
    // 检查当获取一个不存在的属性时，是否会按预期抛出 std::runtime_error 异常
    ASSERT_THROW(test_mat.getProperty("non_existent_prop"), std::runtime_error);
}