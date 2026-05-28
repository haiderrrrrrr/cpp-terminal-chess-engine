#include <cctype>
#include <iostream>
#include <limits>
#include <string>

using namespace std;

const int BOARD_SIZE = 8;
const char EMPTY = '.';

struct Move {
    int from_row;
    int from_col;
    int to_row;
    int to_col;
};

void initializeBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    const string blackBackRank = "rnbqkbnr";
    const string whiteBackRank = "RNBQKBNR";

    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            board[row][col] = EMPTY;
        }
    }

    for (int col = 0; col < BOARD_SIZE; col++) {
        board[0][col] = blackBackRank[col];
        board[1][col] = 'p';
        board[6][col] = 'P';
        board[7][col] = whiteBackRank[col];
    }
}

bool isWhitePiece(char piece) {
    return piece != EMPTY && isupper(static_cast<unsigned char>(piece));
}

bool isBlackPiece(char piece) {
    return piece != EMPTY && islower(static_cast<unsigned char>(piece));
}

bool belongsToPlayer(char piece, bool whiteTurn) {
    return whiteTurn ? isWhitePiece(piece) : isBlackPiece(piece);
}

bool isOpponentPiece(char piece, bool whiteTurn) {
    return whiteTurn ? isBlackPiece(piece) : isWhitePiece(piece);
}

bool isInsideBoard(int row, int col) {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

void displayBoard(const char board[BOARD_SIZE][BOARD_SIZE]) {
    cout << "\n    a b c d e f g h\n";
    cout << "  +-----------------+\n";

    for (int row = 0; row < BOARD_SIZE; row++) {
        cout << 8 - row << " | ";
        for (int col = 0; col < BOARD_SIZE; col++) {
            cout << board[row][col] << ' ';
        }
        cout << "| " << 8 - row << '\n';
    }

    cout << "  +-----------------+\n";
    cout << "    a b c d e f g h\n\n";
}

bool parseSquare(const string& square, int& row, int& col) {
    if (square.length() != 2) {
        return false;
    }

    char file = static_cast<char>(tolower(static_cast<unsigned char>(square[0])));
    char rank = square[1];

    if (file < 'a' || file > 'h' || rank < '1' || rank > '8') {
        return false;
    }

    col = file - 'a';
    row = '8' - rank;
    return true;
}

bool readMove(Move& move) {
    string from;
    string to;

    cout << "Enter move (example: e2 e4) or 'quit': ";
    cin >> from;

    if (!cin || from == "quit" || from == "q") {
        return false;
    }

    cin >> to;
    if (!cin) {
        return false;
    }

    if (!parseSquare(from, move.from_row, move.from_col) ||
        !parseSquare(to, move.to_row, move.to_col)) {
        cout << "Use board coordinates like e2 e4.\n";
        return true;
    }

    return true;
}

int sign(int value) {
    if (value > 0) {
        return 1;
    }
    if (value < 0) {
        return -1;
    }
    return 0;
}

bool isPathClear(const char board[BOARD_SIZE][BOARD_SIZE], const Move& move) {
    int rowStep = sign(move.to_row - move.from_row);
    int colStep = sign(move.to_col - move.from_col);
    int row = move.from_row + rowStep;
    int col = move.from_col + colStep;

    while (row != move.to_row || col != move.to_col) {
        if (board[row][col] != EMPTY) {
            return false;
        }
        row += rowStep;
        col += colStep;
    }

    return true;
}

bool isValidPawnMove(const char board[BOARD_SIZE][BOARD_SIZE], const Move& move, bool whiteTurn) {
    int direction = whiteTurn ? -1 : 1;
    int startRow = whiteTurn ? 6 : 1;
    int rowDiff = move.to_row - move.from_row;
    int colDiff = move.to_col - move.from_col;
    char target = board[move.to_row][move.to_col];

    if (colDiff == 0 && rowDiff == direction && target == EMPTY) {
        return true;
    }

    if (colDiff == 0 && move.from_row == startRow && rowDiff == 2 * direction &&
        target == EMPTY && board[move.from_row + direction][move.from_col] == EMPTY) {
        return true;
    }

    if (abs(colDiff) == 1 && rowDiff == direction && isOpponentPiece(target, whiteTurn)) {
        return true;
    }

    return false;
}

bool isValidPieceMove(const char board[BOARD_SIZE][BOARD_SIZE], const Move& move, bool whiteTurn) {
    char piece = board[move.from_row][move.from_col];
    int rowDiff = abs(move.to_row - move.from_row);
    int colDiff = abs(move.to_col - move.from_col);

    switch (tolower(static_cast<unsigned char>(piece))) {
        case 'p':
            return isValidPawnMove(board, move, whiteTurn);
        case 'n':
            return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
        case 'b':
            return rowDiff == colDiff && isPathClear(board, move);
        case 'r':
            return (rowDiff == 0 || colDiff == 0) && isPathClear(board, move);
        case 'q':
            return ((rowDiff == colDiff) || (rowDiff == 0 || colDiff == 0)) && isPathClear(board, move);
        case 'k':
            return rowDiff <= 1 && colDiff <= 1;
        default:
            return false;
    }
}

bool isPseudoLegalMove(const char board[BOARD_SIZE][BOARD_SIZE], const Move& move, bool whiteTurn) {
    if (!isInsideBoard(move.from_row, move.from_col) || !isInsideBoard(move.to_row, move.to_col)) {
        return false;
    }

    if (move.from_row == move.to_row && move.from_col == move.to_col) {
        return false;
    }

    char piece = board[move.from_row][move.from_col];
    char target = board[move.to_row][move.to_col];

    if (!belongsToPlayer(piece, whiteTurn)) {
        return false;
    }

    if (belongsToPlayer(target, whiteTurn)) {
        return false;
    }

    if (tolower(static_cast<unsigned char>(target)) == 'k') {
        return false;
    }

    return isValidPieceMove(board, move, whiteTurn);
}

bool findKing(const char board[BOARD_SIZE][BOARD_SIZE], bool whiteKing, int& kingRow, int& kingCol) {
    char king = whiteKing ? 'K' : 'k';

    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            if (board[row][col] == king) {
                kingRow = row;
                kingCol = col;
                return true;
            }
        }
    }

    return false;
}

bool canAttackSquare(const char board[BOARD_SIZE][BOARD_SIZE], int fromRow, int fromCol, int targetRow, int targetCol) {
    char piece = board[fromRow][fromCol];
    if (piece == EMPTY) {
        return false;
    }

    bool whitePiece = isWhitePiece(piece);
    int rowDiff = abs(targetRow - fromRow);
    int colDiff = abs(targetCol - fromCol);
    Move attack{fromRow, fromCol, targetRow, targetCol};

    switch (tolower(static_cast<unsigned char>(piece))) {
        case 'p': {
            int direction = whitePiece ? -1 : 1;
            return targetRow - fromRow == direction && abs(targetCol - fromCol) == 1;
        }
        case 'n':
            return (rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2);
        case 'b':
            return rowDiff == colDiff && isPathClear(board, attack);
        case 'r':
            return (rowDiff == 0 || colDiff == 0) && isPathClear(board, attack);
        case 'q':
            return ((rowDiff == colDiff) || (rowDiff == 0 || colDiff == 0)) && isPathClear(board, attack);
        case 'k':
            return rowDiff <= 1 && colDiff <= 1;
        default:
            return false;
    }
}

bool isSquareAttacked(const char board[BOARD_SIZE][BOARD_SIZE], int row, int col, bool byWhite) {
    for (int fromRow = 0; fromRow < BOARD_SIZE; fromRow++) {
        for (int fromCol = 0; fromCol < BOARD_SIZE; fromCol++) {
            char piece = board[fromRow][fromCol];
            if (piece == EMPTY || isWhitePiece(piece) != byWhite) {
                continue;
            }

            if (canAttackSquare(board, fromRow, fromCol, row, col)) {
                return true;
            }
        }
    }

    return false;
}

bool isKingInCheck(const char board[BOARD_SIZE][BOARD_SIZE], bool whiteKing) {
    int kingRow = 0;
    int kingCol = 0;

    if (!findKing(board, whiteKing, kingRow, kingCol)) {
        return true;
    }

    return isSquareAttacked(board, kingRow, kingCol, !whiteKing);
}

void promotePawnIfNeeded(char board[BOARD_SIZE][BOARD_SIZE], const Move& move, bool announce) {
    char& piece = board[move.to_row][move.to_col];

    if (piece == 'P' && move.to_row == 0) {
        piece = 'Q';
        if (announce) {
            cout << "White pawn promoted to queen.\n";
        }
    } else if (piece == 'p' && move.to_row == 7) {
        piece = 'q';
        if (announce) {
            cout << "Black pawn promoted to queen.\n";
        }
    }
}

void applyMove(char board[BOARD_SIZE][BOARD_SIZE], const Move& move, bool announce = true) {
    board[move.to_row][move.to_col] = board[move.from_row][move.from_col];
    board[move.from_row][move.from_col] = EMPTY;
    promotePawnIfNeeded(board, move, announce);
}

bool isLegalMove(const char board[BOARD_SIZE][BOARD_SIZE], const Move& move, bool whiteTurn) {
    if (!isPseudoLegalMove(board, move, whiteTurn)) {
        return false;
    }

    char nextBoard[BOARD_SIZE][BOARD_SIZE];
    for (int row = 0; row < BOARD_SIZE; row++) {
        for (int col = 0; col < BOARD_SIZE; col++) {
            nextBoard[row][col] = board[row][col];
        }
    }

    applyMove(nextBoard, move, false);
    return !isKingInCheck(nextBoard, whiteTurn);
}

bool hasAnyLegalMove(const char board[BOARD_SIZE][BOARD_SIZE], bool whiteTurn) {
    for (int fromRow = 0; fromRow < BOARD_SIZE; fromRow++) {
        for (int fromCol = 0; fromCol < BOARD_SIZE; fromCol++) {
            if (!belongsToPlayer(board[fromRow][fromCol], whiteTurn)) {
                continue;
            }

            for (int toRow = 0; toRow < BOARD_SIZE; toRow++) {
                for (int toCol = 0; toCol < BOARD_SIZE; toCol++) {
                    Move move{fromRow, fromCol, toRow, toCol};
                    if (isLegalMove(board, move, whiteTurn)) {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

int main() {
    char board[BOARD_SIZE][BOARD_SIZE];
    bool whiteTurn = true;

    initializeBoard(board);

    cout << "Terminal Chess Engine\n";
    cout << "White uses uppercase pieces. Black uses lowercase pieces.\n";
    cout << "Move format: source square, then target square. Example: e2 e4\n";
    cout << "Type 'quit' to exit.\n";

    while (true) {
        displayBoard(board);

        bool inCheck = isKingInCheck(board, whiteTurn);
        if (!hasAnyLegalMove(board, whiteTurn)) {
            if (inCheck) {
                cout << (whiteTurn ? "Checkmate. Black wins.\n" : "Checkmate. White wins.\n");
            } else {
                cout << "Stalemate. Draw.\n";
            }
            return 0;
        }

        if (inCheck) {
            cout << "Check.\n";
        }

        cout << (whiteTurn ? "White" : "Black") << " to move.\n";

        Move move{};
        if (!readMove(move)) {
            cout << "Game ended by player.\n";
            return 0;
        }

        if (!isLegalMove(board, move, whiteTurn)) {
            cout << "Invalid move. Try again.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        char captured = board[move.to_row][move.to_col];
        applyMove(board, move);

        if (captured != EMPTY) {
            cout << "Captured " << captured << ".\n";
        }

        whiteTurn = !whiteTurn;

        if (isKingInCheck(board, whiteTurn)) {
            cout << "Check.\n";
        }
    }
}
