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
#include <type_traits>
#include <vector>

enum Cell : uint8_t { BALL, EMPTY_BOX, WALL, FULL_BOX, EMPTY, PLAYER };
constexpr uint8_t InvalidCell = static_cast<uint8_t>(-1);

template <uint8_t Rows, uint8_t Cols> struct State {
  using MaskT = std::conditional_t<Rows * Cols <= 24, uint32_t, uint64_t>;
  std::array<MaskT, 2> Masks;
  static constexpr uint8_t StateSize = Rows * Cols;
  static constexpr MaskT StateMask = (MaskT(1) << StateSize) - 1;
  static constexpr MaskT PlayerMask = 255ULL << StateSize;

  State() = default;

  State(const std::array<Cell, StateSize> &Grid) : Masks{} {
    for (uint8_t I = 0; I != StateSize; ++I) {
      if (Grid[I] == PLAYER) {
        setPlayer(I);
      } else if (Grid[I] != EMPTY && Grid[I] != WALL) {
        if (Grid[I] != FULL_BOX) {
          Masks[Grid[I]] |= (MaskT(1) << I);
        } else {
          Masks[BALL] |= (MaskT(1) << I);
          Masks[EMPTY_BOX] |= (MaskT(1) << I);
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
    if constexpr (X == FULL_BOX)
      return (Masks[BALL] & Masks[EMPTY_BOX]) >> Pos & 1;
    else
      return (Masks[X] & ~Masks[X ^ 1]) >> Pos & 1;
  }

  uint8_t getPlayer() const {
    return static_cast<uint8_t>(Masks[0] >> StateSize & 255);
  }

  void setPlayer(uint8_t Pos) {
    Masks[0] &= StateMask;
    Masks[0] |= static_cast<MaskT>(Pos) << StateSize;
  }

  template <uint8_t X> void remove(uint8_t Pos) {
    assert(has<X>(Pos));
    Masks[X] &= ~(MaskT(1) << Pos);
  }

  template <uint8_t X> void insert(uint8_t Pos) {
    assert(!has<X>(Pos));
    Masks[X] |= (MaskT(1) << Pos);
  }

  template <uint8_t X> void move(uint8_t Old, uint8_t New) {
    remove<X>(Old);
    insert<X>(New);
  }

  static int CountTrailingZero(MaskT V) {
    if constexpr (std::is_same_v<uint32_t, MaskT>)
      return __builtin_ctz(V);
    else
      return __builtin_ctzll(V);
  }

  virtual int getHeuristic() const {
    int CurR = getPlayer() / Cols;
    int CurC = getPlayer() % Cols;
    int Dist = 0;
    for (MaskT V = ((Masks[0] ^ Masks[1]) & StateMask); V > 0;) {
      int Bit = CountTrailingZero(V & -V);
      int R = Bit / Cols, C = Bit % Cols;
      Dist += std::abs(CurR - R) + std::abs(CurC - C);
      V ^= (MaskT(1) << Bit);
    }
    return Dist;
  }

  bool isEmpty(uint8_t Pos, MaskT WallMask) const {
    return ((WallMask | Masks[BALL] | Masks[EMPTY_BOX]) >> Pos & 1) == 0;
  }

  std::pair<State, int>
  getNextState(uint8_t Dir,
               const std::array<std::array<uint8_t, 4>, StateSize> &Dest,
               MaskT WallMask) const {
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
    NextState.move<BALL>(Dst, Dest[Dst][Dir]);
    NextState.move<EMPTY_BOX>(Dst, Dest[Dst][Dir]);
    return std::make_pair(NextState, 2);
  }
};

namespace std {

template <uint8_t Rows, uint8_t Cols> struct hash<State<Rows, Cols>> {
  template <typename T> uint64_t getHash(const T &V) const {
    return std::hash<T>()(V);
  }

  uint64_t operator()(const State<Rows, Cols> &S) const {
    uint64_t Seed = 7122;
    Seed ^= getHash(S.Masks[0]) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
    Seed ^= getHash(S.Masks[1]) + 0x9e3779b9 + (Seed << 6) + (Seed >> 2);
    return Seed;
  }
};

} // namespace std

template <uint8_t Rows, uint8_t Cols>
inline void debug(std::ostream &OS, const State<Rows, Cols> &S,
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
