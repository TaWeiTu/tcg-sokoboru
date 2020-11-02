#include "Solver.h"
#include <queue>
#include <tuple>

AStarSolver::AStarSolver(int Rows, int Cols, const std::vector<Cell> &Grid)
    : Solver(Rows, Cols, Grid), Cache(BucketCount) {}

bool AStarSolver::Comparator::operator()(const ListState &X,
                                         const ListState &Y) const {
  return std::get<0>(X) == std::get<0>(Y) ? std::get<1>(X) > std::get<1>(Y)
                                          : std::get<0>(X) > std::get<0>(Y);
}

void AStarSolver::backtrace(State S, int Dist, std::ostream &OS) {
  OS << Dist << "\n";
  std::string Path = "";
  while (true) {
    auto Iter = Cache.find(S);
    assert(Iter != Cache.end());
    if (std::get<1>(Iter->second) == S)
      break;
    Path += MoveStr[std::get<2>(Iter->second)];
    S = std::get<1>(Iter->second);
  }
  std::reverse(Path.begin(), Path.end());
  OS << Path << "\n";
}

void AStarSolver::solve(std::ostream &OS) {
  auto Start = State(Grid);
  std::priority_queue<ListState, std::vector<ListState>, Comparator> OpenList;
  OpenList.emplace(0, 0, Start);
  Cache[Start] = std::make_tuple(0, Start, Move(0));
  uint64_t WallMask = 0;
  for (unsigned P = 0, E = Grid.size(); P != E; ++P) {
    if (Grid[P] == WALL)
      WallMask |= (1ULL << P);
  }
  while (!OpenList.empty()) {
    auto [Heuristic, Dist, S] = OpenList.top();
    OpenList.pop();
    if (S.terminate()) {
      backtrace(S, Dist, OS);
      return;
    }

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
        OpenList.emplace(NextHeuristic, NextDist, NextState);
      }
    }
  }
}
