#include "knight.hpp"
#include "../board.hpp"
#include "../game.hpp"
#include "../move.hpp"
#include <string>

Knight::Knight(int x, int y, int ty) : Piece(x, y, ty) {
}

bool Knight::isValidMove(Game& game, int newX, int newY) {
    int xDiff = abs(newX - xPos);
    int yDiff = abs(newY - yPos);
    return xDiff * yDiff == 2;
}

vector<Move> Knight::getMoves(Game& game) {
    vector<Move> moves;
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
            moves.emplace_back(Move{this, game.getPieceAt(newX, newY), 0, 0, newX, newY});
        }
    }
    return moves;
}

bool Knight::isStartingLocation() {
    if (type == 1) {
        return yPos == 7 && (xPos == 1 || xPos == 6);
    }
    return yPos == 0 && (xPos == 1 || xPos == 6);
}