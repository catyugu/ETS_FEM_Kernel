# fem::io 命名空间

fem::io 命名空间负责处理有限元模型的输入输出操作，包括从文件导入网格数据和将计算结果导出到文件。

## 类列表

- [Importer](classes/Importer.md) - 数据导入器
- [Exporter](classes/Exporter.md) - 数据导出器

## 概述

fem::io 命名空间提供了有限元数据的输入输出功能。它允许从外部文件读取网格和边界条件数据，并将计算结果写入文件以供后处理和可视化。

## 依赖关系

- fem::mesh - 网格数据结构
- fem::core - 核心数据结构