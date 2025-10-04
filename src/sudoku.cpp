#include "sudoku.h"
#include <algorithm>

Sudoku::Sudoku() {
    board_ = sampleBoard();
}

Sudoku::Sudoku(const Board& b) {
    board_ = b;
}

Board Sudoku::getBoard() const { return board_; }
void Sudoku::setBoard(const Board& b) { board_ = b; }
void Sudoku::resetToSample() { board_ = sampleBoard(); }

Board Sudoku::sampleBoard() {
    return {
        {3,0,6,5,0,8,4,0,0},
        {5,2,0,0,0,0,0,0,0},
        {0,8,7,0,0,0,0,3,1},
        {0,0,3,0,1,0,0,8,0},
        {9,0,0,8,6,3,0,0,5},
        {0,5,0,0,9,0,6,0,0},
        {1,3,0,0,0,0,2,5,0},
        {0,0,0,0,0,0,0,7,4},
        {0,0,5,2,0,6,3,0,0}
    };
}

bool Sudoku::usedInRow(const Board& board, int row, int num) const {
    for (int c = 0; c < N; ++c) if (board[row][c] == num) return true;
    return false;
}
bool Sudoku::usedInCol(const Board& board, int col, int num) const {
    for (int r = 0; r < N; ++r) if (board[r][col] == num) return true;
    return false;
}
bool Sudoku::usedInBox(const Board& board, int boxStartRow, int boxStartCol, int num) const {
    for (int r = 0; r < 3; ++r)
        for (int c = 0; c < 3; ++c)
            if (board[boxStartRow + r][boxStartCol + c] == num) return true;
    return false;
}

bool Sudoku::isSafe(const Board& board, int row, int col, int num) const {
    if (num < 1 || num > 9) return false;
    if (board[row][col] != 0) return false;
    if (usedInRow(board, row, num)) return false;
    if (usedInCol(board, col, num)) return false;
    int br = row - row % 3, bc = col - col % 3;
    if (usedInBox(board, br, bc, num)) return false;
    return true;
}

bool Sudoku::findEmpty(const Board& board, int &row, int &col) const {
    for (row = 0; row < N; ++row)
        for (col = 0; col < N; ++col)
            if (board[row][col] == 0) return true;
    return false;
}

bool Sudoku::solve(Board& board) const {
    int row, col;
    if (!findEmpty(board, row, col)) return true;

    for (int num = 1; num <= 9; ++num) {
        if (isSafe(board, row, col, num)) {
            board[row][col] = num;
            if (solve(board)) return true;
            board[row][col] = 0;
        }
    }
    return false;
}

bool Sudoku::getHint(const Board& board, int row, int col, int &hintNum) const {
    if (board[row][col] != 0) return false;
    Board copy = board;
    if (!solve(copy)) return false;
    hintNum = copy[row][col];
    return true;
}
