#include "verticalPropagator.hpp"

VerticalPropagator::VerticalPropagator(Piece* rootPiece, Move* movePerformed) : Propagator(rootPiece, movePerformed) {

}

void VerticalPropagator::propagate(Game& game) {
    int yDir = 1 - (2 * movePerformed->yFrom - rootPiece->yPos > 0);
    // A piece is coming in front of me or moving out of my line
    if (movePerformed->xFrom != movePerformed->xTo) {
        updater(game, movePerformed->xFrom, movePerformed->yFrom + yDir, 0, yDir, rootPiece->xPos != movePerformed->xTo);
        return;
    }
    // It must still be in the line of sight
    // If the distance has increased then there are new squares that are threatened
    if (movePerformed->yTo * yDir > movePerformed->yFrom * yDir) {
        updater(game, movePerformed->xFrom, movePerformed->yFrom + yDir, 0, yDir, true);
        return;
    }
    // Distance towards me has decreased, there are now fewer squares that are threatened
    updater(game, movePerformed->xTo, movePerformed->yTo + yDir, 0, yDir, false);
}