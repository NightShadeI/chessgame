#include "bishop.hpp"
#include "../board.hpp"
#include "../game.hpp"
#include "../move.hpp"
#include <string>
#include <vector>
#include <utility>
#include <cmath>


Bishop::Bishop(int x, int y, int ty) : Piece(x, y, ty) {
}

bool Bishop::isValidMove(Game& game, int newX, int newY) {
    int xDiff = abs(newX - xPos);
    int yDiff = abs(newY - yPos);
    return xDiff == yDiff;
}

vector<unique_ptr<Move>> Bishop::getMoves(Game& game) {
    vector<unique_ptr<Move>> moves;
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
            moves.emplace_back(make_unique<Move>(Move{this, game.getPieceAt(curX, curY), 0, 0, curX, curY}));
            if (pieceAt) break;
        }
    }
    return moves;
}

bool Bishop::isStartingLocation() {
    if (type == 1) {
        return yPos == 7 && (xPos == 2 || xPos == 5);
    }
    return yPos == 0 && (xPos == 2 || xPos == 5);
}