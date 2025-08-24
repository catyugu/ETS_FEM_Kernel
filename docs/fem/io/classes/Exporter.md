# Exporter 类

## 描述

`Exporter` 类负责将有限元分析结果导出到文件中，以便进行后处理和可视化。目前支持将结果导出为 VTK 格式文件，这是一种广泛使用的科学数据可视化格式。

## 类定义

```cpp
class Exporter
```

## 成员函数

### template<int TDim, typename TScalar, typename... Args> static void write_vtk(const std::string& filename, const Problem<TDim, TScalar>& problem, Args... var_names)

将有限元问题的解导出为 VTK 格式的文件。支持选择性导出特定变量。

**模板参数:**
- `TDim` - 问题的空间维度
- `TScalar` - 标量类型，支持double和std::complex<double>等类型
- `Args` - 可变参数包，用于指定要导出的变量名称

**参数:**
- `filename` - 输出文件的路径
- `problem` - 有限元问题对象的常量引用
- `var_names` - 要导出的变量名称列表（可选）。如果不提供任何变量名，则导出所有变量。

**异常:**
- `std::runtime_error` - 当无法打开文件进行写入时抛出

### template<int TDim, typename TScalar> static void write_vtk(const std::string& filename, const Problem<TDim, TScalar>& problem)

将有限元问题的所有解变量导出为 VTK 格式的文件。这是旧版本接口，向后兼容。

**模板参数:**
- `TDim` - 问题的空间维度
- `TScalar` - 标量类型，支持double和std::complex<double>等类型

**参数:**
- `filename` - 输出文件的路径
- `problem` - 有限元问题对象的常量引用

**异常:**
- `std::runtime_error` - 当无法打开文件进行写入时抛出

## 示例用法

```
// 假设已经有一个求解完成的问题对象
FEM::Problem<2> problem(std::move(mesh), std::move(physics));

// ... 执行求解过程 ...

// 导出所有变量（向后兼容用法）
try {
    FEM::IO::Exporter::write_vtk("results_all.vtk", problem);
    std::cout << "All results exported successfully to results_all.vtk" << std::endl;
} catch (const std::runtime_error& e) {
    std::cerr << "Export failed: " << e.what() << std::endl;
}

// 只导出特定变量
try {
    FEM::IO::Exporter::write_vtk("results_voltage.vtk", problem, "Voltage");
    std::cout << "Voltage results exported successfully to results_voltage.vtk" << std::endl;
} catch (const std::runtime_error& e) {
    std::cerr << "Export failed: " << e.what() << std::endl;
}

// 导出多个特定变量
try {
    FEM::IO::Exporter::write_vtk("results_multi.vtk", problem, "Voltage", "Temperature");
    std::cout << "Voltage and Temperature results exported successfully to results_multi.vtk" << std::endl;
} catch (const std::runtime_error& e) {
    std::cerr << "Export failed: " << e.what() << std::endl;
}
```

## 实现细节

`Exporter` 类提供了一个模板函数 `write_vtk`，用于将 [Problem](../../core/classes/Problem.md) 对象中的结果数据导出为 VTK 格式的文件。VTK（Visualization Toolkit）格式是一种广泛支持的科学数据格式，可以被多种可视化软件（如 ParaView）读取。

导出的数据包括：
1. 网格节点坐标
2. 单元连接信息
3. 节点解（标量场）

该函数通过 [Problem](../../core/classes/Problem.md) 类的公共访问器获取网格和解数据：
- [getMesh()](../../core/classes/Problem.md) - 获取网格数据
- [getSolution()](../../core/classes/Problem.md) - 获取解向量

支持的单元类型：
- 线单元（2节点）
- 三角形单元（3节点）
- 四面体单元（4节点）

## 依赖关系

- [Problem](../../core/classes/Problem.md) - 有限元问题类
- STL - 文件流等标准库组件