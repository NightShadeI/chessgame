#include <string>
#include <cmath>

#include "pawn.hpp"
#include "../game.hpp"
#include "../board.hpp"
#include "../move.hpp"
#include "../propagators/piecePropagators.hpp"

Pawn::Pawn(int x, int y, int ty) : Piece(x, y, ty) {
}

bool Pawn::isValidMove(Game& game, int newX, int newY) {
    int xDiff = abs(newX - xPos);
    int yDiff = abs(newY - yPos);
    if (yDiff > 2 || xDiff > 1) return false;
    if (type > 0 != (yPos - newY) > 0) return false;
    Piece* p = game.getPieceAt(newX, newY);
    if (xDiff == 0 && p) return false;
    if (yDiff == 2) {
        if (xDiff > 0) return false;
        if (type == 1 && yPos == 6) return true;
        if (type == -1 && yPos == 1) return true;
        return false;
    }
    return xDiff == 0 || p;
}

inline unique_ptr<Move> Pawn::generateMove(Game& game, int newX, int newY) {
    unique_ptr<Move> move = make_unique<Move>();
    move->moved = this;
    move->captured = game.getPieceAt(newX, newY);
    move->xTo = newX;
    move->yTo = newY;
    return move;
}

vector<unique_ptr<Move>> Pawn::getMoves(Game& game) {
    vector<unique_ptr<Move>> moves;

    if (yPos == 0 || yPos == 7) return moves;

    Piece* pieceAt;

    // Squares diagonal the pawn
    if (xPos - 1 >= 0) {
        pieceAt = game.getPieceAt(xPos - 1, yPos - type);
        if (pieceAt && pieceAt->type != type) {
            moves.emplace_back(generateMove(game, xPos - 1, yPos - type));
        }
        
    }
    if (xPos + 1 < Board::width) {
        pieceAt = game.getPieceAt(xPos + 1, yPos - type);
        if (pieceAt && pieceAt->type != type) {
            moves.emplace_back(generateMove(game, xPos + 1, yPos - type));
        }
    }

    // Square ahead of the pawn
    pieceAt = game.getPieceAt(xPos, yPos - type);
    if (pieceAt) return moves;
    moves.emplace_back(generateMove(game, xPos, yPos - type));

    // Square twice the distance ahead
    if ((unsigned)(type + yPos - 1) < 6) return moves;
    pieceAt = game.getPieceAt(xPos, yPos - type * 2);
    if (pieceAt) return moves;
    moves.emplace_back(generateMove(game, xPos, yPos - type * 2));

    return moves;
}

void Pawn::setup(Game& game) {
    PiecePropagators::pawnSetup(game, this);
}

void Pawn::cleanThreats(Game& game) {
    PiecePropagators::pawnSetup(game, this, true);
}

void Pawn::updateThreats(Game& game, int newX, int newY, Piece* captured) {
    PiecePropagators::pawnUpdater(game, this, newX, newY);
}