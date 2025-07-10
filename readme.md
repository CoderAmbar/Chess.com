# Chess Game with Raylib

![Chess Game Screenshot](screenshot.png)

A complete chess implementation with all standard rules including pawn promotion, en passant, castling, and checkmate detection, built using Raylib for graphics.

## Features

### Complete Chess Rules Implementation
- **Piece Movements**:
  - Pawns (including double move from starting position)
  - Knights (L-shaped movement)
  - Bishops (diagonal movement)
  - Rooks (straight movement)
  - Queens (combination of rook and bishop)
  - Kings (one square in any direction)
  
- **Special Moves**:
  - Pawn promotion (auto-promotes to queen)
  - En passant captures
  - Castling (both kingside and queenside)
  - Check detection
  - Checkmate detection

### Visual Features
- Highlighted valid moves (transparent green squares)
- King in check indication (red highlight)
- Classic chess board colors (beige and dark brown)
- Piece sprites for all chess pieces

### Game Flow
- Turn-based gameplay (white moves first)
- Automatic game state tracking
- Illegal move prevention
- Game over detection

## Technical Implementation

### Board Representation
- 8x8 integer array representing the chess board
- Positive numbers for white pieces, negative for black
- Piece values:
  - 1/-1: Pawn
  - 2/-2: Knight
  - 3/-3: Bishop
  - 4/-4: Rook
  - 5/-5: Queen
  - 6/-6: King

### Key Algorithms
- **Move Validation**: `IsValidMove()` function handles all movement rules
- **Check Detection**: `IsKingInCheck()` scans board for attacking pieces
- **Checkmate Detection**: `IsCheckmate()` verifies no legal moves remain
- **Special Moves**:
  - `CanCastle()` validates castling conditions
  - `PromotePawn()` handles pawn promotion
  - En passant tracking via `enPassantTargetRow/Col`

### Game State Tracking
- Castling rights tracked via:
  - `whiteKingMoved`, `blackKingMoved`
  - `whiteRookKingsideMoved`, `whiteRookQueensideMoved`
  - `blackRookKingsideMoved`, `blackRookQueensideMoved`
- Current turn stored in `isWhiteTurn`
- Selection state in `selectedSquareRow`, `selectedSquareCol`, `pieceSelected`

## Requirements

- [Raylib](https://www.raylib.com/) (version 4.0 or later)
- C++ compiler (g++, MSVC, or compatible)
- Make (optional, for build automation)

## Installation & Compilation

### Windows
1. Install Raylib for Windows
2. Compile with:
g++ main.cpp -o chess.exe -lraylib -lopengl32 -lgdi32 -lwinmm


### Linux
1. Install Raylib development packages
2. Compile with:
g++ main.cpp -o chess -lraylib -lGL -lm -lpthread -ldl -lrt -lX11


### MacOS
1. Install Raylib via Homebrew: `brew install raylib`
2. Compile with:
g++ main.cpp -o chess -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL libraylib.a


## How to Play

### Basic Controls
- Left-click to select a piece
- Left-click on highlighted square to move
- Game automatically handles special moves:
- Pawn promotion (to queen)
- En passant captures
- Castling when conditions are met

### Special Move Details
1. **Castling**:
- Move king two squares toward rook
- Rook jumps to opposite side of king
- Conditions:
  - No pieces between
  - King and rook haven't moved
  - Not in check
  - Doesn't move through check

2. **En Passant**:
- Occurs when pawn moves two squares
- Opponent can capture as if it moved one square
- Must be done immediately after two-square move

3. **Pawn Promotion**:
- Automatically promotes to queen when reaching last rank
- (Future: Will implement promotion choice)

## Code Structure

### Key Functions
- `DrawBoard()`: Renders the chess board
- `DrawPieces()`: Draws all pieces using loaded textures
- `HandlePieceMovement()`: Calculates valid moves for selected piece
- `DrawValidMoves()`: Highlights possible moves
- `WouldBeInCheck()`: Simulates moves to check for safety

### Asset Management
- Piece images loaded from:
- `D:/Projects and Stuff/assets/` (white and black pieces)
- Textures unloaded on game exit

### Game Loop
1. Process input
2. Update game state
3. Render board and pieces
4. Handle special cases (promotion, checkmate)

## Future Improvements

### Planned Features
- [✔] Promotion choice UI (queen/rook/bishop/knight)
- [ ] Undo move functionality
- [ ] Game timer/clock
- [✔] Save/load game state
- [ ] AI opponent
- [ ] Move history display
- [ ] Animated piece movements
- [✔] Sound effects

### Code Refactoring
- [ ] Separate into multiple source files
- [ ] Implement proper chess notation
- [ ] Add unit tests
- [✔] Optimize move validation

## Credits

- Chess piece sprites: [Chess.com](https://www.chess.com)
- Raylib: [www.raylib.com](https://www.raylib.com/)
- Developed by: Ambar @CoderAmbar
- Conrtibutors: Sara Jain @SaraJain90

## License

MIT License - See [LICENSE](LICENSE) file for details
