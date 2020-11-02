#ifndef SOLVER_H
#define SOLVER_H

#include "State.h"
#include "Utils.h"
#include <bits/extc++.h>
#include <ostream>
#include <unordered_set>

class Solver {
protected:
  int Rows, Cols;
  const std::vector<Cell> &Grid;
  std::vector<std::array<uint8_t, 4>> Dest;

public:
  Solver(int Rows, int Cols, const std::vector<Cell> &Grid)
      : Rows(Rows), Cols(Cols), Grid(Grid), Dest(Rows * Cols) {
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
  }

  virtual std::pair<int, std::string> solve() = 0;
};

// class BFSSolver : public Solver {
//   std::array<std::vector<State>, 3> Que;
//   std::unordered_map<State, State> PrevState;
//   std::unordered_map<State, uint8_t> MoveDirs;
//
//   void backTrack(std::ostream &OS, State S, int Dist, uint8_t Dir);
//
// public:
//   BFSSolver(int Rows, int Cols, const std::vector<Cell> &Grid);
//   void solve(std::ostream &OS) override;
// };
//
// class IDDFSSolver : public Solver {
//   std::unordered_map<State, int> VisState;
//
//   bool IDDFS(const State &S, const State &Parent, int Depth, int DepthLimit);
//
// public:
//   IDDFSSolver(int Rows, int Cols, const std::vector<Cell> &Grid);
//   void solve(std::ostream &OS) override;
// };

class AStarSolver : public Solver {
public:
  using ListState = std::tuple<int, int, State>;

private:
  struct Comparator {
    bool operator()(const ListState &X, const ListState &Y) const;
  };

  static constexpr size_t BucketCount = 1'000'000;
  std::unordered_map<State, std::tuple<int, State, Move>> Cache;

  std::string backtrace(State S);

public:
  AStarSolver(int Rows, int Cols, const std::vector<Cell> &Grid);
  std::pair<int, std::string> solve() override;
};

#endif // SOLVER_H
