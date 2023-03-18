#include <string>

#include "rook.hpp"
#include "../board.hpp"
#include "../game.hpp"
#include "../move.hpp"

Rook::Rook(int x, int y, int ty) : Piece(x, y, ty) {
}

bool Rook::isValidMove(Game& game, int newX, int newY) {
    int xDiff = newX - xPos;
    int yDiff = newY - yPos;
    return xDiff * yDiff == 0;
}

vector<Move> Rook::getMoves(Game& game) {
    vector<Move> moves;
    const std::vector<std::pair<int, int>> dirs = {
        { 0,  1},
        { 1,  0},
        { 0, -1},
        {-1,  0}
    };
    for (const std::pair<int, int>& dir : dirs) {
        int curX = xPos, curY = yPos;
        while (curX += dir.first, curY += dir.second, (unsigned)curX < Board::width && (unsigned)curY < Board::height) {
            Piece* pieceAt = game.getPieceAt(curX, curY);
            if (pieceAt && pieceAt->type == type) break;
            moves.emplace_back(Move{this, game.getPieceAt(curX, curY), 0, 0, curX, curY});
            if (pieceAt) break;
        }
    }
    return moves;
}

bool Rook::isStartingLocation() {
    if (type == 1) {
        return yPos == 7 && (xPos == 0 || xPos == 7);
    }
    return yPos == 0 && (xPos == 0 || xPos == 7);
}