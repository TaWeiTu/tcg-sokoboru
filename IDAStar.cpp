#include "Solver.h"
#include <algorithm>
#include <climits>
#include <queue>

IDAStarSolver::IDAStarSolver(int Rows, int Cols, const std::vector<Cell> &Grid)
    : Solver(Rows, Cols, Grid) {}

bool IDAStarSolver::Comparator::operator()(const ListState &X,
                                           const ListState &Y) const {
  return std::get<0>(X) == std::get<0>(Y) ? std::get<1>(X) > std::get<1>(Y)
                                          : std::get<0>(X) > std::get<0>(Y);
}

int IDAStarSolver::DFS(State CurState, State PrevState, int Dist, int Cost,
                       int Threshold) {
  if (Cost > Threshold)
    return Cost;
  if (CurState.terminate())
    return PathFound;

  uint8_t Pos = CurState.getPlayer();
  int Result = std::numeric_limits<int>::max();
  for (uint8_t Dir = 0; Dir < 4; ++Dir) {
    uint8_t Next = Dest[Pos][Dir];
    if (Next == InvalidCell)
      continue;

    auto [NextState, Cost] = CurState.getNextState(Dir, Dest, WallMask);
    if (Cost < 0 || NextState == PrevState ||
        InPath.find(NextState) != InPath.end())
      continue;

    int NextDist = Dist + Cost;
    int H = NextState.getHeuristic(Rows, Cols);
    if (H < 0)
      continue;
    int NextHeuristic = NextDist + H;
    Path.push_back(NextState);
    InPath.insert(NextState);
    int V = DFS(NextState, CurState, NextDist, NextHeuristic, Threshold);
    if (V == PathFound)
      return PathFound;
    Path.pop_back();
    InPath.erase(NextState);
    Result = std::min(Result, V);
  }
  return Result;
}

void IDAStarSolver::solve(std::ostream &OS) {
  int Threshold = 10;
  auto Start = State(Grid);
  Path.push_back(Start);
  InPath.insert(Start);
  while (true) {
    std::cerr << "Threshold = " << Threshold << "\n";
    int V = DFS(Start, Start, 0, 0, Threshold);
    if (V == PathFound) {
      std::cerr << "Found Dist = " << Threshold << "\n";
      return;
    }
    Threshold = V;
  }
}
