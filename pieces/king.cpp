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
    if (xDiff <= 1 && yDiff <= 1) return true;
    if (totalMoves == 0) {
        if (type == 1) {
            Piece* whiteKingsideRook = game.getPieceAt(Board::width-1, Board::height-1);
            Piece* whiteQueensideRook = game.getPieceAt(0, Board::height-1);
            if (newX == 6 && newY == 7 && whiteKingsideRook && whiteKingsideRook->totalMoves == 0 && ((game.bitBoard & WHITE_KINGSIDE_CASTLE_MASK) == 0)) {
                return true;
            }
            if (newX == 2 && newY == 7 && whiteQueensideRook && whiteQueensideRook->totalMoves == 0 && ((game.bitBoard & WHITE_QUEENSIDE_CASTLE_MASK) == 0)) {
                return true;
            }
        } else {
            Piece* blackKingsideRook = game.getPieceAt(Board::width-1, 0);
            Piece* blackQueensideRook = game.getPieceAt(0, 0);
            if (newX == 6 && newY == 0 && blackKingsideRook && blackKingsideRook->totalMoves == 0 && ((game.bitBoard & BLACK_KINGSIDE_CASTLE_MASK) == 0)) {
                return true;
            }
            if (newX == 2 && newY == 0 && blackQueensideRook && blackQueensideRook->totalMoves == 0 && ((game.bitBoard & BLACK_QUEENSIDE_CASTLE_MASK) == 0)) {
                return true;
            }
        }
    }
    return false;
}

vector<Move> King::getMoves(Game& game) {
    vector<Move> moves;
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
            moves.emplace_back(Move{this, game.getPieceAt(newX, newY), 0, 0, newX, newY});
        }
    }
    if (totalMoves == 0) {
        if (type == 1) {
            Piece* whiteKingsideRook = game.getPieceAt(Board::width-1, Board::height-1);
            Piece* whiteQueensideRook = game.getPieceAt(0, Board::height-1);
            if (whiteKingsideRook && whiteKingsideRook->totalMoves == 0 && ((game.bitBoard & WHITE_KINGSIDE_CASTLE_MASK) == 0)) {
                moves.emplace_back(Move{this, nullptr, 0, 0, 6, 7, MoveType::WHITE_KINGSIDE_CASTLE});
            }
            if (whiteQueensideRook && whiteQueensideRook->totalMoves == 0 && ((game.bitBoard & WHITE_QUEENSIDE_CASTLE_MASK) == 0)) {
                moves.emplace_back(Move{this, nullptr, 0, 0, 2, 7, MoveType::WHITE_QUEENSIDE_CASTLE});
            }
        } else {
            Piece* blackKingsideRook = game.getPieceAt(Board::width-1, 0);
            Piece* blackQueensideRook = game.getPieceAt(0, 0);
            if (blackKingsideRook && blackKingsideRook->totalMoves == 0 && ((game.bitBoard & BLACK_KINGSIDE_CASTLE_MASK) == 0)) {
                moves.emplace_back(Move{this, nullptr, 0, 0, 6, 0, MoveType::BLACK_KINGSIDE_CASTLE});
            }
            if (blackQueensideRook && blackQueensideRook->totalMoves == 0 && ((game.bitBoard & BLACK_QUEENSIDE_CASTLE_MASK) == 0)) {
                moves.emplace_back(Move{this, nullptr, 0, 0, 2, 0, MoveType::BLACK_QUEENSIDE_CASTLE});
            }
        }
    }
    return moves;
}

bool King::isStartingLocation() {
    if (type == 1) {
        return yPos == 7 && xPos == 4;
    }
    return yPos == 0 && xPos == 4;
}