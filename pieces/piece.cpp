#include <vector>
#include <unordered_set>
#include <string>

#include "piece.hpp"
#include "../game.hpp"
#include "../board.hpp"
#include "../move.hpp"
#include "../board.hpp"

Piece::Piece(int x, int y, int ty) {
    xPos = x;
    yPos = y;
    type = ty;
    dragOffsetX = 0;
    dragOffsetY = 0;
    totalMoves = 0;
}

void Piece::loadTexture() {
    myTexture.loadFromFile(getPiecePath());
}

void Piece::doMove(Game& game, Move& move) {
    xPos = move.xTo;
    yPos = move.yTo;
    dragOffsetX = 0;
    dragOffsetY = 0;
    totalMoves++;
}

void Piece::undoMove(Game& game, Move& move) {
    xPos = move.xFrom;
    yPos = move.yFrom;
    totalMoves--;
}

void Piece::setDrag(int deltaX, int deltaY) {
    dragOffsetX = deltaX;
    dragOffsetY = deltaY;
}

void Piece::render(sf::RenderWindow& window) {
    const int drawX = Board::tileSize * xPos + getXTranslation() + dragOffsetX;
    const int drawY = Board::tileSize * yPos + 10 + dragOffsetY;
    sf::Sprite mySprite;
    mySprite.setTexture(myTexture);
    mySprite.setPosition(drawX, drawY);
    mySprite.setScale(0.5f, 0.5f);
    window.draw(mySprite);
}

vector<Move> Piece::getMoves(Game& game) {
    return vector<Move>();
}

/*
* This function is more for humans and determing if their move is valid
*/
bool Piece::vigorousCanDoMove(Game& game, Move& move) {

    int newX = move.xTo;
    int newY = move.yTo;

    // Check the move is inside the the board
    if ((unsigned)newX >= Board::width || (unsigned)newY >= Board::height) return false;

    // Check the move is not the square the piece is currently on
    if (newX == xPos && newY == yPos) return false;

    // Check if the move is a valid move for the piece
    if (!isValidMove(game, newX, newY)) {
        return false;
    }

    // Check there are no pieces in the way of this piece
    // Knights are able to jump so we ignore these pieces
    if (getPieceType() != PieceName::KNIGHT) {
        int xDir = newX > xPos ? 1 : newX == xPos ? 0 : -1;
        int yDir = newY > yPos ? 1 : newY == yPos ? 0 : -1;
        int curX = xPos + xDir, curY = yPos + yDir;
        while (curX != newX || curY != newY) {
            if (game.getPieceAt(curX, curY)) {
                return false;
            }
            curX += xDir;
            curY += yDir;
        }
    }

    // if the piece at the target square is the same type as ours
    // then we also cannot move there since we cant take our own
    Piece* targetPiece = game.getPieceAt(newX, newY);
    if (targetPiece && targetPiece->type == type) return false;

    // Check if the king can be captured
    game.movePiece(move);
    vector<Move> replies = game.getCaptures();
    game.undoMove();
    bool kingCapturable = false;
    for (Move& m : replies) {
        Piece* captured = m.captured;
        if (captured && captured->getPieceType() == PieceName::KING) {
            kingCapturable = true;
        }
    }
    return !kingCapturable;
}

bool Piece::isValidMove(Game& game, int newX, int newY) {
    return false;
}

bool Piece::isStartingLocation() {
    return false;
}