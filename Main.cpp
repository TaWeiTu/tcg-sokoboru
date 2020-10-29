#include <iostream>

#include "Solver.h"
#include "Utils.h"

int main() {
  while (true) {
    int Rows, Cols;
    if (!(std::cin >> Rows >> Cols))
      break;

    auto Grid = readSokoboruGrid(Rows, Cols);
    Solver *S = new AStarSolver(Rows, Cols, Grid);
    S->solve(std::cout);
  }
}
