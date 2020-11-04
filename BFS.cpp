#include "Solver.h"
#include <algorithm>
#include <string>

BFSSolver::BFSSolver(int Rows, int Cols, const std::vector<Cell> &Grid)
    : Solver(Rows, Cols, Grid) {}

std::pair<int, std::string> BFSSolver::solve() {
  auto Start = State(Grid);
  PrevState[Start] = Start;
  Que[0].emplace_back(Start);
  int Dist = 0;
  size_t NumVisited = 0;
  while (true) {
    auto &Q = Que[Dist % 3];
    for (auto &State : Q) {
      uint8_t Pos = State.getPlayer();
      NumVisited++;
      for (uint8_t Dir = 0; Dir < 4; ++Dir) {
        uint8_t Next = Dest[Pos][Dir];
        if (Next == InvalidCell)
          continue;
        auto [NextState, Cost] = State.getNextState(Dir, Dest, WallMask);
        if (Cost < 0)
          continue;

        if (NextState.terminate()) {
          std::cerr << "NumVisited = " << NumVisited << "\n";
          return std::make_pair(Dist + 1, backtrace(State, Dir));
        }

        if (PrevState.find(NextState) != PrevState.end())
          continue;

        PrevState[NextState] = State;
        MoveDirs[NextState] = Dir;
        Que[(Dist + Cost) % 3].push_back(std::move(NextState));
      }
    }
    Q.clear();
    Dist++;
  }
}

std::string BFSSolver::backtrace(State S, uint8_t Dir) {
  std::string Result(1, MoveStr[Dir]);
  while (true) {
    if (MoveDirs.find(S) == MoveDirs.end())
      break;
    Result += MoveStr[MoveDirs[S]];
    S = PrevState[S];
  }
  std::reverse(Result.begin(), Result.end());
  return Result;
}
