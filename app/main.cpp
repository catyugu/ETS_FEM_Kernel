#include <io/Exporter.hpp>

#include "../fem/core/Problem.hpp"
#include "../fem/materials/Material.hpp"
#include "../fem/kernels/HeatDiffusionKernel.hpp"
#include "../fem/physics/HeatTransfer.hpp"
#include "../utils/SimpleLogger.hpp"

// 定义问题维度
constexpr int problem_dim = 1;

void run_1d_heat_conduction_test() {
    Utils::Logger::instance().info("--- Setting up 1D Heat Conduction Problem ---");

    // 1. 创建网格
    auto mesh = FEM::Mesh::create_uniform_1d_mesh(1.0, 10); // 长度 1m, 10个单元

    // 2. 定义材料
    FEM::Material copper("Copper");
    copper.setProperty("thermal_conductivity", 401.0);

    // 3. 创建物理场和 Kernels
    auto heat_physics = std::make_unique<FEM::HeatTransfer<problem_dim>>();
    heat_physics->addKernel(
        std::make_unique<FEM::HeatDiffusionKernel<problem_dim, 2>>(copper)
    );

    // 4. 创建 Problem (使用默认的SparseLU求解器)
    auto problem = std::make_unique<FEM::Problem<problem_dim>>(std::move(mesh), std::move(heat_physics));

    // 5. **登记**边界条件 (注意函数名变化)
    problem->addDirichletBC(0, 373.15);      // 左端点 100°C
    problem->addDirichletBC(10, 293.15);     // 右端点 20°C

    // 6. **组装**全局矩阵和向量
    problem->assemble();

    // 7. **施加**所有已登记的边界条件
    problem->applyBCs();

    // 8. **求解**线性系统
    problem->solve();

    // --- 新增：在这里显式调用 Exporter ---
    Utils::Logger::instance().info("Exporting results to output.vtk...");
    FEM::IO::Exporter::write_vtk("output.vtk", *problem);
    Utils::Logger::instance().info("Export complete.");
}

void run_1d_heat_conduction_test_cg() {
    Utils::Logger::instance().info("--- Setting up 1D Heat Conduction Problem with CG solver ---");

    // 1. 创建网格
    auto mesh = FEM::Mesh::create_uniform_1d_mesh(1.0, 10); // 长度 1m, 10个单元

    // 2. 定义材料
    FEM::Material copper("Copper");
    copper.setProperty("thermal_conductivity", 401.0);

    // 3. 创建物理场和 Kernels
    auto heat_physics = std::make_unique<FEM::HeatTransfer<problem_dim>>();
    heat_physics->addKernel(
        std::make_unique<FEM::HeatDiffusionKernel<problem_dim, 2>>(copper)
    );

    // 4. 创建 Problem (使用共轭梯度求解器)
    auto problem = std::make_unique<FEM::Problem<problem_dim>>(std::move(mesh), std::move(heat_physics), FEM::SolverType::ConjugateGradient);

    // 5. **登记**边界条件 (注意函数名变化)
    problem->addDirichletBC(0, 373.15);      // 左端点 100°C
    problem->addDirichletBC(10, 293.15);     // 右端点 20°C

    // 6. **组装**全局矩阵和向量
    problem->assemble();

    // 7. **施加**所有已登记的边界条件
    problem->applyBCs();

    // 8. **求解**线性系统
    problem->solve();

    // --- 新增：在这里显式调用 Exporter ---
    Utils::Logger::instance().info("Exporting results to output_cg.vtk...");
    FEM::IO::Exporter::write_vtk("output_cg.vtk", *problem);
    Utils::Logger::instance().info("Export complete.");
}

int main() {
    try {
        run_1d_heat_conduction_test();
        run_1d_heat_conduction_test_cg();
    } catch (const std::exception& e) {
        Utils::Logger::instance().error(e.what());
        return 1;
    }
    return 0;
}