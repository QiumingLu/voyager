add_executable(sudoku_server sudoku_server.cc sudoku_solver.cc)
target_link_libraries(sudoku_server voyager)

add_executable(sudoku_client sudoku_client.cc)
target_link_libraries(sudoku_client voyager)
