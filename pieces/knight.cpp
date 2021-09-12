#include "knight.hpp"
#include "../board.hpp"
#include "../game.hpp"
#include "../move.hpp"
#include "../propagators/piecePropagators.hpp"
#include <string>

Knight::Knight(int x, int y, int ty) : Piece(x, y, ty) {
}

std::string Knight::getPieceName() {
    return "Knight";
}

int Knight::getPieceValue() {
    return 30;
}

bool Knight::isValidMove(Game& game, int newX, int newY) {
    int xDiff = abs(newX - xPos);
    int yDiff = abs(newY - yPos);
    return xDiff * yDiff == 2;
}

std::vector<Move*> Knight::getMoves(Game& game) {
    std::vector<Move*> moves;
    const std::vector<std::pair<int, int>> dirs = {
        {-2,  1},
        {-1,  2},
        { 1,  2},
        { 2,  1},
        { 2, -1},
        { 1, -2},
        {-1, -2},
        {-2, -1}
    };
    for (const std::pair<int, int>& dir : dirs) {
        int newX = xPos + dir.first;
        int newY = yPos + dir.second;
        if ((unsigned)newX < Board::width && (unsigned)newY < Board::height) {
            Piece* pieceAt = game.getPieceAt(newX, newY);
            if (pieceAt && pieceAt->type == type) continue;
            Move* move = new Move;
            move->moved = this;
            move->captured = game.getPieceAt(newX, newY);
            move->xTo = newX;
            move->yTo = newY;
            moves.emplace_back(move);
        }
    }
    return moves;
}

void Knight::setup(Game& game) {
    PiecePropagators::knightSetup(game, this);
}

void Knight::updateThreats(Game& game, int newX, int newY, Piece* captured) {
    PiecePropagators::knightUpdater(game, this, newX, newY);
}