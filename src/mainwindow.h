#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QGridLayout>
#include <QPushButton>
#include "sudoku.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPlaceClicked();
    void onUndoClicked();
    void onHintClicked();
    void onSolveClicked();
    void onResetClicked();
    void onCellEdited();

private:
    void setupUi();
    void loadBoardToUi(const Board& b);
    Board readBoardFromUi() const;
    void styleUi();
    void markFixedCells();

    QWidget *central_;
    QGridLayout *boardLayout_;
    QVector<QLineEdit*> cells_; // 81 cells
    QPushButton *placeBtn;
    QPushButton *undoBtn;
    QPushButton *hintBtn;
    QPushButton *solveBtn;
    QPushButton *resetBtn;

    Sudoku model_;
    QVector<bool> fixedMask_;
};

#endif // MAINWINDOW_H
