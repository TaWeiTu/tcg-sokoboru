#include "Solver.h"
#include <queue>
#include <tuple>

AStarSolver::AStarSolver(int Rows, int Cols, const std::vector<Cell> &Grid)
    : Solver(Rows, Cols, Grid) {}

bool AStarSolver::Comparator::operator()(const ListState &X,
                                         const ListState &Y) const {
  return std::get<0>(X) == std::get<0>(Y) ? std::get<1>(X) > std::get<1>(Y)
                                          : std::get<0>(X) > std::get<0>(Y);
}

void AStarSolver::solve(std::ostream &OS) {
  auto Start = State(Grid);
  std::priority_queue<ListState, std::vector<ListState>, Comparator> OpenList;
  OpenList.emplace(0, 0, Start);
  while (!OpenList.empty()) {
    auto [Heuristic, Dist, S] = OpenList.top();
    OpenList.pop();
    if (S.terminate()) {
      OS << "Found Dist = " << Dist << "\n";
      return;
    }

    if (Cache[S] < Heuristic)
      continue;

    uint8_t Pos = S.Player;
    for (uint8_t Dir = 0; Dir < 4; ++Dir) {
      uint8_t Next = Dest[Pos][Dir];
      if (Next == InvalidCell)
        continue;

      auto [NextState, Cost] = S.getNextState(Dir, Dest);
      if (Cost < 0)
        continue;

      int NextDist = Dist + Cost;
      int NextHeuristic = NextDist + NextState.getHeuristic(Rows, Cols);
      auto Iter = Cache.find(NextState);
      if (Iter == Cache.end() || Iter->second > NextHeuristic) {
        Cache[NextState] = NextHeuristic;
        OpenList.emplace(NextHeuristic, NextDist, NextState);
      }
    }
  }
}
