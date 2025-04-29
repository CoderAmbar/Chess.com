#include "raylib.h"
#include <stdio.h>
#include <direct.h> 
#include <algorithm>  
#include <iostream>  
#include <cstring> // For memcpy
 
using namespace std;

// Screen-related constants
#define BOARD_WIDTH 500
#define BOARD_HEIGHT 500

// Game screens
typedef enum GameScreen {
    GAME_MENU,
    NEW_GAME,
    PREVIOUS_GAME,
    GAME_SETTINGS,
    THEME_SETTINGS,
    EXIT
} GameScreen;

// Chess piece textures
Texture2D whitePawn, whiteKnight, whiteBishop, whiteRook, whiteQueen, whiteKing;
Texture2D blackPawn, blackKnight, blackBishop, blackRook, blackQueen, blackKing;
Texture2D backgroundImage;
Texture2D boardTexture;
Texture2D menuBackground;

// Sounds
Sound moveSound, captureSound, checkSound, castleSound, checkmateSound, promotionSound;
Music menuMusic, gameMusic;

// Game state variables
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

int selectedSquareRow = -1;
int selectedSquareCol = -1;
bool isWhiteTurn = true;
int enPassantTargetRow = -1;
int enPassantTargetCol = -1;
bool whiteKingMoved = false;
bool whiteRookKingsideMoved = false;
bool whiteRookQueensideMoved = false;
bool blackKingMoved = false;
bool blackRookKingsideMoved = false;
bool blackRookQueensideMoved = false;
bool promotionActive = false;
int promotionRow, promotionCol;
bool isWhitePromoting;
bool gameOver = false;
bool promotionPending = false;

// Settings
bool highlightLegalMoves = true;
float musicVolume = 1.0f;
float soundVolume = 1.0f;

// Function declarations
void ResetGame();
void DrawChessBoard(bool isWhiteTurn);
void DrawPieces(int board[8][8]);
void LoadResources();
void UnloadResources();
void HandlePieceMovement(int board[8][8], int row, int col, int possibleMoves[8][8]);
void DrawValidMoves(int board[8][8], int piece, int row, int col);
bool IsValidMove(int board[8][8], int piece, int startRow, int startCol, int endRow, int endCol, bool validateCheck = true);
bool IsKingInCheck(int board[8][8], bool isWhite);
bool IsCheckmate(int board[8][8], bool isWhite);
bool WouldBeInCheck(int board[8][8], int piece, int startRow, int startCol, int endRow, int endCol, bool isWhite);
bool CanCastle(int board[8][8], bool isWhite, bool kingside);
void DrawPromotionMenu();
void UpdateGame();
void DrawGame();

// Helper functions
float Clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void ResetGame() {
    // Reset board to starting position
    int newBoard[8][8] = {
        {-4, -2, -3, -5, -6, -3, -2, -4},
        {-1, -1, -1, -1, -1, -1, -1, -1},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1},
        {4, 2, 3, 5, 6, 3, 2, 4}
    };
    memcpy(board, newBoard, sizeof(newBoard));

    // Reset game state
    selectedSquareRow = -1;
    selectedSquareCol = -1;
    isWhiteTurn = true;
    enPassantTargetRow = -1;
    enPassantTargetCol = -1;
    whiteKingMoved = false;
    whiteRookKingsideMoved = false;
    whiteRookQueensideMoved = false;
    blackKingMoved = false;
    blackRookKingsideMoved = false;
    blackRookQueensideMoved = false;
    promotionActive = false;
    gameOver = false;
    promotionPending = false;
}

void LoadResources() {
    // Load textures
    whitePawn = LoadTexture("assets/white_pawn.png");
    whiteKnight = LoadTexture("assets/white_knight.png");
    whiteBishop = LoadTexture("assets/white_bishop.png");
    whiteRook = LoadTexture("assets/white_rook.png");
    whiteQueen = LoadTexture("assets/white_queen.png");
    whiteKing = LoadTexture("assets/white_king.png");

    blackPawn = LoadTexture("assets/black_pawn.png");
    blackKnight = LoadTexture("assets/black_knight.png");
    blackBishop = LoadTexture("assets/black_bishop.png");
    blackRook = LoadTexture("assets/black_rook.png");
    blackQueen = LoadTexture("assets/black_queen.png");
    blackKing = LoadTexture("assets/black_king.png");

    backgroundImage = LoadTexture("assets/chess_background.png");
    boardTexture = LoadTexture("assets/chessboard.png");
    menuBackground = LoadTexture("assets/menuBackground.png");

    // Load sounds
    InitAudioDevice();
    
    moveSound = LoadSound("assets/moveSound.mp3");
    captureSound = LoadSound("assets/captureSound.mp3");
    checkSound = LoadSound("assets/checkSound.mp3");
    castleSound = LoadSound("assets/castleSound.mp3");
    checkmateSound = LoadSound("assets/checkmateSound.mp3");
    promotionSound = LoadSound("assets/promotionSound.mp3");
    
    menuMusic = LoadMusicStream("assets/menuMusic.mp3");
    gameMusic = LoadMusicStream("assets/gameMusic.mp3");
    
    // Set initial volumes
    SetSoundVolume(moveSound, soundVolume);
    SetSoundVolume(captureSound, soundVolume);
    SetSoundVolume(checkSound, soundVolume);
    SetSoundVolume(castleSound, soundVolume);
    SetSoundVolume(checkmateSound, soundVolume);
    SetSoundVolume(promotionSound, soundVolume);
    
    SetMusicVolume(menuMusic, musicVolume);
    SetMusicVolume(gameMusic, musicVolume);
}

void UnloadResources() {
    // Unload textures
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
    UnloadTexture(backgroundImage);
    UnloadTexture(boardTexture);
    UnloadTexture(menuBackground);

    // Unload sounds
    UnloadSound(moveSound);
    UnloadSound(captureSound);
    UnloadSound(checkSound);
    UnloadSound(castleSound);
    UnloadSound(checkmateSound);
    UnloadSound(promotionSound);
    
    UnloadMusicStream(menuMusic);
    UnloadMusicStream(gameMusic);
    
    CloseAudioDevice();
}

void DrawChessBoard(bool isWhiteTurn) {
    int boardOffsetX = (GetScreenWidth() - BOARD_WIDTH) / 2;
    int boardOffsetY = (GetScreenHeight() - BOARD_HEIGHT) / 2;
    
    if (boardTexture.id != 0) {
        DrawTexturePro(
            boardTexture,
            (Rectangle){0, 0, (float)boardTexture.width, (float)boardTexture.height},
            (Rectangle){
                (float)boardOffsetX,
                (float)boardOffsetY,
                (float)BOARD_WIDTH,
                (float)BOARD_HEIGHT
            },
            (Vector2){0, 0},
            0,
            WHITE
        );
    } else {
        // Fallback: Draw colored squares
        for (int row = 0; row < 8; row++) {
            for (int col = 0; col < 8; col++) {
                Color color = ((row + col) % 2 == 0) ? BEIGE : DARKBROWN;
                DrawRectangle(
                    col * 62.5f + boardOffsetX,
                    row * 62.5f + boardOffsetY,
                    62.5f, 62.5f, color
                );
            }
        }
    }

    // Highlight king in check
    if (IsKingInCheck(board, isWhiteTurn)) {
        int kingRow = -1, kingCol = -1;
        int kingPiece = isWhiteTurn ? 6 : -6;

        // Find king position
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
            DrawRectangle(kingCol * 62.5 + boardOffsetX, kingRow * 62.5 + boardOffsetY, 
                          62.5, 62.5, ColorAlpha(RED, 0.5f));
        }
    }
}

void DrawPieces(int board[8][8]) {
    const float squareSize = BOARD_WIDTH / 8.0f;
    int boardOffsetX = (GetScreenWidth() - BOARD_WIDTH) / 2;
    int boardOffsetY = (GetScreenHeight() - BOARD_HEIGHT) / 2;
    
    BeginBlendMode(BLEND_ALPHA);

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
                float x = boardOffsetX + col * squareSize + (squareSize - pieceTexture->width * 0.45f) / 2;
                float y = boardOffsetY + row * squareSize + (squareSize - pieceTexture->height * 0.45f) / 2;
                
                float scale = 0.45f;
                
                DrawTextureEx(*pieceTexture, (Vector2){x, y}, 0.0f, scale, WHITE);
            }
        }
    }
    
    EndBlendMode();
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
        
        // Move king to this square
        tempBoard[row][col] = isWhite ? 6 : -6;
        tempBoard[row][kingCol] = 0;
        
        if (IsKingInCheck(tempBoard, isWhite)) {
            return false;
        }
    }
    
    return true;
}

void DrawPromotionMenu() {
    int boardOffsetX = (GetScreenWidth() - BOARD_WIDTH) / 2;
    int boardOffsetY = (GetScreenHeight() - BOARD_HEIGHT) / 2;
    
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){ 0, 0, 0, 128 });
    
    // Draw menu box
    float menuWidth = 250;
    float menuHeight = 80;
    float menuX = boardOffsetX + (BOARD_WIDTH - menuWidth) / 2;
    float menuY = boardOffsetY + (BOARD_HEIGHT - menuHeight - 10) / 2;
    DrawRectangle(menuX, menuY, menuWidth, menuHeight, LIGHTGRAY);
    
    Texture2D* pieces[4] = {
        isWhitePromoting ? &whiteQueen : &blackQueen,
        isWhitePromoting ? &whiteRook : &blackRook,
        isWhitePromoting ? &whiteBishop : &blackBishop,
        isWhitePromoting ? &whiteKnight : &blackKnight
    };
    
    const char* labels[4] = { "Q", "R", "B", "K" };
    
    for (int i = 0; i < 4; i++) {
        float btnX = menuX + 20 + i * 55;
        float btnY = menuY + 20;
        
        Vector2 mousePos = GetMousePosition();
        Color btnColor = CheckCollisionPointRec(mousePos, (Rectangle){btnX, btnY, 50, 50}) ? LIGHTGRAY : WHITE;
        DrawRectangle(btnX, btnY, 50, 50, btnColor);
        DrawTextureEx(*pieces[i], (Vector2){ btnX, btnY}, 0, 0.35f, WHITE);
        DrawText(labels[i], btnX, btnY, 10, BLACK);

        if (btnColor.g == LIGHTGRAY.g) { 
            DrawRectangleLines(btnX, btnY, 50, 50, GOLD);
        }
    }
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
    int boardOffsetX = (GetScreenWidth() - BOARD_WIDTH) / 2;
    int boardOffsetY = (GetScreenHeight() - BOARD_HEIGHT) / 2;
    HandlePieceMovement(board, row, col, possibleMoves);

    Color transparentGreen = {200, 200, 200, 128};
    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
            if (possibleMoves[r][c] == 1) {
                DrawRectangle(c * 62.5 + boardOffsetX, r * 62.5 + boardOffsetY, 62.5, 62.5, transparentGreen);
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
    if (!IsKingInCheck(board, isWhite)) {
        return false;
    }

    for (int startRow = 0; startRow < 8; startRow++) {
        for (int startCol = 0; startCol < 8; startCol++) {
            int piece = board[startRow][startCol];
            
            if (piece != 0 && ((isWhite && piece > 0) || (!isWhite && piece < 0))) {
                
                for (int endRow = 0; endRow < 8; endRow++) {
                    for (int endCol = 0; endCol < 8; endCol++) {
                        
                        int capturedPiece = board[endRow][endCol];
                        board[endRow][endCol] = piece;
                        board[startRow][startCol] = 0;
                        
                        bool stillInCheck = IsKingInCheck(board, isWhite);
                        
                        board[startRow][startCol] = piece;
                        board[endRow][endCol] = capturedPiece;
                        
                        if (!stillInCheck) {
                            return false; 
                        }
                    }
                }
            }
        }
    }
    
    return true; 
}

void UpdateGame() {
    int boardOffsetX = (GetScreenWidth() - BOARD_WIDTH) / 2;
    int boardOffsetY = (GetScreenHeight() - BOARD_HEIGHT) / 2;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        
        if (mousePos.x >= boardOffsetX && mousePos.x < boardOffsetX + BOARD_WIDTH &&
            mousePos.y >= boardOffsetY && mousePos.y < boardOffsetY + BOARD_HEIGHT) {
            
            int row = (mousePos.y - boardOffsetY) / 62.5;
            int col = (mousePos.x - boardOffsetX) / 62.5;

            // If no piece is selected, try to select one
            if (selectedSquareRow == -1 && selectedSquareCol == -1) {
                if ((isWhiteTurn && board[row][col] > 0) || (!isWhiteTurn && board[row][col] < 0)) {
                    selectedSquareRow = row;
                    selectedSquareCol = col;
                }
            }
            // If we have a selected piece, try to move it
            else {
                // Check if we're clicking on another piece of the same color
                if (board[row][col] != 0 && 
                    board[row][col] * board[selectedSquareRow][selectedSquareCol] > 0) {
                    // Select the new piece instead
                    selectedSquareRow = row;
                    selectedSquareCol = col;
                } 
                else {
                    // Attempt to make a move
                    if (IsValidMove(board, board[selectedSquareRow][selectedSquareCol], 
                                  selectedSquareRow, selectedSquareCol, row, col)) {
                        int piece = board[selectedSquareRow][selectedSquareCol];
                        bool isWhite = piece > 0;
                        
                        // Handle castling
                        if (abs(piece) == 6 && abs(selectedSquareCol - col) == 2) {
                            bool kingside = col > selectedSquareCol;
                            int rookCol = kingside ? 7 : 0;
                            int newRookCol = kingside ? 5 : 3;
                            
                            board[row][newRookCol] = board[row][rookCol];
                            board[row][rookCol] = 0;
                        }
                        
                        // Handle en passant
                        if (abs(piece) == 1 && col != selectedSquareCol && board[row][col] == 0) {
                            board[selectedSquareRow][col] = 0;
                        }
                        
                        // Make the move
                        int originalPiece = board[row][col];
                        board[row][col] = piece;
                        board[selectedSquareRow][selectedSquareCol] = 0;
                        
                        // Set en passant target
                        if (abs(piece) == 1 && abs(row - selectedSquareRow) == 2) {
                            enPassantTargetRow = (row + selectedSquareRow) / 2;
                            enPassantTargetCol = col;
                        } else {
                            enPassantTargetRow = -1;
                            enPassantTargetCol = -1;
                        }
                        
                        // Play sounds
                        if (originalPiece == 0) {
                            PlaySound(abs(piece) == 6 && abs(selectedSquareCol - col) == 2 ? castleSound : moveSound);
                        } else {
                            PlaySound(captureSound);
                        }
                        
                        // Check for check/checkmate
                        if (IsKingInCheck(board, !isWhiteTurn)) {
                            PlaySound(checkSound);
                            if (IsCheckmate(board, !isWhiteTurn)) {
                                PlaySound(checkmateSound);
                                gameOver = true;
                            }
                        }
                        
                        // Check for pawn promotion
                        if (abs(piece) == 1 && (row == 0 || row == 7)) {
                            promotionPending = true;
                            promotionRow = row;
                            promotionCol = col;
                            isWhitePromoting = isWhiteTurn;
                        }
                        
                        // Update castling flags
                        if (abs(piece) == 6) {
                            isWhite ? whiteKingMoved = true : blackKingMoved = true;
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
                        
                        // Validate move doesn't leave king in check
                        if (IsKingInCheck(board, isWhiteTurn)) {
                            board[selectedSquareRow][selectedSquareCol] = piece;
                            board[row][col] = originalPiece;
                        } else {
                            isWhiteTurn = !isWhiteTurn;
                        }
                    }
                    selectedSquareRow = -1;
                    selectedSquareCol = -1;
                }
            }
        }
    }

    // Handle promotion selection
if (promotionPending && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    int boardOffsetX = (GetScreenWidth() - BOARD_WIDTH) / 2;
    int boardOffsetY = (GetScreenHeight() - BOARD_HEIGHT) / 2;
    
    float menuWidth = 250;
    float menuHeight = 80;
    float menuX = boardOffsetX + (BOARD_WIDTH - menuWidth) / 2;
    float menuY = boardOffsetY + (BOARD_HEIGHT - menuHeight - 10) / 2;
    
    Vector2 mousePos = GetMousePosition();
    
    for (int i = 0; i < 4; i++) {
        float btnX = menuX + 20 + i * 55;
        float btnY = menuY + 20;
        
        if (CheckCollisionPointRec(mousePos, (Rectangle){btnX, btnY, 50, 50})) {
            board[promotionRow][promotionCol] = isWhitePromoting ? (5 - i) : -(5 - i);
            promotionPending = false;
            PlaySound(promotionSound);
            
            isWhiteTurn = !isWhiteTurn;
            
                if (IsKingInCheck(board, !isWhitePromoting)) {
                    PlaySound(checkSound);
                    if (IsCheckmate(board, !isWhitePromoting)) {
                        PlaySound(checkmateSound);
                        gameOver = true;
                    }
                }
                break;
            }
        }
    }
}

void DrawGame() {
    DrawTexturePro(backgroundImage, 
        (Rectangle){0, 0, (float)backgroundImage.width, (float)backgroundImage.height},
        (Rectangle){0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()},
        (Vector2){0, 0}, 0, WHITE);

    DrawChessBoard(isWhiteTurn);
    DrawPieces(board);

    if (selectedSquareRow != -1 && selectedSquareCol != -1) {
        DrawValidMoves(board, board[selectedSquareRow][selectedSquareCol], 
                      selectedSquareRow, selectedSquareCol);
    }

    if (promotionPending) {
        DrawPromotionMenu();
    }

    if (gameOver) {
        DrawText("Checkmate! Game Over.", GetScreenWidth()/2 - 150, GetScreenHeight()/2 - 20, 30, RED);
    }
}

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    
    InitWindow(screenWidth, screenHeight, "Chess Game");
    SetTargetFPS(60);
    
    LoadResources();
    
    GameScreen currentScreen = GAME_MENU;
    bool usingKeyboard = false;
    int selectedButton = 0;
    bool keyReleased = true;
    
    // Menu button dimensions
    float buttonWidth = screenWidth * 0.150f;
    float buttonHeight = screenHeight * 0.075f;
    float gap = screenHeight * 0.028f;
    float buttonX = screenWidth * 0.1f;
    float totalButtonStackHeight = (5 * buttonHeight) + (4 * gap);
    float titleSpace = screenHeight * 0.12f;
    float startY = ((screenHeight - totalButtonStackHeight) / 2) + titleSpace;
    
    PlayMusicStream(menuMusic);
    
    while (!WindowShouldClose()) {
        UpdateMusicStream(menuMusic);
        UpdateMusicStream(gameMusic);
        
        if (IsKeyPressed(KEY_BACKSPACE) && currentScreen != GAME_MENU) {
            PlaySound(moveSound);
            currentScreen = GAME_MENU;
            StopMusicStream(gameMusic);
            PlayMusicStream(menuMusic);
            continue;
        }

        switch (currentScreen) {
            case GAME_MENU: {
                const char* titleText = "C H E S S";
                
                Rectangle buttons[5];
                float buttonY[5];
                for (int i = 0; i < 5; i++) {
                    buttonY[i] = startY + i * (buttonHeight + gap);
                    buttons[i] = (Rectangle){
                        buttonX,
                        startY + i * (buttonHeight + gap),
                        buttonWidth,
                        buttonHeight
                    };
                }

                // Check if mouse moved
                Vector2 mousePos = GetMousePosition();
                static Vector2 prevMousePos = mousePos;
                if (mousePos.x != prevMousePos.x || mousePos.y != prevMousePos.y) {
                    usingKeyboard = false;
                }
                prevMousePos = mousePos;

                // Keyboard navigation
                if (IsKeyDown(KEY_DOWN) && keyReleased) {
                    selectedButton = (selectedButton + 1) % 5;
                    keyReleased = false;
                    PlaySound(moveSound);
                    usingKeyboard = true;
                }
                else if (IsKeyDown(KEY_UP) && keyReleased) {
                    selectedButton = (selectedButton - 1 + 5) % 5;
                    keyReleased = false;
                    PlaySound(moveSound);
                    usingKeyboard = true;
                }
                else if (!IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN)) {
                    keyReleased = true;
                }

                // Handle selection with Enter key
                if (IsKeyPressed(KEY_ENTER)) {
                    PlaySound(moveSound);
                    switch (selectedButton) {
                        case 0: 
                            currentScreen = NEW_GAME; 
                            ResetGame();
                            StopMusicStream(menuMusic);
                            PlayMusicStream(gameMusic);
                            break;
                        case 1: currentScreen = PREVIOUS_GAME; break;
                        case 2: currentScreen = THEME_SETTINGS; break;
                        case 3: currentScreen = GAME_SETTINGS; break;
                        case 4: currentScreen = EXIT; break;
                    }
                }

                BeginDrawing();
                ClearBackground(BLACK);  
                DrawTexturePro(
                    menuBackground,
                    (Rectangle){ 0, 0, (float)menuBackground.width, (float)menuBackground.height },
                    (Rectangle){ 0, 0, (float)screenWidth, (float)screenHeight },
                    (Vector2){ 0, 0 },
                    0.0f,
                    WHITE
                );
                
                DrawTextEx(GetFontDefault(), "CHESS", (Vector2){ (screenWidth * 0.1f), startY - titleSpace - 100}, 100, 10, WHITE);

                const char* buttonTexts[5] = {
                    "New Game",
                    "Previous Game",
                    "Theme Settings",
                    "Game Settings",
                    "Exit"
                };

                for (int i = 0; i < 5; i++) {
                    bool isSelected = (i == selectedButton) && usingKeyboard;
                    bool isHovered = CheckCollisionPointRec(GetMousePosition(), buttons[i]) && !usingKeyboard;
                
                    DrawRectangleRec(buttons[i], ColorAlpha(BLANK, 0.0f));
                
                    if (isSelected || isHovered) {
                        DrawRectangleLinesEx(buttons[i], 2.5f, WHITE);
                        
                        int textX = buttonX + 20;
                        int textY = buttonY[i] + buttonHeight / 4;
                        const char* text = buttonTexts[i];
                        int fontSize = 20;
                
                        DrawText(text, textX - 1, textY, fontSize, WHITE);
                        DrawText(text, textX + 1, textY, fontSize, WHITE);
                        DrawText(text, textX, textY - 1, fontSize, WHITE);
                        DrawText(text, textX, textY + 1, fontSize, WHITE);
                        
                        DrawText(text, textX, textY, fontSize, BLACK);
                    } else {
                        DrawText(buttonTexts[i], buttonX + 20, buttonY[i] + buttonHeight / 4, 20, WHITE);
                    }
                }

                // Mouse click handling
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    for (int i = 0; i < 5; i++) {
                        if (CheckCollisionPointRec(GetMousePosition(), buttons[i])) {
                            PlaySound(moveSound);
                            selectedButton = i;
                            usingKeyboard = false;
                            switch (i) {
                                case 0: 
                                    currentScreen = NEW_GAME; 
                                    ResetGame();
                                    StopMusicStream(menuMusic);
                                    PlayMusicStream(gameMusic);
                                    break;
                                case 1: currentScreen = PREVIOUS_GAME; break;
                                case 2: currentScreen = THEME_SETTINGS; break;
                                case 3: currentScreen = GAME_SETTINGS; break;
                                case 4: currentScreen = EXIT; break;
                            }
                            break;
                        }
                    }
                }
                EndDrawing();
                break;
            }

            case NEW_GAME: {
                UpdateGame();
                
                BeginDrawing();
                DrawGame();
                EndDrawing();
                break;
            }

            case PREVIOUS_GAME: {
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("Previous Game Screen", 100, 100, 40, WHITE);
                EndDrawing();
                break;
            }

            case GAME_SETTINGS: {
                BeginDrawing();
                ClearBackground(BLACK);
                
                DrawTexturePro(
                    menuBackground,
                    (Rectangle){ 0, 0, (float)menuBackground.width, (float)menuBackground.height },
                    (Rectangle){ 0, 0, (float)screenWidth, (float)screenHeight },
                    (Vector2){ 0, 0 },
                    0.0f,
                    WHITE
                );
            
                DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.7f));
            
                DrawTextEx(GetFontDefault(), "Game Settings", (Vector2){screenWidth/2.0f - MeasureTextEx(GetFontDefault(), "Game Settings", 40, 1).x/2, 50}, 40, 1, WHITE);
            
                DrawText("Highlight Legal Moves:", 100, 120, 20, WHITE);
                Rectangle highlightToggle = { 350, 120, 50, 25 };
                DrawRectangleRec(highlightToggle, highlightLegalMoves ? GREEN : RED);
                DrawText(highlightLegalMoves ? "ON" : "OFF", 355, 122, 20, WHITE);
            
                if (CheckCollisionPointRec(GetMousePosition(), highlightToggle)) {
                    DrawRectangleLinesEx(highlightToggle, 2, GOLD);
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        highlightLegalMoves = !highlightLegalMoves;
                        PlaySound(moveSound);
                    }
                }
            
                DrawText("Music Volume:", 100, 170, 20, WHITE);
                Rectangle musicSliderBar = { 350, 175, 200, 20 };
                Rectangle musicSliderKnob = { 350 + (musicVolume * 200) - 5, 170, 10, 30 };
            
                DrawRectangleRec(musicSliderBar, ColorAlpha(DARKGRAY, 0.7f));
                DrawRectangleRec(musicSliderKnob, BLUE);
            
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && 
                    CheckCollisionPointRec(GetMousePosition(), (Rectangle){musicSliderBar.x, musicSliderBar.y-10, musicSliderBar.width, 40})) {
                    musicVolume = (GetMouseX() - musicSliderBar.x) / musicSliderBar.width;
                    musicVolume = Clamp(musicVolume, 0.0f, 1.0f);
                    SetMusicVolume(menuMusic, musicVolume);
                    SetMusicVolume(gameMusic, musicVolume);
                }
            
                DrawText("Sound Volume:", 100, 220, 20, WHITE);
                Rectangle soundSliderBar = { 350, 225, 200, 20 };
                Rectangle soundSliderKnob = { 350 + (soundVolume * 200) - 5, 220, 10, 30 };
            
                DrawRectangleRec(soundSliderBar, ColorAlpha(DARKGRAY, 0.7f));
                DrawRectangleRec(soundSliderKnob, BLUE);
        
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && 
                    CheckCollisionPointRec(GetMousePosition(), (Rectangle){soundSliderBar.x, soundSliderBar.y-10, soundSliderBar.width, 40})) {
                    soundVolume = (GetMouseX() - soundSliderBar.x) / soundSliderBar.width;
                    soundVolume = Clamp(soundVolume, 0.0f, 1.0f);
                    SetSoundVolume(moveSound, soundVolume);
                    SetSoundVolume(captureSound, soundVolume);
                    SetSoundVolume(checkSound, soundVolume);
                    SetSoundVolume(castleSound, soundVolume);
                    SetSoundVolume(checkmateSound, soundVolume);
                    SetSoundVolume(promotionSound, soundVolume);
                }
            
                // Back button
                Rectangle backButton = { screenWidth/2.0f - 100.0f, 500.0f, 200.0f, 50.0f };
                bool isBackButtonHovered = CheckCollisionPointRec(GetMousePosition(), backButton);

                // Draw button text with hover effect
                const char* backText = "Back";
                float textX = backButton.x + 80.0f;
                float textY = backButton.y + 15.0f;
                int fontSize = 20;

                if (isBackButtonHovered) {
                    DrawText(backText, textX - 1, textY, fontSize, WHITE);
                    DrawText(backText, textX + 1, textY, fontSize, WHITE);
                    DrawText(backText, textX, textY - 1, fontSize, WHITE);
                    DrawText(backText, textX, textY + 1, fontSize, WHITE);
                    DrawText(backText, textX, textY, fontSize, BLACK);
                } else {
                    DrawText(backText, textX, textY, fontSize, WHITE);
                }

                // Handle click
                if (isBackButtonHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    currentScreen = GAME_MENU;
                    PlaySound(moveSound);
                }
                                
                EndDrawing();
                break;
            }

            case THEME_SETTINGS: {
                BeginDrawing();
                ClearBackground(BLACK);
                DrawText("Theme Settings Screen", 100, 100, 40, WHITE);
                EndDrawing();
                break;
            }

            case EXIT:
                CloseWindow();
                break;
        }

        if (currentScreen == EXIT) break;
    }

    UnloadResources();
    CloseWindow();

    return 0;
}
