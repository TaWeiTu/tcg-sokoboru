#include <chrono>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
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
  size_t Iter = 0;
  std::mutex Mutex;

  auto Solve = [&I, &Answer, &Iter, &Mutex]() {
    auto getIter = [&]() -> size_t {
      std::lock_guard<std::mutex> Guard(Mutex);
      if (Iter < I.size())
        return Iter++;
      return static_cast<size_t>(-1);
    };

    while (true) {
      size_t It = getIter();
      if (It == static_cast<size_t>(-1))
        break;
      std::unique_ptr<Solver> S =
          std::make_unique<BFSSolver>(I[It].Rows, I[It].Cols, I[It].Grid);
      Answer[It] = S->solve();
    }
  };

  std::thread T1(Solve);
  std::thread T2(Solve);
  T1.join();
  T2.join();
  for (int i = 0; i < K; ++i)
    std::cout << Answer[i].first << "\n" << Answer[i].second << "\n";
  return 0;
}
