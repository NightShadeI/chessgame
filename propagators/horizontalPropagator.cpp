#include "horizontalPropagator.hpp"

HorizontalPropagator::HorizontalPropagator(Piece* rootPiece, Move* movePerformed) : Propagator(rootPiece, movePerformed) {
}

void HorizontalPropagator::openPropagation(Game& game) {
    int xDir = (2 * ((movePerformed->xFrom - rootPiece->xPos) > 0)) - 1;
    // A piece is moving out of my line of sight
    if (movePerformed->yFrom != movePerformed->yTo) {
        updater(game, movePerformed->xFrom + xDir, movePerformed->yFrom, xDir, 0, true);
        return;
    }
    // It must still be in the line of sight
    // If the distance has increased then there are new squares that are threatened
    if (movePerformed->xTo * xDir > movePerformed->xFrom * xDir) {
        updater(game, movePerformed->xFrom + xDir, movePerformed->yFrom, xDir, 0, true);
        return;
    }
    // If it is in the line of sight and the distance decreased, we don't do anything
}

void HorizontalPropagator::closePropagation(Game& game) {
    int xDir = (2 * ((movePerformed->xTo - rootPiece->xPos) > 0)) - 1;
    // A piece is into my line of sight
    if (movePerformed->yFrom != movePerformed->yTo) {
        updater(game, movePerformed->xTo + xDir, movePerformed->yTo, xDir, 0, false);
        return;
    }
    // It must still be in the line of sight
    // If the distance has decreased then some positions are no longer threatened
    if (movePerformed->xTo * xDir < movePerformed->xFrom * xDir) {
        updater(game, movePerformed->xTo + xDir, movePerformed->yTo, xDir, 0, false);
        return;
    }
    // If it is in the line of sight and the distance increased, we don't do anything
}