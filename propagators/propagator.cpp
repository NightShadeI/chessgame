#include "propagator.hpp"
#include "../board.hpp"
#include "../threatTile.hpp"
#include "verticalPropagator.hpp"
#include "horizontalPropagator.hpp"
#include "positiveDiagonalPropagator.hpp"
#include "negativeDiagonalPropagator.hpp"

Propagator::Propagator(Piece* rootPiece, Move* movePerformed) {
    this->rootPiece = rootPiece;
    this->movePerformed = movePerformed;
}

void Propagator::openPropagation(Game& game) {
}

void Propagator::closePropagation(Game& game) {
}

void Propagator::updater(Game& game, int xStart, int yStart, int xDir, int yDir, bool threaten) {
    int curX = xStart, curY = yStart;
    while ((unsigned)curX < Board::width && (unsigned)curY < Board::height) {
        if (threaten) {
            game.addThreat(rootPiece, curX, curY);
        } else {
            // if no piece is removed then false is returned
            if (!game.removeThreat(rootPiece, curX, curY)) return;
        }
        // Piece blocks the way so we stop the propergation
        if (game.getPieceAt(curX, curY)) return;
        curX += xDir;
        curY += yDir;
    }
}

Propagator* Propagator::fetchPropagator(Piece* queryPiece, Move* queryMove, bool updateFrom) {
    int xRef = updateFrom ? queryMove->xFrom : queryMove->xTo;
    int yRef = updateFrom ? queryMove->yFrom : queryMove->yTo;
    if (xRef == queryPiece->xPos) {
        return new VerticalPropagator(queryPiece, queryMove);
    }
    if (yRef == queryPiece->yPos) {
        return new HorizontalPropagator (queryPiece, queryMove);
    }
    // We need a diagonal updater
    int xDistance = xRef - queryPiece->xPos;
    int yDistance = yRef - queryPiece->yPos;
    if (xDistance > 0 == yDistance > 0) {
        return new NegativeDiagonalPropagator (queryPiece, queryMove);  
    }
    return new PositiveDiagonalPropagator (queryPiece, queryMove); 
}