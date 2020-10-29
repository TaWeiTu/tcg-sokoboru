#ifndef STATE_H
#define STATE_H

#include <algorithm>
#include <array>
#include <cassert>
#include <functional>
#include <ostream>
#include <vector>

enum Cell : uint8_t { BALL, EMPTY_BOX, FULL_BOX, WALL, EMPTY, PLAYER };
constexpr uint8_t InvalidCell = static_cast<uint8_t>(-1);

struct State {
  uint8_t Player;
  std::array<uint64_t, 4> Masks;
  uint64_t EmptyBoxes, FullBoxes, Balls, Others;

  State() = default;

  State(const std::vector<Cell> &Grid) : Masks{} {
    for (uint8_t I = 0, E = Grid.size(); I != E; ++I) {
      if (Grid[I] == Cell::PLAYER)
        Player = I;
      else if (Grid[I] != Cell::EMPTY)
        Masks[Grid[I]] |= (1ULL << I);
    }
  }

  bool operator==(const State &RHS) const {
    return Player == RHS.Player && Masks == RHS.Masks;
  }

  bool terminate() const { return Masks[BALL] == 0; }

  template <uint8_t X> bool has(uint8_t Pos) const {
    return Masks[X] >> Pos & 1;
  }

  void setPlayer(uint8_t Pos) { Player = Pos; }

  template <uint8_t X> void remove(uint8_t Pos) {
    assert(has<X>(Pos));
    Masks[X] &= ~(1ULL << Pos);
  }

  template <uint8_t X> void insert(uint8_t Pos) {
    assert(!has<X>(Pos));
    Masks[X] |= (1ULL << Pos);
  }

  template <uint8_t X> void move(uint8_t Old, uint8_t New) {
    remove<X>(Old);
    insert<X>(New);
  }

  virtual int getHeuristic(int Rows, int Cols) const {
    uint8_t Dist[2] = {0, 0};
    uint8_t CurR = Player / Cols;
    uint8_t CurC = Player % Cols;
    for (uint8_t R = 0; R < Rows; ++R) {
      for (uint8_t C = 0; C < Cols; ++C) {
        uint8_t P = R * Cols + C;
        uint8_t D = std::abs(CurR - R) + std::abs(CurC - C);
        if (has<BALL>(P))
          Dist[0] = std::max(Dist[0], D);
        if (has<EMPTY_BOX>(P))
          Dist[1] = std::max(Dist[1], D);
      }
    }
    return std::min(Dist[0], Dist[1]) * __builtin_popcountll(Masks[BALL]);
  }

  bool isEmpty(uint8_t Pos) const {
    return ((Masks[BALL] | Masks[EMPTY_BOX] | Masks[WALL] | Masks[FULL_BOX]) >>
                Pos &
            1) == 0;
  }

  enum EmptyOr : uint8_t { EMPTY, FOUND, NOT_FOUND };
  template <uint8_t X> EmptyOr isEmptyOr(uint8_t Pos) const {
    return isEmpty(Pos) ? EmptyOr::EMPTY
                        : has<X>(Pos) ? EmptyOr::FOUND : EmptyOr::NOT_FOUND;
  }

  std::pair<State, int>
  getNextState(uint8_t Dir,
               const std::vector<std::array<uint8_t, 4>> &Dest) const {
    State NextState = *this;
    uint8_t Dst = Dest[Player][Dir];
    NextState.setPlayer(Dst);
    if (isEmpty(Dst))
      return std::make_pair(NextState, 1);
    if (has<WALL>(Dst))
      return std::make_pair(NextState, -1);

    if (has<EMPTY_BOX>(Dst)) {
      if (Dest[Dst][Dir] == InvalidCell)
        return std::make_pair(NextState, -1);
      EmptyOr Status = isEmptyOr<BALL>(Dest[Dst][Dir]);
      if (Status == EmptyOr::NOT_FOUND)
        return std::make_pair(NextState, -1);
      if (Status == EmptyOr::EMPTY) {
        NextState.move<EMPTY_BOX>(Dst, Dest[Dst][Dir]);
      } else {
        NextState.remove<EMPTY_BOX>(Dst);
        NextState.remove<BALL>(Dest[Dst][Dir]);
        NextState.insert<FULL_BOX>(Dest[Dst][Dir]);
      }
      return std::make_pair(NextState, 1);
    }

    if (has<BALL>(Dst)) {
      uint8_t Roll = Dst;
      NextState.remove<BALL>(Dst);
      while (Dest[Roll][Dir] != InvalidCell && isEmpty(Dest[Roll][Dir]))
        Roll = Dest[Roll][Dir];

      if (Dest[Roll][Dir] != InvalidCell && has<EMPTY_BOX>(Dest[Roll][Dir])) {
        Roll = Dest[Roll][Dir];
        NextState.remove<EMPTY_BOX>(Roll);
        NextState.insert<FULL_BOX>(Roll);
      } else {
        if (Roll == Dst)
          return std::make_pair(NextState, -1);
        NextState.insert<BALL>(Roll);
      }
      return std::make_pair(NextState, 1);
    }

    assert(has<FULL_BOX>(Dst));
    if (Dest[Dst][Dir] == InvalidCell || !isEmpty(Dest[Dst][Dir]))
      return std::make_pair(NextState, -1);
    NextState.move<FULL_BOX>(Dst, Dest[Dst][Dir]);
    return std::make_pair(NextState, 2);
  }
};

namespace std {

template <> struct hash<State> {
  template <typename T> uint64_t getHash(const T &V) const {
    return std::hash<T>()(V);
  }

  uint64_t operator()(const State &S) const {
    uint64_t Seed = 7122;
    Seed ^= getHash(S.Player) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
    Seed ^= getHash(S.Masks[0]) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
    Seed ^= getHash(S.Masks[1]) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
    Seed ^= getHash(S.Masks[2]) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
    return Seed;
  }
};

} // namespace std

inline void debug(std::ostream &OS, const State &S, int Rows, int Cols) {
  for (int Pos = 0; Pos < Rows * Cols; ++Pos) {
    if (Pos == S.Player)
      OS << '@';
    else if (S.has<BALL>(Pos))
      OS << 'O';
    else if (S.has<EMPTY_BOX>(Pos))
      OS << '$';
    else if (S.has<FULL_BOX>(Pos))
      OS << '*';
    else if (S.has<WALL>(Pos))
      OS << '#';
    else
      OS << '-';

    if ((Pos + 1) % Cols == 0)
      OS << "\n";
  }
}

#endif // STATE_H
