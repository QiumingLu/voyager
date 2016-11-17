#ifndef EXAMPLES_SUDOKU_SUDOKU_SOLVER_H_
#define EXAMPLES_SUDOKU_SUDOKU_SOLVER_H_

#include <string>
#include "voyager/util/slice.h"

namespace sudoku {

class SudokuSolver {
 public:
  explicit SudokuSolver(const voyager::Slice& s);
  explicit SudokuSolver(std::string&& s);

  std::string Solve();

 private:
  static const int kSize = 9;

  bool InitSudoku();
  bool Solve(int row, int col);

  bool rows_[kSize][kSize];
  bool cols_[kSize][kSize];
  bool subboard_[kSize][kSize];
  char board_[kSize][kSize];
};

}  // namespace sudoku

#endif  // EXAMPLES_SUDOKU_SUDOKU_SOLVER_H_
