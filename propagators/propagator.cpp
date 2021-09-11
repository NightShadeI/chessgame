#include "propagator.hpp"
#include "../board.hpp"
#include "../threatTile.hpp"
#include "verticalPropagator.hpp"
#include "horizontalPropagator.hpp"
#include "PositiveDiagonalPropagator.hpp"
#include "NegativeDiagonalPropagator.hpp"

Propagator::Propagator(Piece* rootPiece, Move* movePerformed) {
    this->rootPiece = rootPiece;
    this->movePerformed = movePerformed;
}

void Propagator::propagate(Game& game) {
}

void Propagator::updater(Game& game, int xStart, int yStart, int xDir, int yDir, bool threaten) {
    int curX = xStart, curY = yStart;
    while ((unsigned)curX < Board::width && (unsigned)curY < Board::height) {
        if (threaten) {
            game.addThreat(rootPiece, curX, curY);
        } else {
            game.removeThreat(rootPiece, curX, curY);
        }
        // Piece blocks the way so we stop the propergation
        if (game.getPieceAt(curX, curY)) return;
        curX += xDir;
        curY += yDir;
    }
}

Propagator Propagator::fetchPropagator(Piece* queryPiece, Move* queryMove, bool updateFrom) {
    int xRef = updateFrom ? queryMove->xFrom : queryMove->xTo;
    int yRef = updateFrom ? queryMove->yFrom : queryMove->yTo;
    if (xRef == queryPiece->xPos) {
        return VerticalPropagator(queryPiece, queryMove);
    }
    if (yRef == queryPiece->yPos) {
        return HorizontalPropagator(queryPiece, queryMove);
    }
    // We need a diagonal updater
    int xDistance = xRef - queryPiece->xPos;
    int yDistance = yRef - queryPiece->yPos;
    if (xDistance > 0 == yDistance > 0) {
        return PositiveDiagonalPropagator(queryPiece, queryMove);
    }
    return NegativeDiagonalPropagator(queryPiece, queryMove);
}