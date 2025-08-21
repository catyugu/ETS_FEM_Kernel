#include <io/Exporter.hpp>

#include "../fem/core/Problem.hpp"
#include "../fem/materials/Material.hpp"
#include "../fem/kernels/HeatDiffusionKernel.hpp"
#include "../fem/physics/HeatTransfer.hpp"
#include "../fem/bcs/DirichletBC.hpp" // 包含DirichletBC头文件
#include "../utils/SimpleLogger.hpp"

// 定义问题维度
constexpr int problem_dim = 1;

int main() {
    Utils::Logger::instance().info("--- Setting up 1D Heat Conduction Problem (Refactored) ---");

    // 1. 创建网格 (现在它内部已经命名了 "left" 和 "right" 边界)
    auto mesh = FEM::Mesh::create_uniform_1d_mesh(1.0, 10);

    // 2. 定义材料
    FEM::Material copper("Copper");
    copper.setProperty("thermal_conductivity", 401.0);

    // 3. 创建物理场
    auto heat_physics = std::make_unique<FEM::HeatTransfer<problem_dim>>();

    // 4. 将 Kernels 添加到物理场
    heat_physics->addKernel(
        std::make_unique<FEM::HeatDiffusionKernel<problem_dim, 2>>(copper)
    );

    // 5. 【新方式】创建边界条件对象，并添加到物理场
    heat_physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<problem_dim>>("left", 373.15) // 施加在名为 "left" 的边界上
    );
    heat_physics->addBoundaryCondition(
        std::make_unique<FEM::DirichletBC<problem_dim>>("right", 293.15) // 施加在名为 "right" 的边界上
    );

    // 6. 创建 Problem
    auto problem = std::make_unique<FEM::Problem<problem_dim>>(std::move(mesh), std::move(heat_physics));

    // 7. 组装
    problem->assemble();

    // 8. 求解 (内部会自动处理所有已登记的边界条件)
    problem->solve();

    // 9. 导出结果
    Utils::Logger::instance().info("Exporting results to output.vtk...");
    FEM::IO::Exporter::write_vtk("output.vtk", *problem);
    Utils::Logger::instance().info("Export complete.");
}