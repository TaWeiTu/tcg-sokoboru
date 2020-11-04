#include "Solver.h"

IDAStarSolver::IDAStarSolver(int Rows, int Cols, const std::vector<Cell> &Grid)
    : Solver(Rows, Cols, Grid) {}

int IDAStarSolver::DFS(State CurState, State PrevState, int Threshold, int Est, int Dist) {
  NumVisited++;
  if (Est > Threshold)
    return Est;
  if (CurState.terminate())
    return -1;

  int MinCut = 1'000'000;
  uint8_t Pos = CurState.getPlayer();
  for (uint8_t Dir = 0; Dir < 4; ++Dir) {
    uint8_t Next = Dest[Pos][Dir];
    if (Next == InvalidCell)
      continue;

    auto [NextState, Cost] = CurState.getNextState(Dir, Dest, WallMask);
    if (Cost < 0 || NextState == PrevState)
      continue;

    int NextDist = Dist + Cost;
    int H = NextState.getHeuristic(Rows, Cols);
    if (H < 0)
      continue;
    int NextHeuristic = NextDist + H;
    auto Iter = Cache.find(NextState);
    if (Iter == Cache.end() || Iter->second > NextHeuristic) {
      Cache[NextState] = NextHeuristic;
      int Cut = DFS(NextState, CurState, Threshold, NextHeuristic, NextDist);
      if (Cut == -1) return -1;
      MinCut = std::min(MinCut, Cut);
    }
  }
  return MinCut;
}

std::pair<int, std::string> IDAStarSolver::solve() {
  int Threshold = 10;
  const State Start(Grid);
  const int H = Start.getHeuristic(Rows, Cols);
  NumVisited = 0;
  while (true) {
    Cache.clear();
    int Cut = DFS(Start, Start, Threshold, H, 0);
    if (Cut == -1) {
      std::cerr << "NumVisited = " << NumVisited << "\n";
      return std::make_pair(Threshold, "");
    }
    Threshold = Cut;
  }
}
