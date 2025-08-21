#pragma once

#include <string>
#include "../core/Problem.hpp"
#include <fstream>
#include <iostream>
#include <complex>

namespace FEM {
    namespace IO {
        class Exporter {
        public:
            template<int TDim, typename TScalar>
            static void write_vtk(const std::string& filename, const Problem<TDim, TScalar>& problem);
            
        private:
            template<int TDim, typename TScalar>
            static void write_mesh_data(std::ofstream& file, const Problem<TDim, TScalar>& problem);
            
            template<int TDim, typename TScalar>
            static void write_solution_data(std::ofstream& file, const Problem<TDim, TScalar>& problem);
        };
        
        // 为常用的Problem类型提供显式实例化声明
        extern template void Exporter::write_vtk<1, double>(const std::string&, const Problem<1, double>&);
        extern template void Exporter::write_vtk<2, double>(const std::string&, const Problem<2, double>&);
        extern template void Exporter::write_vtk<3, double>(const std::string&, const Problem<3, double>&);
        extern template void Exporter::write_vtk<1, std::complex<double>>(const std::string&, const Problem<1, std::complex<double>>&);
        extern template void Exporter::write_vtk<2, std::complex<double>>(const std::string&, const Problem<2, std::complex<double>>&);
        extern template void Exporter::write_vtk<3, std::complex<double>>(const std::string&, const Problem<3, std::complex<double>>&);
    }
}