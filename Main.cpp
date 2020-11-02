#include <chrono>
#include <iostream>
#include <utility>

#include "Solver.h"
#include "Utils.h"

struct Instance {
  int Rows, Cols;
  std::vector<Cell> Grid;

  Instance(int R, int C, std::vector<Cell> G)
      : Rows(R), Cols(C), Grid(std::move(G)) {}
};

int main() {
  std::vector<Instance> I;
  while (true) {
    int Rows, Cols;
    if (!(std::cin >> Rows >> Cols))
      break;

    auto Grid = readSokoboruGrid(Rows, Cols);
    I.emplace_back(Rows, Cols, std::move(Grid));
  }
  int K = I.size();
  std::vector<std::pair<int, std::string>> Answer(K);

  auto Solve = [&I, &Answer](int Beg, int End) {
    while (Beg < End) {
      auto Result = AStarSolver(I[Beg].Rows, I[Beg].Cols, I[Beg].Grid).solve();
      Answer[Beg] = Result;
      Beg++;
    }
  };

  std::thread T1(Solve, 0, K / 2);
  std::thread T2(Solve, K / 2, K);
  T1.join();
  T2.join();
  for (int i = 0; i < K; ++i)
    std::cout << Answer[i].first << "\n" << Answer[i].second << "\n";
  return 0;
}
