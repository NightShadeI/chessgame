#include "knight.hpp"
#include "../board.hpp"
#include "../game.hpp"
#include "../move.hpp"
#include "../propagators/piecePropagators.hpp"
#include <string>

Knight::Knight(int x, int y, int ty) : Piece(x, y, ty) {
}

bool Knight::isValidMove(Game& game, int newX, int newY) {
    int xDiff = abs(newX - xPos);
    int yDiff = abs(newY - yPos);
    return xDiff * yDiff == 2;
}

vector<unique_ptr<Move>> Knight::getMoves(Game& game) {
    vector<unique_ptr<Move>> moves;
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
            unique_ptr<Move> move = make_unique<Move>();
            move->moved = this;
            move->captured = game.getPieceAt(newX, newY);
            move->xTo = newX;
            move->yTo = newY;
            moves.emplace_back(std::move(move));
        }
    }
    return moves;
}

void Knight::setup(Game& game) {
    PiecePropagators::knightSetup(game, this);
}

void Knight::cleanThreats(Game& game) {
    PiecePropagators::knightSetup(game, this, true);
}

void Knight::updateThreats(Game& game, int newX, int newY, Piece* captured) {
    PiecePropagators::knightUpdater(game, this, newX, newY);
}