#include "cg.hh"
#include <chrono>
#include <iostream>
#include <cuda_runtime.h>

using clk = std::chrono::high_resolution_clock;
using second = std::chrono::duration<double>;
using time_point = std::chrono::time_point<clk>;

/*
Implementation of a simple CG solver using matrix in the mtx format (Matrix
market) Any matrix in that format can be used to test the code
*/
int main(int argc, char ** argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " [martix-market-filename]"
              << " [CUDA block size]" << std::endl;
    return 1;
  }

  CGSolverSparse sparse_solver;
  sparse_solver.read_matrix(argv[1]);
  
  dim3 block_size;
  block_size.x = std::stoi(argv[2]);
  dim3 grid_size;
  grid_size.x = sparse_solver.nz() / block_size.x + (sparse_solver.nz() % block_size.x != 0);

  int n = sparse_solver.n();
  int m = sparse_solver.m();
  double h = 1. / n;

  sparse_solver.init_source_term(h);

  std::vector<double> x_s(n);
  std::fill(x_s.begin(), x_s.end(), 0.);

  std::cout << "Call CG sparse on matrix size " << m << " x " << n << ")"
            << std::endl;
  auto t1 = clk::now();
  sparse_solver.solve(x_s,grid_size,block_size);
  second elapsed = clk::now() - t1;
  std::cout << "Time for CG (sparse solver)  = " << elapsed.count() << " [s]\n";

  return 0;
}
