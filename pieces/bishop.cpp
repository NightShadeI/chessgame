#include "bishop.hpp"
#include "../board.hpp"
#include "../game.hpp"
#include "../move.hpp"
#include "../propagators/piecePropagators.hpp"
#include <string>
#include <vector>
#include <utility>
#include <cmath>


Bishop::Bishop(int x, int y, int ty) : Piece(x, y, ty) {
}

int Bishop::getXTranslation() {
    return 8;
}

std::string Bishop::getPieceName() {
    return "Bishop";
}

int Bishop::getPieceValue() {
    return 30;
}

bool Bishop::isValidMove(Game& game, int newX, int newY) {
    int xDiff = abs(newX - xPos);
    int yDiff = abs(newY - yPos);
    return xDiff == yDiff;
}

std::vector<Move*> Bishop::getMoves(Game& game) {
    std::vector<Move*> moves;
    const std::vector<std::pair<int, int>> dirs = {
        {-1,  1},
        { 1,  1},
        { 1, -1},
        {-1, -1}
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

void Bishop::setup(Game& game) {
    PiecePropagators::bishopSetup(game, this);
}

void Bishop::updateThreats(Game& game, int newX, int newY) {
    PiecePropagators::bishopUpdater(game, this, newX, newY);
}