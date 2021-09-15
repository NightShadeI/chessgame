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

bool Piece::isSlidingPiece() {
    return false;
}

std::string Piece::getPiecePath() {
    std::string pieceColour = type == -1 ? "black" : "white";
    return "img/" + pieceColour + getPieceName() + ".png";
}

bool Piece::canDoMove(Game& game, int newX, int newY) {

    string pieceName = getPieceName();
    if (pieceName == "King") {
        ThreatTile* mySquare = game.threatMap[yPos][xPos];
        ThreatTile* moveTo = game.threatMap[newY][newX];
        Piece* pieceAt = game.getPieceAt(newX, newY);
        // Sliding pieces also threaten their own square, for efficiency reasons
        int unsafeReq = pieceAt && pieceAt->isSlidingPiece() ? 2 : 1;
        int numAttackingMe = type == 1 ? mySquare->blackCount : mySquare->whiteCount;
        int numAttackers = type == 1 ? moveTo->blackCount : moveTo->whiteCount;
        if (numAttackingMe == 0 && numAttackers < unsafeReq) return true;
        if (numAttackers >= unsafeReq) return false;
        // We need to determine if the square will be threatened when we move there
        for (Piece* p : mySquare->threatening) {
            if (p->type == type) continue;
            string enemyName = p->getPieceName();
            if (enemyName == "Knight" || enemyName == "Pawn") continue;
            int xDiff = xPos - p->xPos;
            int yDiff = yPos - p->yPos;
            int newXDiff = newX - p->xPos;
            int newYDiff = newY - p->yPos;
            // Implies piece capture
            if (newXDiff == 0 && newYDiff == 0 && numAttackers < unsafeReq) return true;
            if (xDiff * newYDiff == yDiff * newXDiff) return false;
        }
        return true;
    }

    Piece* myKing = type == 1 ? game.whiteKing : game.blackKing;
    ThreatTile* kingSquare = game.threatMap[myKing->yPos][myKing->xPos];
    int numAttackingKing = type == 1 ? kingSquare->blackCount : kingSquare->whiteCount;

    // If we are under double check we have to move the king
    if (numAttackingKing >= 2) {
        return false;
    }

    // We first need to determine if this piece is pinned
    // if the piece is pinned then we cannot possibly move it
    int xDiff = xPos - myKing->xPos;
    int yDiff = yPos - myKing->yPos;
    bool mightBePinned = (xDiff == 0 || yDiff == 0 || abs(xDiff) == abs(yDiff));

    if (numAttackingKing == 0 && !mightBePinned) return true;

    // Does the piece block the king? Only used if in check
    bool doesBlock = false;
    // If the king is in check, we need to determine if this can block the checker
    if (numAttackingKing == 1) {
        // Let's find the piece attacking the king
        Piece* kingsAttacker;
        for (Piece* p : kingSquare->threatening) {
            if (p->type != type) {
                kingsAttacker = p;
                break;
            }
        }
        if (kingsAttacker->xPos == newX && kingsAttacker->yPos == newY) {
            // We are most certainly blocking if we capture the piece!
            doesBlock = true;
        } else if (kingsAttacker->getPieceName() == "Knight") {
            // if we don't capture the knight then this is an invalid move
            return false;
        } else if (kingsAttacker->xPos == myKing->xPos) {
            // Vertical attack
            if (newX != myKing->xPos) return false;
            int yAttackDistance = kingsAttacker->yPos - myKing->yPos;
            int yMovementDistance = newY - myKing->yPos;
            if (yMovementDistance > 0 == yAttackDistance > 0 && abs(yMovementDistance) <= abs(yAttackDistance)) {
                doesBlock = true;
            }
        } else if (kingsAttacker->yPos == myKing->yPos) {
            // Horizontal attack
            if (newY != myKing->yPos) return false;
            int xAttackDistance = kingsAttacker->xPos - myKing->xPos;
            int xMovementDistance = newX - myKing->xPos;
            if (xMovementDistance > 0 == xAttackDistance > 0 && abs(xMovementDistance) <= abs(xAttackDistance)) {
                doesBlock = true;
            }
        } else {
            // Diagonal attack
            int xMovementDistance = newX - myKing->xPos;
            int yMovementDistance = newY - myKing->yPos;
            if (abs(xMovementDistance) != abs(yMovementDistance)) return false;
            int xAttackerDistance = kingsAttacker->xPos - myKing->xPos;
            int yAttackerDistance = kingsAttacker->yPos - myKing->yPos;
            if (xMovementDistance > 0 == xAttackerDistance > 0 &&
                yMovementDistance > 0 == yAttackerDistance > 0 &&
                abs(xMovementDistance) <= abs(xAttackerDistance)
            ) {
                doesBlock = true;
            }
        }
    }

    // If we are certain that we aren't pinned we can return immediately
    if (!mightBePinned) {
        return doesBlock;
    }

    // We now need to determine if the piece is pinned. If the piece is pinned, then
    // of course the move that is being examined cannot be valid
    ThreatTile* mySquare = game.threatMap[yPos][xPos];
    int numAttackingMe = type == 1 ? mySquare->blackCount : mySquare->whiteCount;
    int newXDiff = newX - xPos;
    int newYDiff = newY - yPos;

    // Means that we cannot possibly be pinned
    // Checks if none are attacking me, or if we are moving along the same file
    if (numAttackingMe == 0 || xDiff * newYDiff == yDiff * newXDiff) {
        // doesBlock should always be false if moving along same file
        if (numAttackingKing == 1) return doesBlock;
        return true;
    }

    // Otherwise we might be pinned. We need to determine if there are any pieces between us and the king
    int xDir = xDiff > 0 ? 1 : xDiff == 0 ? 0 : -1;
    int yDir = yDiff > 0 ? 1 : yDiff == 0 ? 0 : -1;
    int curX = myKing->xPos + xDir;
    int curY = myKing->yPos + yDir;
    while (curX != xPos || curY != yPos) {
        Piece* pieceAt = game.getPieceAt(curX, curY);
        // Implies we cant be pinned
        if (pieceAt) {
            if (numAttackingKing == 1) return doesBlock;
            return true;
        }
        curX += xDir;
        curY += yDir;
    }


    // Okay so we definitely might be pinned. Let's check the pieces attacking us. If they are on our file,
    // Then this is certainly an invalid move
    for (Piece* p : mySquare->threatening) {
        if (p->type == type) continue;
        if (p->getPieceName() == "King") continue;
        if (p->getPieceName() == "Knight") continue;
        if (p->getPieceName() == "Pawn") continue;
        int attackingXDiff = p->xPos - myKing->xPos;
        int attackingYDiff = p->yPos - myKing->yPos;

        // Implies we are certainly pinned
        if (xDiff * attackingYDiff == yDiff * attackingXDiff) {
            return false;
        }
    }

    // If we get here we KNOW we aren't pinned, this piece is free to move anywhere provided we aren't in check
    // This is the very final validation
    if (numAttackingKing == 1) return doesBlock;

    return true;
}

/*
* This function is more for humans and determing if their move is valid
*/
bool Piece::vigorousCanDoMove(Game& game, int newX, int newY) {
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

    // Check if the king can be captured the old way, incase threatmap is disabled
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