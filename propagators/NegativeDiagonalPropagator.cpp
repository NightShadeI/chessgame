#include "negativeDiagonalPropagator.hpp"

NegativeDiagonalPropagator::NegativeDiagonalPropagator(Piece* rootPiece, Move* movePerformed) : Propagator(rootPiece, movePerformed) {

}

void NegativeDiagonalPropagator::propagate(Game& game) {
    int oldXDistance = movePerformed->xFrom - rootPiece->xPos;
    int oldYDistance = movePerformed->yFrom - rootPiece->yPos;
    int newXDistance = movePerformed->xTo - rootPiece->xPos;
    int newYDistance = movePerformed->yTo - rootPiece->yPos;
    // Piece has either moved closer or further away
    if (-oldXDistance == oldYDistance && -newXDistance == newYDistance) {
        int xDir = (2 * movePerformed->xFrom - rootPiece->xPos > 0) - 1;
        // Piece has moved closer
        if (oldXDistance * xDir > newXDistance * xDir) {
            updater(game, movePerformed->xTo + xDir, movePerformed->yTo + xDir, xDir, xDir, false);
            return;
        }
        updater(game, movePerformed->xFrom + xDir, movePerformed->yFrom + xDir, xDir, xDir, true);
        return;
    }
    bool wasBlocking = -oldXDistance == oldYDistance;
    int xRef = wasBlocking ? movePerformed->xFrom : movePerformed->xTo;
    int yRef = wasBlocking ? movePerformed->yFrom : movePerformed->yTo;
    int xDir = (2 * xRef - rootPiece->xPos > 0) - 1;
    updater(game, xRef + xDir, yRef + xDir, xDir, xDir, wasBlocking);
}