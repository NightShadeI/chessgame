#include "horizontalPropagator.hpp"

HorizontalPropagator::HorizontalPropagator(Piece* rootPiece, Move* movePerformed) : Propagator(rootPiece, movePerformed) {

}

void HorizontalPropagator::propagate(Game& game) {
    int xDir = (2 * movePerformed->xFrom - rootPiece->xPos > 0) - 1;
    // A piece is coming in front of me or moving out of my line
    if (movePerformed->yFrom != movePerformed->yTo) {
        updater(game, movePerformed->xFrom + xDir, movePerformed->yFrom, xDir, 0, rootPiece->yPos != movePerformed->yTo);
        return;
    }
    // It must still be in the line of sight
    // If the distance has increased then there are new squares that are threatened
    if (movePerformed->xTo * xDir > movePerformed->xFrom * xDir) {
        updater(game, movePerformed->xFrom + xDir, movePerformed->yFrom, xDir, 0, true);
        return;
    }
    // Distance towards me has decreased, there are now fewer squares that are threatened
    updater(game, movePerformed->xTo + xDir, movePerformed->yTo, xDir, 0, false);
}