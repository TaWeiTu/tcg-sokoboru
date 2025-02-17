#include "Solver.h"

IDDFSSolver::IDDFSSolver(int Rows, int Cols, const std::vector<Cell> &Grid)
    : Solver(Rows, Cols, Grid) {}

bool IDDFSSolver::IDDFS(const State &S, const State &Parent, int Depth,
                        int DepthLimit) {
  NumVisited++;
  if (S.terminate())
    return true;
  if (Depth >= DepthLimit)
    return false;

  uint8_t Pos = S.getPlayer();
  std::array<std::vector<State>, 2> DFSChild{};
  for (uint8_t Dir = 0; Dir < 4; ++Dir) {
    uint8_t Next = Dest[Pos][Dir];
    if (Next == InvalidCell)
      continue;
    auto [NextState, Cost] = S.getNextState(Dir, Dest, WallMask);
    if (NextState == Parent)
      continue;
    if (Cost < 0)
      continue;
    DFSChild[Cost - 1].push_back(std::move(NextState));
  }

  for (int Cost = 0; Cost < 2; ++Cost) {
    for (auto &X : DFSChild[Cost]) {
      if (VisState.find(X) != VisState.end() && VisState[X] <= Depth + 1 + Cost)
        continue;
      VisState[X] = Depth + 1 + Cost;
      if (IDDFS(X, S, Depth + 1 + Cost, DepthLimit))
        return true;
    }
  }
  return false;
}

std::pair<int, std::string> IDDFSSolver::solve() {
  auto Start = State(Grid);
  NumVisited = 0;
  for (int DepthLimit = 1;; DepthLimit += 3) {
    VisState.clear();
    if (IDDFS(Start, Start, 0, DepthLimit)) {
      std::cerr << "NumVisited = " << NumVisited << "\n";
      return std::make_pair(DepthLimit, "");
    }
  }
}
