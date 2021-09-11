#include "../pieces/piece.hpp"
#include "../game.hpp"

namespace PiecePropagators {
    void knightUpdater(Game& game, Piece* knight, int newX, int newY);
    void bishopUpdater(Game& game, Piece* bishop, int newX, int newY);
    void rookUpdater(Game& game, Piece* rook, int newX, int newY);
    void queenUpdater(Game& game, Piece* queen, int newX, int newY);
    void kingUpdater(Game& game, Piece* king, int newX, int newY);
    void pawnUpdater(Game& game, Piece* pawn, int newX, int newY);
}