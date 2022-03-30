#include <string>

#include "rook.hpp"
#include "../board.hpp"
#include "../game.hpp"
#include "../move.hpp"
#include "../propagators/piecePropagators.hpp"

Rook::Rook(int x, int y, int ty) : Piece(x, y, ty) {
}

bool Rook::isValidMove(Game& game, int newX, int newY) {
    int xDiff = newX - xPos;
    int yDiff = newY - yPos;
    return xDiff * yDiff == 0;
}

vector<unique_ptr<Move>> Rook::getMoves(Game& game) {
    vector<unique_ptr<Move>> moves;
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
            unique_ptr<Move> move = make_unique<Move>();
            move->moved = this;
            move->captured = game.getPieceAt(curX, curY);
            move->xTo = curX;
            move->yTo = curY;
            moves.emplace_back(std::move(move));
            if (pieceAt) break;
        }
    }
    return moves;
}

void Rook::setup(Game& game) {
    PiecePropagators::rookSetup(game, this);
    game.addThreat(this, xPos, yPos);
}

void Rook::cleanThreats(Game& game) {
    PiecePropagators::rookSetup(game, this, true);
    game.removeThreat(this, xPos, yPos);
}

void Rook::updateThreats(Game& game, int newX, int newY, Piece* captured) {
    PiecePropagators::rookUpdater(game, this, newX, newY, captured);
}