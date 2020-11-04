#ifndef SOLVER_H
#define SOLVER_H

#include "HashMap.h"
#include "State.h"
#include "Utils.h"
#include <ostream>
#include <unordered_set>

#include "absl/container/flat_hash_map.h"

class Solver {
protected:
  int Rows, Cols;
  const std::vector<Cell> &Grid;
  std::array<std::array<uint8_t, 4>, 50> Dest;
  uint64_t WallMask;

public:
  Solver(int Rows, int Cols, const std::vector<Cell> &Grid)
      : Rows(Rows), Cols(Cols), Grid(Grid), WallMask(0) {
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
    for (unsigned P = 0, E = Grid.size(); P != E; ++P) {
      if (Grid[P] == WALL)
        WallMask |= (1ULL << P);
    }
  }

  virtual ~Solver() = default;
  virtual std::pair<int, std::string> solve() = 0;
};

class BFSSolver : public Solver {
  std::array<std::vector<State>, 3> Que;
  absl::flat_hash_map<State, State> PrevState;
  absl::flat_hash_map<State, uint8_t> MoveDirs;

  std::string backtrace(State S, uint8_t Dir);

public:
  BFSSolver(int Rows, int Cols, const std::vector<Cell> &Grid);
  std::pair<int, std::string> solve() override;
};

class IDDFSSolver : public Solver {
  absl::flat_hash_map<State, int> VisState;
  size_t NumVisited;

  bool IDDFS(const State &S, const State &Parent, int Depth, int DepthLimit);

public:
  IDDFSSolver(int Rows, int Cols, const std::vector<Cell> &Grid);
  std::pair<int, std::string> solve() override;
};

class AStarSolver : public Solver {
public:
  using ListState = std::tuple<int, int, State>;

private:
  struct Comparator {
    bool operator()(const ListState &X, const ListState &Y) const;
  };

  static constexpr size_t BucketCount = 1'000'000;
  // std::unordered_map<State, std::tuple<int, State, Move>> Cache;
  absl::flat_hash_map<State, std::tuple<int, State, Move>> Cache;
  // HashMap<State, std::tuple<int, State, Move>> Cache;

  std::string backtrace(State S);

public:
  AStarSolver(int Rows, int Cols, const std::vector<Cell> &Grid);
  virtual ~AStarSolver() = default;
  std::pair<int, std::string> solve() override;
};

class IDAStarSolver : public Solver {
  int DFS(State CurState, State PrevState, int Threshold, int Est, int Dist);
  absl::flat_hash_map<State, int> Cache;
  size_t NumVisited;

public:
  IDAStarSolver(int Rows, int Cols, const std::vector<Cell> &Grid);
  virtual ~IDAStarSolver() = default;
  std::pair<int, std::string> solve() override;
};

#endif // SOLVER_H
