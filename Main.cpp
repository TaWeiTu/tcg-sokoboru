#include <chrono>
#include <iostream>

#include "Solver.h"
#include "Utils.h"

int main() {
  while (true) {
    int Rows, Cols;
    if (!(std::cin >> Rows >> Cols))
      break;

    auto TimeStart = std::chrono::system_clock::now();
    auto Grid = readSokoboruGrid(Rows, Cols);
    Solver *S = new AStarSolver(Rows, Cols, Grid);
    S->solve(std::cout);
    auto TimeEnd = std::chrono::system_clock::now();
    std::cerr << "Time elapsed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     TimeEnd - TimeStart)
                     .count()
              << "ms.\n";
  }
}
