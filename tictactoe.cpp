/*
    Mini Game Project - Tic Tac Toe
    ---------------------------------
    A console-based Tic Tac Toe game demonstrating:
    - Loops (for, while)
    - Arrays (2D board array)
    - Conditional logic (if/else, switch)
    - Dynamic board display after every move
    - Win / Draw detection
    - Replay option

    Compile:  g++ TicTacToe.cpp -o TicTacToe
    Run:      ./TicTacToe
*/

#include <iostream>
using namespace std;

const int SIZE = 3;

// Prints the current state of the board
void printBoard(char board[SIZE][SIZE]) {
    cout << "\n";
    for (int row = 0; row < SIZE; row++) {
        cout << "   ";
        for (int col = 0; col < SIZE; col++) {
            cout << board[row][col];
            if (col < SIZE - 1) cout << " | ";
        }
        cout << "\n";
        if (row < SIZE - 1) cout << "  ---+---+---\n";
    }
    cout << "\n";
}

// Resets the board to empty cells (numbered 1-9 for easy input reference)
void initBoard(char board[SIZE][SIZE]) {
    int num = 1;
    for (int row = 0; row < SIZE; row++) {
        for (int col = 0; col < SIZE; col++) {
            board[row][col] = '0' + num; // characters '1' through '9'
            num++;
        }
    }
}

// Checks whether the given player symbol has won
bool checkWin(char board[SIZE][SIZE], char symbol) {
    // Check rows and columns
    for (int i = 0; i < SIZE; i++) {
        bool rowWin = true, colWin = true;
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] != symbol) rowWin = false;
            if (board[j][i] != symbol) colWin = false;
        }
        if (rowWin || colWin) return true;
    }

    // Check diagonals
    bool diag1 = true, diag2 = true;
    for (int i = 0; i < SIZE; i++) {
        if (board[i][i] != symbol) diag1 = false;
        if (board[i][SIZE - 1 - i] != symbol) diag2 = false;
    }
    return diag1 || diag2;
}

// Checks if the board is completely filled (used to detect a draw)
bool isBoardFull(char board[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++)
        for (int col = 0; col < SIZE; col++)
            if (board[row][col] != 'X' && board[row][col] != 'O')
                return false;
    return true;
}

// Handles one player's move; returns true once a valid move is made
bool makeMove(char board[SIZE][SIZE], char symbol) {
    int choice;
    while (true) {
        cout << "Player " << symbol << ", enter a cell number (1-9): ";
        cin >> choice;

        if (cin.fail() || choice < 1 || choice > 9) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input. Please enter a number between 1 and 9.\n";
            continue;
        }

        int row = (choice - 1) / SIZE;
        int col = (choice - 1) % SIZE;

        if (board[row][col] == 'X' || board[row][col] == 'O') {
            cout << "That cell is already taken. Try again.\n";
            continue;
        }

        board[row][col] = symbol;
        return true;
    }
}

void playGame() {
    char board[SIZE][SIZE];
    initBoard(board);

    char currentPlayer = 'X';
    bool gameOver = false;

    cout << "\n===== TIC TAC TOE =====\n";
    cout << "Players take turns entering a number 1-9 corresponding to a cell:\n";
    printBoard(board);

    while (!gameOver) {
        makeMove(board, currentPlayer);
        printBoard(board);

        if (checkWin(board, currentPlayer)) {
            cout << "Player " << currentPlayer << " wins! Congratulations!\n";
            gameOver = true;
        } else if (isBoardFull(board)) {
            cout << "It's a draw! The board is full.\n";
            gameOver = true;
        } else {
            // Switch turns
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        }
    }
}

int main() {
    char playAgain = 'y';

    while (playAgain == 'y' || playAgain == 'Y') {
        playGame();

        cout << "Play again? (y/n): ";
        cin >> playAgain;
    }

    cout << "\nThanks for playing Tic Tac Toe! Goodbye.\n";
    return 0;
}