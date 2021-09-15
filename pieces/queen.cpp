#include "queen.hpp"
#include "../board.hpp"
#include "../game.hpp"
#include "../move.hpp"
#include "../propagators/piecePropagators.hpp"
#include <string>
#include <cmath>

Queen::Queen(int x, int y, int ty) : Piece(x, y, ty) {
}

std::string Queen::getPieceName() {
    return "Queen";
}

int Queen::getXTranslation() {
    return 5;
}

int Queen::getPieceValue() {
    return 90;
}

bool Queen::isSlidingPiece() {
    return true;
}

bool Queen::isValidMove(Game& game, int newX, int newY) {
    int xDiff = abs(newX - xPos);
    int yDiff = abs(newY - yPos);
    return xDiff == yDiff || xDiff * yDiff == 0;
}

std::vector<Move*> Queen::getMoves(Game& game) {
    std::vector<Move*> moves;
    const std::vector<std::pair<int, int>> dirs = {
        {-1,  1},
        { 0,  1},
        { 1,  1},
        { 1,  0},
        { 1, -1},
        { 0, -1},
        {-1, -1},
        {-1,  0}
    };
    for (const std::pair<int, int>& dir : dirs) {
        int curX = xPos, curY = yPos;
        while (curX += dir.first, curY += dir.second, (unsigned)curX < Board::width && (unsigned)curY < Board::height) {
            Piece* pieceAt = game.getPieceAt(curX, curY);
            if (pieceAt && pieceAt->type == type) break;
            Move* move = new Move;
            move->moved = this;
            move->captured = game.getPieceAt(curX, curY);
            move->xTo = curX;
            move->yTo = curY;
            moves.emplace_back(move);
            if (pieceAt) break;
        }
    }
    return moves;
}

void Queen::setup(Game& game) {
    PiecePropagators::queenSetup(game, this);
    game.addThreat(this, xPos, yPos);
}

void Queen::cleanThreats(Game& game) {
    PiecePropagators::queenSetup(game, this, true);
    game.removeThreat(this, xPos, yPos);
}

void Queen::updateThreats(Game& game, int newX, int newY, Piece* captured) {
    PiecePropagators::queenUpdater(game, this, newX, newY, captured);
}