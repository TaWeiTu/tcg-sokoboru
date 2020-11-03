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
    std::cout << AStarSolver(Rows, Cols, Grid).solve().first << "\n";
    // I.emplace_back(Rows, Cols, std::move(Grid));
  }
  // int K = I.size();
  // std::vector<std::pair<int, std::string>> Answer(K);
  // size_t Iter = 0;
  // std::mutex Mutex;

  // auto Solve = [&I, &Answer, &Iter, &Mutex](size_t ID) {
  //   auto getIter = [&]() -> size_t {
  //     std::lock_guard<std::mutex> Guard(Mutex);
  //     if (Iter < I.size())
  //       return Iter++;
  //     return static_cast<size_t>(-1);
  //   };

  //   while (true) {
  //     size_t It = getIter();
  //     if (It == static_cast<size_t>(-1))
  //       break;
  //     std::cerr << "Thread " << ID << " gets job " << It << "\n";
  //     Answer[It] = AStarSolver(I[It].Rows, I[It].Cols, I[It].Grid).solve();
  //   }
  // };

  // std::thread T1(Solve, 0);
  // std::thread T2(Solve, 1);
  // T1.join();
  // T2.join();
  // for (int i = 0; i < K; ++i)
  //   std::cout << Answer[i].first << "\n" << Answer[i].second << "\n";
  return 0;
}
