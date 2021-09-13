#include <vector>
#include <unordered_set>
#include <string>

#include "piece.hpp"
#include "../game.hpp"
#include "../board.hpp"
#include "../move.hpp"
#include "../board.hpp"
#include "../threatTile.hpp"
#include "../propagators/propagator.hpp"

Piece::Piece(int x, int y, int ty) {
    xPos = x;
    yPos = y;
    type = ty;
    dragOffsetX = 0;
    dragOffsetY = 0;
}

void Piece::loadTexture() {
    myTexture.loadFromFile(getPiecePath());
}

void Piece::doMove(Game& game, Move& move, bool undo) {
    // The move is in reverse if we are undoing a move!
    if (undo) {
        swap(move.xTo, move.xFrom);
        swap(move.yTo, move.yFrom);
    }
    ThreatTile* tileFrom = game.threatMap[yPos][xPos];
    ThreatTile* tileTo = game.threatMap[move.yTo][move.xTo];
    if (!undo || !move.captured) {
        for (Piece* p : tileFrom->threatening) {
            string pieceName = p->getPieceName();
            // We should not update ourselves
            if (p == this) continue;
            // We can skip any of these pieces, they do not update
            if (pieceName == "King") continue;
            if (pieceName == "Pawn") continue;
            if (pieceName == "Knight") continue;
            Propagator* prop = Propagator::fetchPropagator(p, &move, true);
            prop->openPropagation(game);
            delete prop;
        }
    }
    updateThreats(game, move.xTo, move.yTo, move.captured);
    // If a piece was captured, threatened squares cant possibly change!
    if (undo || !move.captured) {
        for (Piece* p : tileTo->threatening) {
            string pieceName = p->getPieceName();
            // We should not update ourselves
            if (p == this) continue;
            // We can skip any of these pieces, they do not update
            if (pieceName == "King") continue;
            if (pieceName == "Pawn") continue;
            if (pieceName == "Knight") continue;
            Propagator* prop = Propagator::fetchPropagator(p, &move, false);
            prop->closePropagation(game);
            delete prop;
        }
    }
    xPos = move.xTo;
    yPos = move.yTo;
    dragOffsetX = 0;
    dragOffsetY = 0;
}

void Piece::setDrag(int deltaX, int deltaY) {
    dragOffsetX = deltaX;
    dragOffsetY = deltaY;
}

int Piece::getXTranslation() {
    return 10;
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

std::vector<Move*> Piece::getMoves(Game& game) {
    return vector<Move*>();
}

std::string Piece::getPieceName() {
    return "defaultName";
}

int Piece::getPieceValue() {
    return 0;
}

std::string Piece::getPiecePath() {
    std::string pieceColour = type == -1 ? "black" : "white";
    return "img/" + pieceColour + getPieceName() + ".png";
}

bool Piece::canDoMove(Game& game, int newX, int newY) {
    return true;
    
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
    if (getPieceName() != "Knight") {
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

    game.movePiece(this, newX, newY);
    vector<Move*> replies = game.getPossibleMoves();
    game.undoMove();
    bool kingCapturable = false;
    for (Move* m : replies) {
        Piece* captured = m->captured;
        if (captured && captured->getPieceName() == "King") {
            kingCapturable = true;
        }
        delete m;
    }

    return !kingCapturable;
}

bool Piece::isValidMove(Game& game, int newX, int newY) {
    return false;
}

void Piece::setup(Game& game) {
}

void Piece::cleanThreats(Game& game) {
}

void Piece::updateThreats(Game& game, int newX, int newY, Piece* captured) {
}