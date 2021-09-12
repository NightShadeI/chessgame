#include "verticalPropagator.hpp"

VerticalPropagator::VerticalPropagator(Piece* rootPiece, Move* movePerformed) : Propagator(rootPiece, movePerformed) {
}

void VerticalPropagator::openPropagation(Game& game) {
    int yDir = (2 * ((movePerformed->yFrom - rootPiece->yPos) > 0)) - 1;
    // A piece is moving out of my line of sight
    if (movePerformed->xFrom != movePerformed->xTo) {
        updater(game, movePerformed->xFrom, movePerformed->yFrom + yDir, 0, yDir, true);
        return;
    }
    // It must still be in the line of sight
    // If the distance has increased then there are new squares that are threatened
    if (movePerformed->yTo * yDir > movePerformed->yFrom * yDir) {
        updater(game, movePerformed->xFrom, movePerformed->yFrom + yDir, 0, yDir, true);
        return;
    }
    // If it is in the line of sight and the distance decreased, we don't do anything
}

void VerticalPropagator::closePropagation(Game& game) {
    int yDir = (2 * ((movePerformed->yTo - rootPiece->yPos) > 0)) - 1;
    // A piece is into my line of sight
    if (movePerformed->xFrom != movePerformed->xTo) {
        updater(game, movePerformed->xTo, movePerformed->yTo + yDir, 0, yDir, false);
        return;
    }
    // It must still be in the line of sight
    // If the distance has decreased then some positions are no longer threatened
    if (movePerformed->yTo * yDir < movePerformed->yFrom * yDir) {
        updater(game, movePerformed->xTo, movePerformed->yTo + yDir, 0, yDir, false);
        return;
    }
    // If it is in the line of sight and the distance increased, we don't do anything
}