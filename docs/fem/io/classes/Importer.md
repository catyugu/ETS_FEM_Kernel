# Importer 类

## 描述

`Importer` 类负责从外部文件导入网格数据。目前支持从 COMSOL 软件导出的 .mphtxt 格式文件中读取网格信息。该类提供静态方法，用于将网格数据读取到 [Mesh](file:///E:/code/cpp/ETS_FEM_Kernel/fem/mesh/Mesh.hpp#L17-L45) 对象中，以便在有限元分析中使用。

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
- 指向新创建的 [Mesh](file:///E:/code/cpp/ETS_FEM_Kernel/fem/mesh/Mesh.hpp#L17-L45) 对象的 unique_ptr，失败则为空指针

**异常:**
- `std::runtime_error` - 当无法打开文件时抛出

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
} catch (const std::runtime_error& e) {
    // 处理文件打开错误
    std::cerr << "Error: " << e.what() << std::endl;
}
```

## 实现细节

`Importer` 类目前专门用于读取 COMSOL 软件导出的 .mphtxt 格式文件。该格式是一种文本格式，包含网格的节点坐标和单元连接信息。

读取过程包括：
1. 解析文件中的节点数量和单元数量
2. 读取节点坐标数据并创建 [Node](file:///E:/code/cpp/ETS_FEM_Kernel/fem/mesh/Node.hpp#L11-L26) 对象
3. 读取单元连接信息并创建 [TriElement](file:///E:/code/cpp/ETS_FEM_Kernel/fem/mesh/Element.hpp#L71-L77) 对象

目前仅支持二维三角形单元（TriElement）。

## 依赖关系

- [Mesh](file:///E:/code/cpp/ETS_FEM_Kernel/fem/mesh/Mesh.hpp#L17-L45) - 网格数据结构
- [Node](file:///E:/code/cpp/ETS_FEM_Kernel/fem/mesh/Node.hpp#L11-L26) - 节点数据结构
- [TriElement](file:///E:/code/cpp/ETS_FEM_Kernel/fem/mesh/Element.hpp#L71-L77) - 三角形单元数据结构
- STL - 文件流、字符串流等标准库组件