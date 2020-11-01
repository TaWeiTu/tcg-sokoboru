#ifndef SOLVER_H
#define SOLVER_H

#include "State.h"
#include "Utils.h"
#include <ostream>
#include <unordered_set>

template <uint8_t Rows, uint8_t Cols> class Solver {
protected:
  std::array<std::array<uint8_t, 4>, Rows * Cols> Dest;
  typename State<Rows, Cols>::MaskT WallMask;
  State<Rows, Cols> StartState;

public:
  Solver(const std::vector<Cell> &Grid) : Dest{}, WallMask(0) {
    for (int R = 0; R < Rows; ++R) {
      for (int C = 0; C < Cols; ++C) {
        auto &Dst = Dest[R * Cols + C];
        std::fill(Dst.begin(), Dst.end(), InvalidCell);
        if (R > 0)
          Dst[UP] = (R - 1) * Cols + C;
        if (R + 1 < Rows)
          Dst[DOWN] = (R + 1) * Cols + C;
        if (C > 0)
          Dst[LEFT] = R * Cols + (C - 1);
        if (C + 1 < Cols)
          Dst[RIGHT] = R * Cols + (C + 1);
      }
    }
    for (unsigned P = 0, E = Rows * Cols; P != E; ++P) {
      if (Grid[P] == WALL)
        WallMask |= (1ULL << P);
    }
    std::array<Cell, Rows * Cols> StaticGrid;
    std::copy(Grid.begin(), Grid.end(), StaticGrid.begin());
    StartState = State<Rows, Cols>(StaticGrid);
  }

  virtual void solve(std::ostream &OS) = 0;
};

/*
class BFSSolver : public Solver {
  std::array<std::vector<State>, 3> Que;
  std::unordered_map<State, State> PrevState;
  std::unordered_map<State, uint8_t> MoveDirs;

  void backTrack(std::ostream &OS, State S, int Dist, uint8_t Dir);

public:
  BFSSolver(int Rows, int Cols, const std::vector<Cell> &Grid);
  void solve(std::ostream &OS) override;
};

class IDDFSSolver : public Solver {
  std::unordered_map<State, int> VisState;

  bool IDDFS(const State &S, const State &Parent, int Depth, int DepthLimit);

public:
  IDDFSSolver(int Rows, int Cols, const std::vector<Cell> &Grid);
  void solve(std::ostream &OS) override;
};

class IDAStarSolver : public Solver {
public:
  using ListState = std::tuple<int, int, uint8_t, State>;

private:
  struct Comparator {
    bool operator()(const ListState &X, const ListState &Y) const;
  };

  int DFS(State CurState, State PrevState, int Dist, int Cost, int Threshold);
  std::unordered_set<State> InPath;
  std::vector<State> Path;

  static constexpr int PathFound = -1;

public:
  IDAStarSolver(int Rows, int Cols, const std::vector<Cell> &Grid);
  void solve(std::ostream &OS) override;
}; */

#endif // SOLVER_H
