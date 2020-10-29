#ifndef UTILS_H
#define UTILS_H

#include "State.h"
#include <cassert>
#include <iostream>
#include <string>

enum Move : uint8_t { DOWN, RIGHT, UP, LEFT };

constexpr char MoveStr[] = "v>^<";

constexpr std::array<Cell, 256> buildCellMapping() {
  std::array<Cell, 256> Res{};
  Res['#'] = WALL;
  Res['@'] = PLAYER;
  Res['O'] = BALL;
  Res['$'] = EMPTY_BOX;
  Res['*'] = FULL_BOX;
  Res['-'] = EMPTY;
  return Res;
}

inline std::vector<Cell> readSokoboruGrid(int Rows, int Cols) {
  constexpr auto M = buildCellMapping();
  std::vector<Cell> Grid(Rows * Cols);
  for (int R = 0; R < Rows; ++R) {
    std::string Str;
    std::cin >> Str;
    assert(Str.size() == Cols);
    for (int C = 0; C < Cols; ++C)
      Grid[R * Cols + C] = M[Str[C]];
  }
  return Grid;
}

#endif // UTILS_H
