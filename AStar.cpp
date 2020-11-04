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

std::string AStarSolver::backtrace(State S) {
  std::string Path = "";
  while (true) {
    auto Iter = Cache.find(S);
    if (std::get<1>(Iter->second) == S)
      break;
    Path += MoveStr[std::get<2>(Iter->second)];
    S = std::get<1>(Iter->second);
  }
  std::reverse(Path.begin(), Path.end());
  return Path;
}

std::pair<int, std::string> AStarSolver::solve() {
  auto Start = State(Grid);
  std::priority_queue<ListState, std::vector<ListState>, Comparator> OpenList;
  OpenList.emplace(0, 0, Start);
  Cache[Start] = std::make_tuple(0, Start, Move(0));
  bool SkipPop = false;
  State ImmNext;
  int ImmHeuristic, ImmDist;
  while (!OpenList.empty()) {
    int Heuristic, Dist;
    State S;
    if (SkipPop) {
      SkipPop = false;
      Dist = ImmDist;
      Heuristic = ImmHeuristic;
      S = ImmNext;
    } else {
      std::tie(Heuristic, Dist, S) = OpenList.top();
      OpenList.pop();
    }
    if (S.terminate())
      return std::make_pair(Dist, backtrace(S));

    if (std::get<0>(Cache[S]) < Heuristic)
      continue;

    uint8_t Pos = S.getPlayer();
    for (uint8_t Dir = 0; Dir < 4; ++Dir) {
      uint8_t Next = Dest[Pos][Dir];
      if (Next == InvalidCell)
        continue;

      auto [NextState, Cost] = S.getNextState(Dir, Dest, WallMask);
      if (Cost < 0)
        continue;

      int NextDist = Dist + Cost;
      int H = NextState.getHeuristic(Rows, Cols);
      if (H < 0)
        continue;
      int NextHeuristic = NextDist + H;
      auto Iter = Cache.find(NextState);
      if (Iter == Cache.end() || std::get<0>(Iter->second) > NextHeuristic) {
        Cache[NextState] = std::make_tuple(NextHeuristic, S, Move(Dir));
        if (NextHeuristic > Heuristic || SkipPop) {
          OpenList.emplace(NextHeuristic, NextDist, NextState);
        } else {
          SkipPop = true;
          ImmNext = NextState;
          ImmHeuristic = NextHeuristic;
          ImmDist = NextDist;
        }
      }
    }
  }
  __builtin_unreachable();
}
