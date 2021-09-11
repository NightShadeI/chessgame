#include "../pieces/piece.hpp"
#include "../game.hpp"

namespace PiecePropagators {

    void knightSetup(Game& game, Piece* knight, bool destructure = false);
    void bishopSetup(Game& game, Piece* bishop, bool destructure = false);
    void rookSetup(Game& game, Piece* rook, bool destructure = false);
    void queenSetup(Game& game, Piece* queen, bool destructure = false);
    void kingSetup(Game& game, Piece* king, bool destructure = false);
    void pawnSetup(Game& game, Piece* pawn, bool destructure = false);

    void knightUpdater(Game& game, Piece* knight, int newX, int newY);
    void bishopUpdater(Game& game, Piece* bishop, int newX, int newY);
    void rookUpdater(Game& game, Piece* rook, int newX, int newY);
    void queenUpdater(Game& game, Piece* queen, int newX, int newY);
    void kingUpdater(Game& game, Piece* king, int newX, int newY);
    void pawnUpdater(Game& game, Piece* pawn, int newX, int newY);

}