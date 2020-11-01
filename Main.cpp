#include <chrono>
#include <iostream>

#include "AStar.h"
#include "Utils.h"

template <int R, int C>
void solveCol(int Rows, int Cols, const std::vector<Cell> &Grid) {
  if (Rows == R && Cols == C)
    AStarSolver<R, C>(Grid).solve(std::cout);
  if constexpr (C + 1 <= 15 && R * (C + 1) <= 50)
    solveCol<R, C + 1>(Rows, Cols, Grid);
}

template <int R>
void solveRow(int Rows, int Cols, const std::vector<Cell> &Grid) {
  solveCol<R, 1>(Rows, Cols, Grid);
  if constexpr (R + 1 <= 15)
    solveRow<R + 1>(Rows, Cols, Grid);
}

int main() {
  while (true) {
    int Rows, Cols;
    if (!(std::cin >> Rows >> Cols))
      break;

    auto TimeStart = std::chrono::system_clock::now();
    auto Grid = readSokoboruGrid(Rows, Cols);
    solveRow<1>(Rows, Cols, Grid);
    auto TimeEnd = std::chrono::system_clock::now();
    std::cerr << "Time elapsed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(
                     TimeEnd - TimeStart)
                     .count()
              << "ms.\n";
  }
}
