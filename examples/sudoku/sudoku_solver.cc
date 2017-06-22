// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "examples/sudoku/sudoku_solver.h"

#include <assert.h>

#include "voyager/util/logging.h"
#include "voyager/util/slice.h"

namespace sudoku {

SudokuSolver::SudokuSolver(const voyager::Slice& s) {
  memset(rows_, false, sizeof(rows_));
  memset(cols_, false, sizeof(cols_));
  memset(subboard_, false, sizeof(subboard_));

  assert(s.size() == kSize * kSize);

  for (size_t i = 0; i < s.size(); ++i) {
    if (s[i] != ' ') {
      board_[i / kSize][i % kSize] = s[i];
    } else {
      board_[i / kSize][i % kSize] = '.';
    }
  }
}

SudokuSolver::SudokuSolver(std::string&& s) {
  memset(rows_, false, sizeof(rows_));
  memset(cols_, false, sizeof(cols_));
  memset(subboard_, false, sizeof(subboard_));

  assert(s.size() == kSize * kSize);

  for (size_t i = 0; i < s.size(); ++i) {
    if (s[i] != ' ') {
      board_[i / kSize][i % kSize] = s[i];
    } else {
      board_[i / kSize][i % kSize] = '.';
    }
  }
}

std::string SudokuSolver::Solve() {
  std::string res;
  if (InitSudoku()) {
    if (Solve(0, 0)) {
      res.resize(kSize * kSize);
      for (int i = 0; i < kSize; ++i) {
        for (int j = 0; j < kSize; ++j) {
          res[i * kSize + j] = board_[i][j];
        }
      }
    } else {
      res = "invalid sudoku!";
    }
  }
  return res;
}

bool SudokuSolver::InitSudoku() {
  for (int i = 0; i < kSize; ++i) {
    for (int j = 0; j < kSize; ++j) {
      if (board_[i][j] == '.') {
        continue;
      }
      int n = board_[i][j] - '1';
      int m = i / 3 * 3 + j / 3;
      if (rows_[i][n] || cols_[j][n] || subboard_[m][n]) {
        return false;
      }
      rows_[i][n] = true;
      cols_[j][n] = true;
      subboard_[m][n] = true;
    }
  }
  return true;
}

#pragma GCC diagnostic ignored "-Wconversion"
bool SudokuSolver::Solve(int row, int col) {
  if (row >= kSize) {
    return true;
  }
  if (col >= kSize) {
    return Solve(row + 1, 0);
  }
  if (board_[row][col] != '.') {
    return Solve(row, col + 1);
  }

  int m = row / 3 * 3 + col / 3;
  for (int i = 0; i < kSize; ++i) {
    if (rows_[row][i] || cols_[col][i] || subboard_[m][i]) {
      continue;
    }
    rows_[row][i] = true;
    cols_[col][i] = true;
    subboard_[m][i] = true;
    board_[row][col] = i + '1';
    if (Solve(row, col + 1)) {
      return true;
    }
    rows_[row][i] = false;
    cols_[col][i] = false;
    subboard_[m][i] = false;
    board_[row][col] = '.';
  }
  return false;
}

}  // namespace sudoku
