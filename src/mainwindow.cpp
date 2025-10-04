#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QIntValidator>
#include <QMessageBox>
#include <QFont>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), model_() {

    setupUi();
    loadBoardToUi(model_.getBoard());
    markFixedCells();
    styleUi();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi() {
    central_ = new QWidget(this);
    setCentralWidget(central_);

    QVBoxLayout *mainLayout = new QVBoxLayout(central_);

    QLabel *title = new QLabel("Sudoku — C++ (Qt) • Backtracking Solver", this);
    QFont titleFont = title->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    title->setFont(titleFont);
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);

    // Board grid
    QWidget *boardWidget = new QWidget(this);
    boardLayout_ = new QGridLayout(boardWidget);
    boardLayout_->setSpacing(6);
    cells_.reserve(81);

    QFont cellFont;
    cellFont.setPointSize(16);
    cellFont.setBold(true);

    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            QLineEdit *le = new QLineEdit(this);
            le->setFixedSize(54, 54);
            le->setAlignment(Qt::AlignCenter);
            le->setFont(cellFont);
            le->setMaxLength(1);
            le->setValidator(new QIntValidator(1,9,le));
            // subtle style for box borders: using stylesheet later
            boardLayout_->addWidget(le, r, c);
            cells_.push_back(le);
            connect(le, &QLineEdit::textEdited, this, &MainWindow::onCellEdited);
        }
    }

    mainLayout->addWidget(boardWidget, 0);

    // Buttons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    placeBtn = new QPushButton("Place (sync to model)", this);
    undoBtn  = new QPushButton("Undo (clear selected)", this);
    hintBtn  = new QPushButton("Hint", this);
    solveBtn = new QPushButton("Solve", this);
    resetBtn = new QPushButton("Reset", this);

    btnLayout->addWidget(placeBtn);
    btnLayout->addWidget(undoBtn);
    btnLayout->addWidget(hintBtn);
    btnLayout->addWidget(solveBtn);
    btnLayout->addWidget(resetBtn);

    mainLayout->addLayout(btnLayout);

    // connections
    connect(placeBtn, &QPushButton::clicked, this, &MainWindow::onPlaceClicked);
    connect(undoBtn, &QPushButton::clicked, this, &MainWindow::onUndoClicked);
    connect(hintBtn, &QPushButton::clicked, this, &MainWindow::onHintClicked);
    connect(solveBtn, &QPushButton::clicked, this, &MainWindow::onSolveClicked);
    connect(resetBtn, &QPushButton::clicked, this, &MainWindow::onResetClicked);

    setFixedSize(600, 720);
}

void MainWindow::styleUi() {
    // make 3x3 boxes visually distinct using stylesheet on each cell
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            QLineEdit *le = cells_[r*9 + c];
            QString borderStyle = "border: 1px solid #9aa7b2; background: #f7fafc;";
            // thicker borders for box boundaries
            if (c % 3 == 0) borderStyle += "border-left: 3px solid #2b2f3a;";
            if (r % 3 == 0) borderStyle += "border-top: 3px solid #2b2f3a;";
            if (c == 8) borderStyle += "border-right: 3px solid #2b2f3a;";
            if (r == 8) borderStyle += "border-bottom: 3px solid #2b2f3a;";
            le->setStyleSheet(borderStyle + " border-radius: 6px;");
        }
    }
    // buttons style (slightly modern)
    QString btnStyle = "QPushButton { padding: 8px 12px; font-weight:600; border-radius:6px; background:#2b90ff; color:white; }"
                       "QPushButton:hover { background:#1f6fe0; }";
    placeBtn->setStyleSheet(btnStyle);
    undoBtn->setStyleSheet(btnStyle);
    hintBtn->setStyleSheet(btnStyle);
    solveBtn->setStyleSheet(btnStyle);
    resetBtn->setStyleSheet("QPushButton { padding:8px; background:#f05454; color:white; border-radius:6px; }"
                            "QPushButton:hover { background:#d64444; }");
}

void MainWindow::loadBoardToUi(const Board& b) {
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            int v = b[r][c];
            QLineEdit *le = cells_[r*9 + c];
            if (v == 0) le->clear();
            else {
                le->setText(QString::number(v));
                le->setReadOnly(true);
                // fixed cells will be marked later
            }
        }
    }
}

Board MainWindow::readBoardFromUi() const {
    Board b(9, std::vector<int>(9,0));
    for (int r = 0; r < 9; ++r)
        for (int c = 0; c < 9; ++c) {
            QString t = cells_[r*9 + c]->text().trimmed();
            if (!t.isEmpty()) b[r][c] = t.toInt();
        }
    return b;
}

void MainWindow::markFixedCells() {
    Board b = model_.getBoard();
    fixedMask_.resize(81);
    for (int i = 0; i < 81; ++i) fixedMask_[i] = false;
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (b[r][c] != 0) {
                int idx = r*9 + c;
                fixedMask_[idx] = true;
                QLineEdit *le = cells_[idx];
                le->setStyleSheet(le->styleSheet() + " background:#dceefc; color:#123;"); // different bg for fixed
            }
        }
    }
}

void MainWindow::onPlaceClicked() {
    // copy UI to model (only non-fixed cells)
    Board b = readBoardFromUi();
    // validate each user input vs model rules
    bool ok = true;
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            int val = b[r][c];
            if (val == 0) continue;
            // allow user to place only if it's valid against other numbers entered or fixed ones
            Board temp = b;
            temp[r][c] = 0; // temporarily clear itself to check safety in context of other numbers
            if (!model_.isSafe(temp, r, c, val)) {
                ok = false;
                QMessageBox::warning(this, "Invalid Move",
                    QString("Invalid value %1 at row %2 col %3").arg(val).arg(r+1).arg(c+1));
                return;
            }
        }
    }
    // if ok, update model
    model_.setBoard(b);
    QMessageBox::information(this, "Placed", "Board synchronized with model.");
}

void MainWindow::onUndoClicked() {
    // clear selected cell(s): pick first focused cell or show a simple prompt
    for (int i = 0; i < 81; ++i) {
        if (cells_[i]->hasFocus()) {
            if (fixedMask_[i]) {
                QMessageBox::information(this, "Undo", "Cannot undo a fixed starting cell.");
            } else {
                cells_[i]->clear();
            }
            return;
        }
    }
    QMessageBox::information(this, "Undo", "Select a cell then press Undo.");
}

void MainWindow::onHintClicked() {
    // use the focused cell as hint target
    for (int i = 0; i < 81; ++i) {
        if (cells_[i]->hasFocus()) {
            int r = i / 9, c = i % 9;
            Board cur = readBoardFromUi();
            if (cur[r][c] != 0) {
                QMessageBox::information(this, "Hint", "Cell already filled.");
                return;
            }
            int hintNum;
            if (model_.getHint(cur, r, c, hintNum)) {
                cells_[i]->setText(QString::number(hintNum));
                // optionally set as temporary (not fixed)
                QMessageBox::information(this, "Hint", QString("Try %1 at (%2,%3)").arg(hintNum).arg(r+1).arg(c+1));
            } else QMessageBox::information(this, "Hint", "No hint available (puzzle unsolvable or filled).");
            return;
        }
    }
    QMessageBox::information(this, "Hint", "Select a cell to get a hint.");
}

void MainWindow::onSolveClicked() {
    Board cur = readBoardFromUi();
    Board copy = cur;
    if (model_.solve(copy)) {
        loadBoardToUi(copy);
        QMessageBox::information(this, "Solved", "Puzzle solved (solution loaded to UI).");
    } else {
        QMessageBox::warning(this, "No Solution", "No solution exists for the current board.");
    }
}

void MainWindow::onResetClicked() {
    model_.resetToSample();
    loadBoardToUi(model_.getBoard());
    markFixedCells();
}

void MainWindow::onCellEdited() {
    // basic UX: if user types something invalid (like 0), remove
    QLineEdit *le = qobject_cast<QLineEdit*>(sender());
    if (!le) return;
    QString t = le->text().trimmed();
    if (t.isEmpty()) return;
    bool ok;
    int v = t.toInt(&ok);
    if (!ok || v < 1 || v > 9) le->clear();
}
