#include "king.hpp"
#include "../board.hpp"
#include "../game.hpp"
#include "../move.hpp"
#include "../propagators/piecePropagators.hpp"
#include <string>
#include <cmath>

King::King(int x, int y, int ty) : Piece(x, y, ty) {
}

std::string King::getPieceName() {
    return "King";
}

PieceName King::getPieceType() {
    return PieceName::KING;
}

int King::getPieceValue() {
    return 1000;
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

void King::setup(Game& game) {
    PiecePropagators::kingSetup(game, this);
    game.addThreat(this, xPos, yPos);
}

void King::cleanThreats(Game& game) {
    PiecePropagators::kingSetup(game, this, true);
    game.removeThreat(this, xPos, yPos);
}

void King::updateThreats(Game& game, int newX, int newY, Piece* captured) {
    PiecePropagators::kingUpdater(game, this, newX, newY);
}