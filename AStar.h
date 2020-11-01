#ifndef ASTAR_H
#define ASTAR_H

#include "Solver.h"
#include <queue>

template <uint8_t Rows, uint8_t Cols>
class AStarSolver : public Solver<Rows, Cols> {
public:
  using StateT = State<Rows, Cols>;
  using ListState = std::tuple<int, int, StateT>;

private:
  struct Comparator {
    bool operator()(const ListState &X, const ListState &Y) const;
  };

  static constexpr size_t BucketCount = 1'000'000;
  std::unordered_map<StateT, std::tuple<int, StateT, Move>> Cache;

  using Solver<Rows, Cols>::Dest;
  using Solver<Rows, Cols>::WallMask;
  using Solver<Rows, Cols>::StartState;

  void backtrace(StateT S, int Dist, std::ostream &OS);

public:
  AStarSolver(const std::vector<Cell> &Grid);
  void solve(std::ostream &OS) override;
};

template <uint8_t Rows, uint8_t Cols>
AStarSolver<Rows, Cols>::AStarSolver(const std::vector<Cell> &Grid)
    : Solver<Rows, Cols>(Grid), Cache(BucketCount) {}

template <uint8_t Rows, uint8_t Cols>
bool AStarSolver<Rows, Cols>::Comparator::operator()(const ListState &X,
                                                     const ListState &Y) const {
  return std::get<0>(X) == std::get<0>(Y) ? std::get<1>(X) > std::get<1>(Y)
                                          : std::get<0>(X) > std::get<0>(Y);
}

template <uint8_t Rows, uint8_t Cols>
void AStarSolver<Rows, Cols>::backtrace(StateT S, int Dist, std::ostream &OS) {
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

template <uint8_t Rows, uint8_t Cols>
void AStarSolver<Rows, Cols>::solve(std::ostream &OS) {
  std::priority_queue<ListState, std::vector<ListState>, Comparator> OpenList;
  OpenList.emplace(0, 0, StartState);
  Cache[StartState] = std::make_tuple(0, StartState, Move(0));
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
      int H = NextState.getHeuristic();
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

#endif // ASTAR_H
