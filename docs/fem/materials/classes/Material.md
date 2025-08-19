# Material 类

## 描述

`Material` 类表示有限元分析中的材料，用于存储和管理材料的各种物理属性。该类使用属性名称作为键，存储一组 [MaterialProperty](file:///E:/code/cpp/ETS_FEM_Kernel/fem/materials/MaterialProperty.hpp#L15-L50) 对象，可以表示常数属性或依赖于其他变量的函数属性。

## 类定义

```cpp
class Material
```

## 构造函数

### explicit Material(const char* name)

构造函数，使用给定的名称初始化材料对象。

**参数:**
- `name` - 材料名称

## 成员函数

### void setProperty(const std::string& name, double value)

设置一个常数材料属性。

**参数:**
- `name` - 属性名称
- `value` - 属性值

### void setProperty(const std::string& name, const std::function<double(const std::map<std::string, double>&)>& func)

设置一个函数材料属性。

**参数:**
- `name` - 属性名称
- `func` - 计算属性值的函数，接受一个变量映射并返回属性值

### const MaterialProperty& getProperty(const std::string& name) const

获取指定名称的材料属性。

**参数:**
- `name` - 属性名称

**返回值:**
- 指定名称的材料属性对象的常量引用

**异常:**
- `std::runtime_error` - 当指定名称的属性不存在时抛出

### const std::string& getName() const

获取材料名称。

**返回值:**
- 材料名称的常量引用

## 示例用法

```cpp
// 创建材料对象
FEM::Material material("Copper");

// 设置常数属性
material.setProperty("thermal_conductivity", 401.0); // W/(m·K)

// 设置函数属性
material.setProperty("density", [](const std::map<std::string, double>& vars) {
    // 密度可能依赖于温度等变量
    double T = vars.at("temperature");
    // 某种温度依赖关系
    return 8960.0 * (1.0 - 0.000017 * (T - 293.0));
});

// 获取属性
const auto& k_prop = material.getProperty("thermal_conductivity");
double k = k_prop.evaluate(); // 对于常数属性
```

## 实现细节

`Material` 类使用 `std::map<std::string, std::shared_ptr<MaterialProperty>>` 来存储材料属性，其中键是属性名称，值是 [MaterialProperty](file:///E:/code/cpp/ETS_FEM_Kernel/fem/materials/MaterialProperty.hpp#L15-L50) 对象的智能指针。这种设计允许材料拥有任意数量的不同属性，并且可以方便地通过名称检索。

## 依赖关系

- [MaterialProperty](file:///E:/code/cpp/ETS_FEM_Kernel/fem/materials/MaterialProperty.hpp#L15-L50) - 材料属性类
- STL - 字符串、映射、智能指针等标准库组件