#include "king.hpp"
#include "../board.hpp"
#include "../game.hpp"
#include "../move.hpp"
#include <string>
#include <cmath>

King::King(int x, int y, int ty) : Piece(x, y, ty) {
}

bool King::isValidMove(Game& game, int newX, int newY) {
    int xDiff = abs(newX - xPos);
    int yDiff = abs(newY - yPos);
    return xDiff <= 1 && yDiff <= 1;
}

vector<unique_ptr<Move>> King::getMoves(Game& game) {
    vector<unique_ptr<Move>> moves;
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
        int newX = xPos + dir.first;
        int newY = yPos + dir.second;
        if ((unsigned)newX < Board::width && (unsigned)newY < Board::height) {
            Piece* pieceAt = game.getPieceAt(newX, newY);
            if (pieceAt && pieceAt->type == type) continue;
            moves.emplace_back(make_unique<Move>(Move{this, game.getPieceAt(newX, newY), 0, 0, newX, newY}));
        }
    }
    return moves;
}