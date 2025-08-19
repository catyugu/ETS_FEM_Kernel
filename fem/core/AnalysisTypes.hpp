#pragma once

namespace FEM {
    // 定义内核所支持的分析类型
    // 这将指导 FEValues 构建正确的 B 矩阵
    enum class AnalysisType {
        SCALAR_DIFFUSION,   // 标量扩散 (热、静电势)
        VECTOR_CURL,        // 矢量旋度 (磁场)
        // 未来可以添加 STRUCTURAL, FLUIDS, etc.
    };
}