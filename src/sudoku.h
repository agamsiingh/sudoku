#ifndef SUDOKU_H
#define SUDOKU_H

#include <vector>
using Board = std::vector<std::vector<int>>;

class Sudoku {
public:
    static const int N = 9;

    Sudoku();
    explicit Sudoku(const Board& b);

    Board getBoard() const;
    void setBoard(const Board& b);
    void resetToSample();
    static Board sampleBoard();

    bool isSafe(const Board& board, int row, int col, int num) const;
    bool solve(Board& board) const;               // backtracking solver (fills board)
    bool getHint(const Board& board, int row, int col, int &hintNum) const;

private:
    bool usedInRow(const Board& board, int row, int num) const;
    bool usedInCol(const Board& board, int col, int num) const;
    bool usedInBox(const Board& board, int boxStartRow, int boxStartCol, int num) const;
    bool findEmpty(const Board& board, int &row, int &col) const;

    Board board_;
};

#endif // SUDOKU_H
