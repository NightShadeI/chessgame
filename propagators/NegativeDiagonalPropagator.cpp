#include "negativeDiagonalPropagator.hpp"

NegativeDiagonalPropagator::NegativeDiagonalPropagator(Piece* rootPiece, Move* movePerformed) : Propagator(rootPiece, movePerformed) {
}

void NegativeDiagonalPropagator::openPropagation(Game& game) {
    int oldXDistance = movePerformed->xFrom - rootPiece->xPos;
    int oldYDistance = movePerformed->yFrom - rootPiece->yPos;
    int newXDistance = movePerformed->xTo - rootPiece->xPos;
    int newYDistance = movePerformed->yTo - rootPiece->yPos;
    int xDir = (2 * (oldXDistance > 0)) - 1;
    // Piece has either moved closer or further away
    if (oldXDistance == oldYDistance && newXDistance == newYDistance) {    
        // Piece has moved further away so we can add threats
        if (oldXDistance * xDir < newXDistance * xDir) {
            updater(game, movePerformed->xFrom + xDir, movePerformed->yFrom + xDir, xDir, xDir, true);
            return;
        }
        // Piece must have moved closer, no new threats to open
        return;
    }
    // Piece was blocking but has now moved out of the way
    updater(game, movePerformed->xFrom + xDir, movePerformed->yFrom + xDir, xDir, xDir, true);
}

void NegativeDiagonalPropagator::closePropagation(Game& game) {
    int oldXDistance = movePerformed->xFrom - rootPiece->xPos;
    int oldYDistance = movePerformed->yFrom - rootPiece->yPos;
    int newXDistance = movePerformed->xTo - rootPiece->xPos;
    int newYDistance = movePerformed->yTo - rootPiece->yPos;
    int xDir = (2 * (newXDistance > 0)) - 1;
    // Piece has either moved closer or further away
    if (oldXDistance == oldYDistance && newXDistance == newYDistance) {    
        // Piece has moved closer can remove threats
        if (oldXDistance * xDir > newXDistance * xDir) {
            updater(game, movePerformed->xTo + xDir, movePerformed->yTo + xDir, xDir, xDir, false);
            return;
        }
        // Piece must have moved further away, is not closing a position
        return;
    }
    // Piece is now blocking
    updater(game, movePerformed->xTo + xDir, movePerformed->yTo + xDir, xDir, xDir, false);
}