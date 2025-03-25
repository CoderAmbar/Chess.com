#include "raylib.h"
#include <stdio.h>
#include <direct.h>
#include <stdlib.h>
#include <algorithm>

using namespace std;

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 500

// Function declarations
void DrawBoard(bool isWhiteTurn);
void DrawPieces(int board[8][8]);
void LoadPieceImages();
void UnloadPieceImages();
void HandlePieceMovement(int board[8][8], int row, int col, int possibleMoves[8][8]);
void DrawValidMoves(int board[8][8], int piece, int row, int col);
bool IsValidMove(int board[8][8], int piece, int startRow, int startCol, int endRow, int endCol, bool validateCheck = true);
bool IsKingInCheck(int board[8][8], bool isWhite);
bool IsCheckmate(int board[8][8], bool isWhite);
bool WouldBeInCheck(int board[8][8], int piece, int startRow, int startCol, int endRow, int endCol, bool isWhite);
void PromotePawn(int board[8][8], int row, int col, bool isWhite);
bool CanCastle(int board[8][8], bool isWhite, bool kingside);

// Global variables for textures
Texture2D whitePawn, whiteKnight, whiteBishop, whiteRook, whiteQueen, whiteKing;
Texture2D blackPawn, blackKnight, blackBishop, blackRook, blackQueen, blackKing;

// Selection variables
int selectedSquareRow = -1;
int selectedSquareCol = -1;
bool pieceSelected = false;

// Game state variables
int enPassantTargetRow = -1;
int enPassantTargetCol = -1;
bool whiteKingMoved = false;
bool whiteRookKingsideMoved = false;
bool whiteRookQueensideMoved = false;
bool blackKingMoved = false;
bool blackRookKingsideMoved = false;
bool blackRookQueensideMoved = false;

// Initial board setup
int board[8][8] = {
    {-4, -2, -3, -5, -6, -3, -2, -4},
    {-1, -1, -1, -1, -1, -1, -1, -1},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {4, 2, 3, 5, 6, 3, 2, 4}
};

void LoadPieceImages() {
    whitePawn = LoadTexture("D:/Projects and Stuff/assets/white_pawn.png");
    whiteKnight = LoadTexture("D:/Projects and Stuff/assets/white_knight.png");
    whiteBishop = LoadTexture("D:/Projects and Stuff/assets/white_bishop.png");
    whiteRook = LoadTexture("D:/Projects and Stuff/assets/white_rook.png");
    whiteQueen = LoadTexture("D:/Projects and Stuff/assets/white_queen.png");
    whiteKing = LoadTexture("D:/Projects and Stuff/assets/white_king.png");

    blackPawn = LoadTexture("D:/Projects and Stuff/assets/black_pawn.png");
    blackKnight = LoadTexture("D:/Projects and Stuff/assets/black_knight.png");
    blackBishop = LoadTexture("D:/Projects and Stuff/assets/black_bishop.png");
    blackRook = LoadTexture("D:/Projects and Stuff/assets/black_rook.png");
    blackQueen = LoadTexture("D:/Projects and Stuff/assets/black_queen.png");
    blackKing = LoadTexture("D:/Projects and Stuff/assets/black_king.png");
}

void UnloadPieceImages() {
    UnloadTexture(whitePawn);
    UnloadTexture(whiteKnight);
    UnloadTexture(whiteBishop);
    UnloadTexture(whiteRook);
    UnloadTexture(whiteQueen);
    UnloadTexture(whiteKing);
    UnloadTexture(blackPawn);
    UnloadTexture(blackKnight);
    UnloadTexture(blackBishop);
    UnloadTexture(blackRook);
    UnloadTexture(blackQueen);
    UnloadTexture(blackKing);
}

void DrawBoard(bool isWhiteTurn) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            Color color = ((row + col) % 2 == 0) ? BEIGE : DARKBROWN;
            DrawRectangle(col * 62.5, row * 62.5, 62.5, 62.5, color);
        }
    }

    // Check ka highlight 
    if (IsKingInCheck(board, isWhiteTurn)) {
        int kingRow = -1, kingCol = -1;
        int kingPiece = isWhiteTurn ? 6 : -6;

        // King ki position
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                if (board[row][col] == kingPiece) {
                    kingRow = row;
                    kingCol = col;
                    break;
                }
            }
            if (kingRow != -1) break;
        }

        if (kingRow != -1 && kingCol != -1) {
            DrawRectangle(kingCol * 62.5, kingRow * 62.5, 62.5, 62.5, RED);
        }
    }
}

void DrawPieces(int board[8][8]) {
    const float squareSize = 62.5;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            int piece = board[row][col];
            if (piece == 0) continue;

            Texture2D* pieceTexture = nullptr;
            switch (piece) {
                case 1: pieceTexture = &whitePawn; break;
                case 2: pieceTexture = &whiteKnight; break;
                case 3: pieceTexture = &whiteBishop; break;
                case 4: pieceTexture = &whiteRook; break;
                case 5: pieceTexture = &whiteQueen; break;
                case 6: pieceTexture = &whiteKing; break;
                case -1: pieceTexture = &blackPawn; break;
                case -2: pieceTexture = &blackKnight; break;
                case -3: pieceTexture = &blackBishop; break;
                case -4: pieceTexture = &blackRook; break;
                case -5: pieceTexture = &blackQueen; break;
                case -6: pieceTexture = &blackKing; break;
                default: pieceTexture = nullptr; break;
            }

            if (pieceTexture) {
                float x = col * squareSize + (squareSize - pieceTexture->width * 0.5f) / 2;
                float y = row * squareSize + (squareSize - pieceTexture->height * 0.5f) / 2;

                Vector2 position = {x, y};
                DrawTextureEx(*pieceTexture, position, 0.0f, 0.5f, WHITE);
            }
        }
    }
}

bool WouldBeInCheck(int board[8][8], int piece, int startRow, int startCol, int endRow, int endCol, bool isWhite) {
    int tempBoard[8][8];
    memcpy(tempBoard, board, sizeof(int) * 8 * 8);
    
    tempBoard[endRow][endCol] = piece;
    tempBoard[startRow][startCol] = 0;
    
    return IsKingInCheck(tempBoard, isWhite);
}

bool CanCastle(int board[8][8], bool isWhite, bool kingside) {
    int row = isWhite ? 7 : 0;
    int kingCol = 4;
    int rookCol = kingside ? 7 : 0;
    int step = kingside ? 1 : -1;
    
    // Check if king and rook have moved
    if (isWhite) {
        if (whiteKingMoved) return false;
        if (kingside && whiteRookKingsideMoved) return false;
        if (!kingside && whiteRookQueensideMoved) return false;
    } else {
        if (blackKingMoved) return false;
        if (kingside && blackRookKingsideMoved) return false;
        if (!kingside && blackRookQueensideMoved) return false;
    }
    
    // Check if squares between are empty
    for (int col = kingCol + step; col != rookCol; col += step) {
        if (board[row][col] != 0) return false;
    }
    
    // Check if king is in check or would pass through check
    for (int col = kingCol; col != kingCol + 2 * step; col += step) {
        int tempBoard[8][8];
        memcpy(tempBoard, board, sizeof(int) * 8 * 8);
        
        tempBoard[row][col] = isWhite ? 6 : -6;
        tempBoard[row][kingCol] = 0;
        
        if (IsKingInCheck(tempBoard, isWhite)) {
            return false;
        }
    }
    
    return true;
}

void PromotePawn(int board[8][8], int row, int col, bool isWhite) {
    if (row != 0 && row != 7) return; // Only promote on first/last rank
    
    // In a real game, you'd show a promotion menu, but we'll just promote to queen here
    board[row][col] = isWhite ? 5 : -5;
}

bool IsValidMove(int board[8][8], int piece, int startRow, int startCol, int endRow, int endCol, bool validateCheck) {
    if (endRow < 0 || endRow >= 8 || endCol < 0 || endCol >= 8) {
        return false;
    }

    // Can't capture your own piece
    if (board[endRow][endCol] != 0 && (board[endRow][endCol] * piece > 0)) {
        return false;
    }

    int rowDiff = endRow - startRow;
    int colDiff = endCol - startCol;

    switch (abs(piece)) {
        case 1: { // Pawn
            int direction = (piece > 0) ? -1 : 1; 

            // Normal move forward
            if (colDiff == 0 && rowDiff == direction && board[endRow][endCol] == 0) {
                if (validateCheck && WouldBeInCheck(board, piece, startRow, startCol, endRow, endCol, piece > 0)) {
                    return false;
                }
                return true;
            }

            // Double move from starting position
            if (colDiff == 0 && rowDiff == 2 * direction &&
                ((piece == 1 && startRow == 6) || (piece == -1 && startRow == 1)) &&
                board[startRow + direction][startCol] == 0 &&
                board[endRow][endCol] == 0) {
                if (validateCheck && WouldBeInCheck(board, piece, startRow, startCol, endRow, endCol, piece > 0)) {
                    return false;
                }
                return true;
            }

            // Capture diagonally
            if (abs(colDiff) == 1 && rowDiff == direction && board[endRow][endCol] * piece < 0) {
                if (validateCheck && WouldBeInCheck(board, piece, startRow, startCol, endRow, endCol, piece > 0)) {
                    return false;
                }
                return true;
            }

            // En passant
            if (abs(colDiff) == 1 && rowDiff == direction && 
                endRow == enPassantTargetRow && endCol == enPassantTargetCol) {
                if (validateCheck && WouldBeInCheck(board, piece, startRow, startCol, endRow, endCol, piece > 0)) {
                    return false;
                }
                return true;
            }

            break;
        }
        case 2: { // Knight
            if ((abs(rowDiff) == 2 && abs(colDiff) == 1) || (abs(rowDiff) == 1 && abs(colDiff) == 2)) {
                if (validateCheck && WouldBeInCheck(board, piece, startRow, startCol, endRow, endCol, piece > 0)) {
                    return false;
                }
                return true;
            }
            break;
        }
        case 3: { // Bishop
            if (abs(rowDiff) == abs(colDiff)) {
                int stepRow = rowDiff > 0 ? 1 : -1;
                int stepCol = colDiff > 0 ? 1 : -1;

                for (int i = 1; i < abs(rowDiff); i++) {
                    if (board[startRow + i * stepRow][startCol + i * stepCol] != 0) {
                        return false;
                    }
                }
                if (validateCheck && WouldBeInCheck(board, piece, startRow, startCol, endRow, endCol, piece > 0)) {
                    return false;
                }
                return true;
            }
            break;
        }
        case 4: { // Rook
            if (rowDiff == 0 || colDiff == 0) {
                int stepRow = (rowDiff == 0) ? 0 : (rowDiff > 0 ? 1 : -1);
                int stepCol = (colDiff == 0) ? 0 : (colDiff > 0 ? 1 : -1);

                for (int i = 1; i < max(abs(rowDiff), abs(colDiff)); i++) {
                    if (board[startRow + i * stepRow][startCol + i * stepCol] != 0) {
                        return false;
                    }
                }
                if (validateCheck && WouldBeInCheck(board, piece, startRow, startCol, endRow, endCol, piece > 0)) {
                    return false;
                }
                return true;
            }
            break;
        }
        case 5: { // Queen
            if (abs(rowDiff) == abs(colDiff)) {
                int stepRow = rowDiff > 0 ? 1 : -1;
                int stepCol = colDiff > 0 ? 1 : -1;

                for (int i = 1; i < abs(rowDiff); i++) {
                    if (board[startRow + i * stepRow][startCol + i * stepCol] != 0) {
                        return false; 
                    }
                }
                if (validateCheck && WouldBeInCheck(board, piece, startRow, startCol, endRow, endCol, piece > 0)) {
                    return false;
                }
                return true;
            }
            if (rowDiff == 0 || colDiff == 0) {
                int stepRow = (rowDiff == 0) ? 0 : (rowDiff > 0 ? 1 : -1);
                int stepCol = (colDiff == 0) ? 0 : (colDiff > 0 ? 1 : -1);

                for (int i = 1; i < max(abs(rowDiff), abs(colDiff)); i++) {
                    if (board[startRow + i * stepRow][startCol + i * stepCol] != 0) {
                        return false; 
                    }
                }
                if (validateCheck && WouldBeInCheck(board, piece, startRow, startCol, endRow, endCol, piece > 0)) {
                    return false;
                }
                return true;
            }
            break;
        }
        case 6: { // King
            // Normal king move
            if ((abs(rowDiff) <= 1 && abs(colDiff) <= 1)) {
                if (validateCheck && WouldBeInCheck(board, piece, startRow, startCol, endRow, endCol, piece > 0)) {
                    return false;
                }
                return true; 
            }
            
            // Castling
            if (abs(colDiff) == 2 && rowDiff == 0) {
                bool kingside = colDiff > 0;
                bool isWhite = piece > 0;
                
                if (CanCastle(board, isWhite, kingside)) {
                    if (validateCheck && WouldBeInCheck(board, piece, startRow, startCol, endRow, endCol, piece > 0)) {
                        return false;
                    }
                    return true;
                }
            }
            break;
        }
    }

    return false;
}

void HandlePieceMovement(int board[8][8], int row, int col, int possibleMoves[8][8]) {
    int piece = board[row][col];
    if (piece == 0) return;

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            possibleMoves[r][c] = 0;
        }
    }

    auto isValid = [](int r, int c) {
        return r >= 0 && r < 8 && c >= 0 && c < 8;
    };

    switch (abs(piece)) {
        case 1: { // Pawn
            int direction = (piece > 0) ? -1 : 1;

            // Single move forward
            if (isValid(row + direction, col) && board[row + direction][col] == 0) {
                if (IsValidMove(board, piece, row, col, row + direction, col)) {
                    possibleMoves[row + direction][col] = 1;
                }
            }

            // Double move from starting position
            if (((piece == 1 && row == 6) || (piece == -1 && row == 1)) &&
                isValid(row + 2 * direction, col) && board[row + 2 * direction][col] == 0 &&
                board[row + direction][col] == 0) {
                if (IsValidMove(board, piece, row, col, row + 2 * direction, col)) {
                    possibleMoves[row + 2 * direction][col] = 1;
                }
            }

            // Capturing diagonally
            if (isValid(row + direction, col - 1) && board[row + direction][col - 1] * piece < 0) {
                if (IsValidMove(board, piece, row, col, row + direction, col - 1)) {
                    possibleMoves[row + direction][col - 1] = 1;
                }
            }
            if (isValid(row + direction, col + 1) && board[row + direction][col + 1] * piece < 0) {
                if (IsValidMove(board, piece, row, col, row + direction, col + 1)) {
                    possibleMoves[row + direction][col + 1] = 1;
                }
            }

            // En passant
            if (enPassantTargetRow != -1) {
                if (abs(col - enPassantTargetCol) == 1 && row + direction == enPassantTargetRow) {
                    if (IsValidMove(board, piece, row, col, enPassantTargetRow, enPassantTargetCol)) {
                        possibleMoves[enPassantTargetRow][enPassantTargetCol] = 1;
                    }
                }
            }
            break;
        }
        case 2: { // Knight
            int moves[8][2] = {
                {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
            };

            for (auto& move : moves) {
                int r = row + move[0], c = col + move[1];
                if (isValid(r, c) && board[r][c] * piece <= 0) {
                    if (IsValidMove(board, piece, row, col, r, c)) {
                        possibleMoves[r][c] = 1;
                    }
                }
            }
            break;
        }
        case 3: // Bishop
        case 4: { // Rook
            int directions[8][2] = {
                {-1, -1}, {-1, 1}, {1, -1}, {1, 1}, // Bishop
                {-1, 0}, {1, 0}, {0, -1}, {0, 1}    // Rook
            };

            int maxSteps = (abs(piece) == 3) ? 4 : 8; // Limit directions for bishop and rook
            for (int i = 0; i < maxSteps; i++) {
                int stepRow = directions[i][0], stepCol = directions[i][1];
                for (int dist = 1; dist < 8; dist++) {
                    int r = row + dist * stepRow, c = col + dist * stepCol;
                    if (!isValid(r, c) || (board[r][c] != 0 && board[r][c] * piece > 0)) break;

                    if (IsValidMove(board, piece, row, col, r, c)) {
                        possibleMoves[r][c] = 1;
                    }

                    if (board[r][c] * piece < 0) break;
                }
            }
            break;
        }
        case 5: { // Queen
            int directions[8][2] = {
                {-1, -1}, {-1, 1}, {1, -1}, {1, 1},
                {-1, 0}, {1, 0}, {0, -1}, {0, 1}
            };

            for (auto& dir : directions) {
                for (int dist = 1; dist < 8; dist++) {
                    int r = row + dist * dir[0], c = col + dist * dir[1];
                    if (!isValid(r, c) || (board[r][c] != 0 && board[r][c] * piece > 0)) break;

                    if (IsValidMove(board, piece, row, col, r, c)) {
                        possibleMoves[r][c] = 1;
                    }

                    if (board[r][c] * piece < 0) break;
                }
            }
            break;
        }
        case 6: { // King
            int moves[8][2] = {
                {-1, -1}, {-1, 1}, {1, -1}, {1, 1},
                {-1, 0}, {1, 0}, {0, -1}, {0, 1}
            };

            for (auto& move : moves) {
                int r = row + move[0], c = col + move[1];
                if (isValid(r, c) && board[r][c] * piece <= 0) {
                    if (IsValidMove(board, piece, row, col, r, c)) {
                        possibleMoves[r][c] = 1;
                    }
                }
            }

            // Castling moves
            bool isWhite = piece > 0;
            if (!(isWhite ? whiteKingMoved : blackKingMoved)) {
                // Kingside
                if (CanCastle(board, isWhite, true)) {
                    possibleMoves[row][col + 2] = 1;
                }
                // Queenside
                if (CanCastle(board, isWhite, false)) {
                    possibleMoves[row][col - 2] = 1;
                }
            }
            break;
        }
    }
}

void DrawValidMoves(int board[8][8], int piece, int row, int col) {
    int possibleMoves[8][8] = {0};

    HandlePieceMovement(board, row, col, possibleMoves);

    Color transparentGreen = {200, 200, 200, 128};
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (possibleMoves[r][c] == 1) {
                DrawRectangle(c * 62.5, r * 62.5, 62.5, 62.5, transparentGreen);
            }
        }
    }
}

bool IsKingInCheck(int board[8][8], bool isWhite) {
    int kingRow = -1, kingCol = -1;
    int kingPiece = isWhite ? 6 : -6;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (board[row][col] == kingPiece) {
                kingRow = row;
                kingCol = col;
                break;
            }
        }
        if (kingRow != -1) break;
    }

    if (kingRow == -1 || kingCol == -1) return false;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            int piece = board[row][col];
            if (piece != 0 && (piece * kingPiece < 0)) {
                if (IsValidMove(board, piece, row, col, kingRow, kingCol, false)) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool IsCheckmate(int board[8][8], bool isWhite) {
    if (!IsKingInCheck(board, isWhite)) return false;

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            int piece = board[row][col];
            if (piece != 0 && (piece > 0 && isWhite || piece < 0 && !isWhite)) {
                for (int targetRow = 0; targetRow < 8; targetRow++) {
                    for (int targetCol = 0; targetCol < 8; targetCol++) {
                        if (IsValidMove(board, piece, row, col, targetRow, targetCol)) {
                            int originalPiece = board[targetRow][targetCol];
                            board[targetRow][targetCol] = piece;
                            board[row][col] = 0;

                            bool stillInCheck = IsKingInCheck(board, isWhite);

                            board[row][col] = piece;
                            board[targetRow][targetCol] = originalPiece;

                            if (!stillInCheck) {
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }

    return true;
}

bool isWhiteTurn = true;  

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Chess Game");

    LoadPieceImages();

    bool gameOver = false;
    bool promotionPending = false;
    int promotionRow = -1;
    int promotionCol = -1;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawBoard(isWhiteTurn);
        DrawPieces(board);

        if (!gameOver) {
            if (promotionPending) {
                // Draw promotion menu (simplified - in a real game you'd show piece options)
                DrawText("Pawn promoted to Queen", 100, SCREEN_HEIGHT / 2 - 20, 20, BLACK);
                PromotePawn(board, promotionRow, promotionCol, isWhiteTurn);
                promotionPending = false;
            } else {
                Vector2 mousePos = GetMousePosition();
                int row = mousePos.y / 62.5;
                int col = mousePos.x / 62.5;

                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    if (selectedSquareRow == -1 && selectedSquareCol == -1) {
                        // Select a piece if it's the current player's turn
                        if ((isWhiteTurn && board[row][col] > 0) || (!isWhiteTurn && board[row][col] < 0)) {
                            selectedSquareRow = row;
                            selectedSquareCol = col;
                            pieceSelected = true;
                        }
                    } else {
                        // If clicking on another piece of the same color, select that piece instead
                        if (board[row][col] != 0 && board[row][col] * board[selectedSquareRow][selectedSquareCol] > 0) {
                            selectedSquareRow = row;
                            selectedSquareCol = col;
                        } else {
                            // Attempt to make a move
                            if (IsValidMove(board, board[selectedSquareRow][selectedSquareCol], selectedSquareRow, selectedSquareCol, row, col)) {
                                int piece = board[selectedSquareRow][selectedSquareCol];
                                bool isWhite = piece > 0;
                                
                                // Handle castling
                                if (abs(piece) == 6 && abs(selectedSquareCol - col) == 2) {
                                    bool kingside = col > selectedSquareCol;
                                    int rookCol = kingside ? 7 : 0;
                                    int newRookCol = kingside ? 5 : 3;
                                    
                                    // Move the rook
                                    board[row][newRookCol] = board[row][rookCol];
                                    board[row][rookCol] = 0;
                                }
                                
                                // Handle en passant
                                if (abs(piece) == 1 && col != selectedSquareCol && board[row][col] == 0) {
                                    // Capture the pawn that was bypassed
                                    board[selectedSquareRow][col] = 0;
                                }
                                
                                // Make the move
                                int originalPiece = board[row][col];
                                board[row][col] = piece;
                                board[selectedSquareRow][selectedSquareCol] = 0;
                                
                                // Set en passant target if pawn moved two squares
                                if (abs(piece) == 1 && abs(row - selectedSquareRow) == 2) {
                                    enPassantTargetRow = (row + selectedSquareRow) / 2;
                                    enPassantTargetCol = col;
                                } else {
                                    enPassantTargetRow = -1;
                                    enPassantTargetCol = -1;
                                }
                                
                                // Check for pawn promotion
                                if (abs(piece) == 1 && (row == 0 || row == 7)) {
                                    promotionPending = true;
                                    promotionRow = row;
                                    promotionCol = col;
                                }
                                
                                // Update castling flags
                                if (abs(piece) == 6) {
                                    if (isWhite) whiteKingMoved = true;
                                    else blackKingMoved = true;
                                }
                                if (abs(piece) == 4) {
                                    if (isWhite) {
                                        if (selectedSquareCol == 0) whiteRookQueensideMoved = true;
                                        if (selectedSquareCol == 7) whiteRookKingsideMoved = true;
                                    } else {
                                        if (selectedSquareCol == 0) blackRookQueensideMoved = true;
                                        if (selectedSquareCol == 7) blackRookKingsideMoved = true;
                                    }
                                }
                                
                                // Check if the move leaves the king in check
                                if (IsKingInCheck(board, isWhiteTurn)) {
                                    // Undo the move if it's illegal
                                    board[selectedSquareRow][selectedSquareCol] = piece;
                                    board[row][col] = originalPiece;
                                } else {
                                    // Valid move, switch turns
                                    isWhiteTurn = !isWhiteTurn;

                                    // Check for checkmate
                                    if (IsCheckmate(board, isWhiteTurn)) {
                                        gameOver = true;
                                    }
                                }
                            }
                            pieceSelected = false;
                            selectedSquareRow = -1;
                            selectedSquareCol = -1;
                        }
                    }
                }

                if (pieceSelected) {
                    DrawValidMoves(board, board[selectedSquareRow][selectedSquareCol], selectedSquareRow, selectedSquareCol);
                }
            }
        } else {
            DrawText("Checkmate! Game Over.", 100, SCREEN_HEIGHT / 2 - 20, 30, BLACK);
        }

        EndDrawing();
    }

    UnloadPieceImages();
    CloseWindow();
    return 0;
}
