#include "game.hpp"
#include "move.hpp"
#include "board.hpp"
#include "appConfig.hpp"
#include "pieces/queen.hpp"
#include "propagators/piecePropagators.hpp"

using namespace std;

const int Game::PAWN_WEIGHT = 1;
const int Game::KING_CLOSE_WEIGHT = 1;

inline void Game::setupPiece(Piece* newPiece) {
    newPiece->loadTexture();
    pieces.insert(newPiece);
    int pieceX = newPiece->xPos;
    int pieceY = newPiece->yPos;
    board[pieceY][pieceX] = newPiece;
}

void Game::addThreat(Piece* attackingPiece, int xPos, int yPos) {
    if ((unsigned)xPos >= Board::width || (unsigned)yPos >= Board::height) return;
    ThreatTile* tile = threatMap[yPos][xPos];
    tile->threatening.insert(attackingPiece);
    if (attackingPiece->type == 1) {
        tile->whiteCount++;
    } else {
        tile->blackCount++;
    }
}

bool Game::removeThreat(Piece* oldAttacker, int xPos, int yPos) {
    if ((unsigned)xPos >= Board::width || (unsigned)yPos >= Board::height) return false;
    ThreatTile* tile = threatMap[yPos][xPos];
    size_t piecesRemoved = tile->threatening.erase(oldAttacker);
    // If the piece did not exist, return false to indicate no removal
    if (piecesRemoved == 0) return false;
    if (oldAttacker->type == 1) {
        tile->whiteCount--;
    } else {
        tile->blackCount--;
    }
    return true;
}

void Game::toggleThreats() {
    useThreatMap = !useThreatMap;
}

void Game::constructBoard() {
    const int BOARD_SIZE = 8;
    board.resize(BOARD_SIZE);
    for (int i = 0; i < BOARD_SIZE; i++) {
        board[i].resize(BOARD_SIZE);
    }
    setupBoard(*this, DEFAULT_FEN);
    setupThreatMap(*this);
}

Piece* Game::getPieceAt(int boardX, int boardY) {
    return board[boardY][boardX];
}

Game::Game() {
    constructBoard();
    totalMoves = 0;
    currentTurn = 1;
    gameScore = 0;
    useThreatMap = false;
}

inline int getPieceStartRow(Piece* p) {
    int pieceType = p->type;
    int pieceDir = -pieceType;
    bool isPawn = p->getPieceName() == "Pawn";
    int startRow = pieceType == 1 ? 7 : 0;
    return startRow + isPawn * pieceDir;
}

void Game::movePiece(Piece* p, int newX, int newY) {
    int pieceType = p->type;
    board[p->yPos][p->xPos] = nullptr;
    Piece* toCapture = board[newY][newX];
    bool isPawn = p->getPieceName() == "Pawn";
    bool isPromotion = isPawn && (newY == 0 || newY == Board::height-1);
    if (!isPromotion) {
        board[newY][newX] = p;
    }
    if (toCapture) {
        pieces.erase(toCapture);
        if (useThreatMap) {
            toCapture->cleanThreats(*this);
        }
        gameScore += (toCapture->getPieceValue()) * -pieceType;
        gameScore += KING_CLOSE_WEIGHT * (getPieceStartRow(toCapture) - toCapture->yPos);
    }
    if (p->getPieceName() != "King") {
        gameScore -= KING_CLOSE_WEIGHT * (p->yPos - newY);
    }
    totalMoves++;
    currentTurn = -currentTurn;
    Move* move = new Move;
    move->moved = p;
    move->captured = toCapture;
    move->xFrom = p->xPos;
    move->yFrom = p->yPos;
    move->xTo = newX;
    move->yTo = newY;
    moveHistory.emplace_back(move);
    p->doMove(*this, *move);
    if (isPromotion) {
        Queen* queen = new Queen(newX, newY, pieceType);
        pieces.erase(p);
        setupPiece(queen);
        if (useThreatMap) {
            queen->setup(*this);
        }
        gameScore -= 80 * pieceType;
    }
}

void Game::undoMove() {
    // No move is before the first move, so ignore
    if (totalMoves == 0) return;
    Move* moveTaken = moveHistory[totalMoves-1];
    moveHistory.pop_back();
    Piece* movedPiece = moveTaken->moved;
    Piece* captured = moveTaken->captured;
    Piece* undoPiece = board[moveTaken->yTo][moveTaken->xTo];
    int pieceType = movedPiece->type;
    bool isPromotion = movedPiece != undoPiece;
    if (movedPiece->getPieceName() != "King") {
        gameScore += KING_CLOSE_WEIGHT * (moveTaken->yFrom - moveTaken->yTo);
    }
    if (isPromotion) {
        pieces.erase(undoPiece);
        if (useThreatMap) {
            undoPiece->cleanThreats(*this);
        }
        delete undoPiece;
        pieces.insert(movedPiece);
        gameScore += 80 * pieceType;
    }
    board[moveTaken->yFrom][moveTaken->xFrom] = movedPiece;
    board[moveTaken->yTo][moveTaken->xTo] = captured;
    if (captured) {
        pieces.insert(captured);
        if (useThreatMap) {
            captured->setup(*this);
        }
        gameScore -= captured->getPieceValue() * -pieceType;
        gameScore -= KING_CLOSE_WEIGHT * (getPieceStartRow(captured) - captured->yPos);
    }
    totalMoves--;
    currentTurn = -currentTurn;
    movedPiece->doMove(*this, *moveTaken, true);
    delete moveTaken;
}

vector<Move*> Game::getPossibleMoves() {
    vector<Move*> moves;
    for (Piece* p : pieces) {
        if (p->type != currentTurn) continue;
        vector<Move*> possibleMoves = p->getMoves(*this);
        moves.insert(moves.end(), possibleMoves.begin(), possibleMoves.end());
    }
    return moves;
} 

vector<Move*> Game::getCaptures() {
    vector<Move*> moves = getPossibleMoves();
    vector<Move*> filteredMoves;
    for (Move* m : moves) {
        if (m->captured) {
            filteredMoves.emplace_back(m);
        } else {
            delete m;
        }
    }
    return filteredMoves;
}

vector<Move*> Game::getValidMoves() {
    vector<Move*> moves = getPossibleMoves();
    vector<Move*> filteredMoves;
    for (Move* m : moves) {
        if (m->moved->canDoMove(*this, m->xTo, m->yTo)) {
            filteredMoves.emplace_back(m);
        } else {
            delete m;
        }
    }
    return filteredMoves;
}

float Game::getGameScore() {
    return gameScore;
}
