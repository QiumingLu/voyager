#ifndef VOYAGER_EXAMPLES_SUDOKU_SUDOKU_SOLVER_H_
#define VOYAGER_EXAMPLES_SUDOKU_SUDOKU_SOLVER_H_

#include <string>
#include "voyager/util/slice.h"

namespace sudoku {

class SudokuSolver {
 public:
  SudokuSolver(const voyager::Slice& s);
  SudokuSolver(std::string&& s);

  std::string Solve();

 private:
  bool InitSudoku();
  bool Solve(int row, int col);

  const static int kSize = 9;
  bool rows_[kSize][kSize];
  bool cols_[kSize][kSize];
  bool subboard_[kSize][kSize];
  char board_[kSize][kSize];
};

}  // namespace sudoku

#endif  // VOYAGER_EXAMPLES_SUDOKU_SUDOKU_SOLVER_H_
