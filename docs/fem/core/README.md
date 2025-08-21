# FEM::Core Namespace

核心有限元功能命名空间，包含有限元方法的基础组件。

## 概述

Core命名空间提供了有限元方法的核心功能组件，包括自由度管理、有限元值计算、线性求解器接口等。

## 类列表

### 基础类
- [DofManager](classes/DofManager.md) - 自由度管理器
- [FiniteElement](classes/FiniteElement.md) - 有限元基类
- [ReferenceElement](classes/ReferenceElement.md) - 参考单元
- [FEValues](classes/FEValues.md) - 有限元值计算
- [FEFaceValues](classes/FEFaceValues.md) - 面单元值计算
- [BoundaryCondition](classes/BoundaryCondition.md) - 边界条件基类

### 求解器相关
- [LinearSolver](classes/LinearSolver.md) - 线性求解器接口
- [Problem](classes/Problem.md) - 问题类

### 枚举类型
- AnalysisType - 分析类型枚举

## 设计原则

1. **模块化**：每个类都有明确的职责
2. **可扩展性**：支持多物理场耦合
3. **高效性**：利用Eigen库进行高效矩阵运算
4. **接口一致性**：提供统一的API接口

## 依赖关系

- Eigen 3.4.0：矩阵运算库
- Utils命名空间：工具类支持