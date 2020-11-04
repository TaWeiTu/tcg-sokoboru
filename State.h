#ifndef STATE_H
#define STATE_H

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <functional>
#include <iostream>
#include <numeric>
#include <ostream>
#include <vector>

enum Cell : uint8_t { BALL, EMPTY_BOX, WALL, FULL_BOX, EMPTY, PLAYER };
constexpr uint8_t InvalidCell = static_cast<uint8_t>(-1);

struct State {
  std::array<uint64_t, 2> Masks;
  static constexpr uint64_t StateMask = (1ULL << 50) - 1;
  static constexpr uint64_t PlayerMask = 255ULL << 50;

  State() = default;

  State(const std::vector<Cell> &Grid) : Masks{} {
    for (uint8_t I = 0, E = Grid.size(); I != E; ++I) {
      if (Grid[I] == PLAYER) {
        setPlayer(I);
      } else if (Grid[I] != EMPTY && Grid[I] != WALL) {
        if (Grid[I] != FULL_BOX) {
          Masks[Grid[I]] |= (1ULL << I);
        } else {
          Masks[BALL] |= (1ULL << I);
          Masks[EMPTY_BOX] |= (1ULL << I);
        }
      }
    }
  }

  bool operator==(const State &RHS) const { return Masks == RHS.Masks; }
  bool operator!=(const State &RHS) const { return Masks != RHS.Masks; }

  bool terminate() const {
    return ((Masks[BALL] ^ Masks[EMPTY_BOX]) & StateMask) == 0;
  }

  template <uint8_t X> bool has(uint8_t Pos) const {
    if constexpr (X == uint8_t(FULL_BOX))
      return (Masks[BALL] & Masks[EMPTY_BOX]) >> Pos & 1;
    else
      return (Masks[X] & ~Masks[X ^ 1]) >> Pos & 1;
  }

  uint8_t getPlayer() const {
    return static_cast<uint8_t>(Masks[0] >> 50 & 255);
  }

  void setPlayer(uint8_t Pos) {
    Masks[0] &= StateMask;
    Masks[0] |= static_cast<uint64_t>(Pos) << 50;
  }

  template <uint8_t X> void remove(uint8_t Pos) {
    assert(has<X>(Pos));
    if constexpr (X != FULL_BOX) {
      Masks[X] &= ~(1ULL << Pos);
    } else {
      Masks[BALL] &= ~(1ULL << Pos);
      Masks[EMPTY_BOX] &= ~(1ULL << Pos);
    }
  }

  template <uint8_t X> void insert(uint8_t Pos) {
    assert(!has<X>(Pos));
    if constexpr (X != FULL_BOX) {
      Masks[X] |= (1ULL << Pos);
    } else {
      Masks[BALL] |= (1ULL << Pos);
      Masks[EMPTY_BOX] |= (1ULL << Pos);
    }
  }

  template <uint8_t X> void move(uint8_t Old, uint8_t New) {
    remove<X>(Old);
    insert<X>(New);
  }
  int getHeuristic([[maybe_unused]] int Rows, [[maybe_unused]] int Cols) const {
    int CurR = getPlayer() / Cols;
    int CurC = getPlayer() % Cols;
    std::array<int, 30> Buffer;
    int Dist = 0, Ptr = 0;
    for (uint64_t V = ((Masks[0] ^ Masks[1]) & StateMask); V > 0;) {
      int Bit = __builtin_ctzll(V & -V);
      int R = Bit / Cols, C = Bit % Cols;
      Buffer[Ptr++] = std::abs(CurR - R) + std::abs(CurC - C);
      V ^= (1ULL << Bit);
    }
    std::nth_element(Buffer.begin(), Buffer.begin() + Ptr / 2, Buffer.begin() + Ptr);
    return std::accumulate(Buffer.begin(), Buffer.begin() + Ptr / 2, 0);
    return Dist;
  }

  bool isEmpty(uint8_t Pos, uint64_t WallMask) const {
    return ((WallMask | Masks[BALL] | Masks[EMPTY_BOX]) >> Pos & 1) == 0;
  }

  std::pair<State, int>
  getNextState(uint8_t Dir, const std::array<std::array<uint8_t, 4>, 50> &Dest,
               uint64_t WallMask) const {
    State NextState = *this;
    uint8_t Dst = Dest[getPlayer()][Dir];
    NextState.setPlayer(Dst);
    if (isEmpty(Dst, WallMask))
      return std::make_pair(NextState, 1);
    if (WallMask >> Dst & 1)
      return std::make_pair(NextState, -1);

    if (has<EMPTY_BOX>(Dst)) {
      if (Dest[Dst][Dir] == InvalidCell)
        return std::make_pair(NextState, -1);
      if (!has<BALL>(Dest[Dst][Dir]) && !isEmpty(Dest[Dst][Dir], WallMask))
        return std::make_pair(NextState, -1);
      NextState.move<EMPTY_BOX>(Dst, Dest[Dst][Dir]);
      return std::make_pair(NextState, 1);
    }

    if (has<BALL>(Dst)) {
      uint8_t Roll = Dst;
      NextState.remove<BALL>(Dst);
      while (Dest[Roll][Dir] != InvalidCell &&
             isEmpty(Dest[Roll][Dir], WallMask))
        Roll = Dest[Roll][Dir];

      if (Dest[Roll][Dir] != InvalidCell && has<EMPTY_BOX>(Dest[Roll][Dir]))
        Roll = Dest[Roll][Dir];
      if (Roll == Dst)
        return std::make_pair(NextState, -1);
      NextState.insert<BALL>(Roll);
      return std::make_pair(NextState, 1);
    }

    assert(has<FULL_BOX>(Dst));
    if (Dest[Dst][Dir] == InvalidCell || !isEmpty(Dest[Dst][Dir], WallMask))
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
    Seed ^= getHash(S.Masks[0]) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
    Seed ^= getHash(S.Masks[1]) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
    return Seed;
  }
};

} // namespace std

inline void debug(std::ostream &OS, const State &S, int Rows, int Cols,
                  const std::vector<Cell> &Grid) {
  for (int Pos = 0; Pos < Rows * Cols; ++Pos) {
    if (Pos == S.getPlayer())
      OS << '@';
    else if (S.has<BALL>(Pos))
      OS << 'O';
    else if (S.has<EMPTY_BOX>(Pos))
      OS << '$';
    else if (S.has<FULL_BOX>(Pos))
      OS << '*';
    else if (Grid[Pos] == WALL)
      OS << '#';
    else
      OS << '-';

    if ((Pos + 1) % Cols == 0)
      OS << "\n";
  }
}

#endif // STATE_H
