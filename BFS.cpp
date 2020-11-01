#include "Solver.h"
#include <algorithm>
#include <string>

BFSSolver::BFSSolver(int Rows, int Cols, const std::vector<Cell> &Grid)
    : Solver(Rows, Cols, Grid) {}

void BFSSolver::solve(std::ostream &OS) {
  auto Start = State(Grid);
  PrevState[Start] = Start;
  Que[0].emplace_back(Start);
  int Dist = 0;
  while (true) {
    auto &Q = Que[Dist % 3];
    for (auto &State : Q) {
      uint8_t Pos = State.Player;
#ifndef NDEBUG
      std::cerr << "Dist = " << Dist << "\n";
      debug(std::cerr, State, Rows, Cols);
#endif
      for (uint8_t Dir = 0; Dir < 4; ++Dir) {
        uint8_t Next = Dest[Pos][Dir];
        if (Next == InvalidCell)
          continue;
        auto [NextState, Cost] = State.getNextState(Dir, Dest, Grid);
        if (Cost < 0)
          continue;

        if (NextState.terminate()) {
          backTrack(OS, State, Dist + 1, Dir);
          return;
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

void BFSSolver::backTrack(std::ostream &OS, State S, int Dist, uint8_t Dir) {
  OS << Dist << "\n";
  std::string Result(1, MoveStr[Dir]);
  while (true) {
    if (MoveDirs.find(S) == MoveDirs.end()) break;
    Result += MoveStr[MoveDirs[S]];
    S = PrevState[S];
  }
  std::reverse(Result.begin(), Result.end());
  OS << Result << "\n";
}
