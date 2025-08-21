# Importer 类

## 描述

`Importer` 类负责从外部文件导入网格数据。目前支持从 COMSOL 软件导出的 .mphtxt 格式文件和 VTK 的 .vtu 格式文件中读取网格信息。该类提供静态方法，用于将网格数据读取到 [Mesh](../../mesh/classes/Mesh.md) 对象中，以便在有限元分析中使用。

## 类定义

```cpp
class Importer
```

## 成员函数

### static std::unique_ptr<Mesh> read_comsol_mphtxt(const std::string& filename)

从 COMSOL .mphtxt 文本文件读取网格。

**参数:**
- `filename` - 输入文件的路径

**返回值:**
- 指向新创建的 [Mesh](../../mesh/classes/Mesh.md) 对象的 unique_ptr，失败则为空指针

**异常:**
- `std::runtime_error` - 当无法打开文件时抛出

### static std::pair<std::unique_ptr<Mesh>, std::vector<double>> read_vtu(const std::string& filename)

从 VTU 文件读取网格和解数据。

**参数:**
- `filename` - 输入文件的路径

**返回值:**
- pair 包含指向新创建的 [Mesh](../../mesh/classes/Mesh.md) 对象的 unique_ptr 和解数据向量

### static std::pair<std::unique_ptr<Mesh>, std::vector<double>> read_vtu_point_data_field(const std::string& filename, const std::string& field_name)

从VTU文件读取指定名称的点数据字段。

**参数:**
- `filename` - VTU文件路径
- `field_name` - 要读取的字段名称

**返回值:**
- pair 包含指向新创建的 [Mesh](../../mesh/classes/Mesh.md) 对象的 unique_ptr 和指定字段的数据

## 示例用法

```cpp
try {
    // 从COMSOL .mphtxt文件读取网格
    auto mesh = FEM::IO::Importer::read_comsol_mphtxt("mesh.mphtxt");
    
    if (mesh) {
        // 成功读取网格，可以用于后续的有限元分析
        std::cout << "Mesh loaded successfully with " 
                  << mesh->getNodes().size() << " nodes and " 
                  << mesh->getElements().size() << " elements." << std::endl;
    } else {
        // 读取失败
        std::cerr << "Failed to load mesh." << std::endl;
    }
    
    // 从VTU文件读取网格和解数据
    auto [vtu_mesh, solution_data] = FEM::IO::Importer::read_vtu("results.vtu");
    
    // 从VTU文件读取特定字段数据
    auto [field_mesh, field_data] = FEM::IO::Importer::read_vtu_point_data_field("results.vtu", "Temperature");
} catch (const std::runtime_error& e) {
    // 处理文件打开错误
    std::cerr << "Error: " << e.what() << std::endl;
}
```

## 实现细节

`Importer` 类支持多种网格文件格式：
1. COMSOL .mphtxt 格式 - COMSOL 软件导出的文本格式
2. VTK .vtu 格式 - VTK 软件的非结构化网格格式

读取过程包括：
1. 解析文件中的节点数量和单元数量
2. 读取节点坐标数据并创建 [Node](../../mesh/classes/Node.md) 对象
3. 读取单元连接信息并创建相应的单元对象

支持的单元类型包括：
- 点单元（PointElement）
- 线单元（LineElement）
- 三角形单元（TriElement）
- 四面体单元（TetraElement）

## 依赖关系

- [Mesh](../../mesh/classes/Mesh.md) - 网格数据结构
- [Node](../../mesh/classes/Node.md) - 节点数据结构
- [Element](../../mesh/classes/Element.md) - 单元数据结构
- STL - 文件流、字符串流等标准库组件