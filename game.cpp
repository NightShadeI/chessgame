#include "game.hpp"
#include "move.hpp"
#include "appConfig.hpp"
#include "pieces/queen.hpp"
#include "propagators/piecePropagators.hpp"

using namespace std;

const int Game::PAWN_WEIGHT = 1;
const int Game::KING_CLOSE_WEIGHT = 1;

void Game::setupPiece(Piece* newPiece) {
    newPiece->loadTexture();
    pieces.insert(newPiece);
    int pieceX = newPiece->xPos;
    int pieceY = newPiece->yPos;
    board[pieceY][pieceX] = newPiece;
    zobristHash ^= zobristScore(newPiece);
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
    setupHashes(*this);
    setupBoard(*this, DEFAULT_FEN);
    setupThreatMap(*this);
    seenPositions[zobristHash]++;
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
    bool isPawn = p->getPieceType() == PieceName::PAWN;
    int startRow = pieceType == 1 ? 7 : 0;
    return startRow + isPawn * pieceDir;
}

int Game::movePiece(Piece* p, int newX, int newY) {
    int pieceType = p->type;
    board[p->yPos][p->xPos] = nullptr;
    zobristHash ^= zobristScore(p);
    Piece* toCapture = board[newY][newX];
    bool isPawn = p->getPieceType() == PieceName::PAWN;
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
        zobristHash ^= zobristScore(toCapture);
    }
    if (p->getPieceType() != PieceName::KING) {
        gameScore -= KING_CLOSE_WEIGHT * (p->yPos - newY);
    }
    totalMoves++;
    currentTurn = -currentTurn;
    zobristHash ^= zobristValues[0];
    unique_ptr<Move> move = make_unique<Move>();
    move->moved = p;
    move->captured = toCapture;
    move->xFrom = p->xPos;
    move->yFrom = p->yPos;
    move->xTo = newX;
    move->yTo = newY;
    p->doMove(*this, *move);
    moveHistory.emplace_back(std::move(move));
    if (isPromotion) {
        Queen* queen = new Queen(newX, newY, pieceType);
        pieces.erase(p);
        setupPiece(queen);
        if (useThreatMap) {
            queen->setup(*this);
        }
        gameScore -= 80 * pieceType;
    } else {
        zobristHash ^= zobristScore(p);
    }
    return ++seenPositions[zobristHash];
}

void Game::undoMove() {
    // No move is before the first move, so ignore
    if (totalMoves == 0) return;
    if (!--seenPositions[zobristHash]) {
        seenPositions.erase(zobristHash);
    }
    unique_ptr<Move> moveTaken = move(moveHistory[totalMoves-1]);
    moveHistory.pop_back();
    Piece* movedPiece = moveTaken->moved;
    Piece* captured = moveTaken->captured;
    Piece* undoPiece = board[moveTaken->yTo][moveTaken->xTo];
    int pieceType = movedPiece->type;
    bool isPromotion = movedPiece != undoPiece;
    if (movedPiece->getPieceType() != PieceName::KING) {
        gameScore += KING_CLOSE_WEIGHT * (moveTaken->yFrom - moveTaken->yTo);
    }
    if (isPromotion) {
        pieces.erase(undoPiece);
        if (useThreatMap) {
            undoPiece->cleanThreats(*this);
        }
        zobristHash ^= zobristScore(undoPiece);
        delete undoPiece;
        pieces.insert(movedPiece);
        gameScore += 80 * pieceType;
    } else {
        zobristHash ^= zobristScore(movedPiece);
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
        zobristHash ^= zobristScore(captured);
    }
    totalMoves--;
    currentTurn = -currentTurn;
    movedPiece->doMove(*this, *moveTaken, true);
    zobristHash ^= zobristValues[0];
    zobristHash ^= zobristScore(movedPiece);
}

vector<unique_ptr<Move>> Game::getPossibleMoves() {
    vector<unique_ptr<Move>> moves;
    for (Piece* p : pieces) {
        if (p->type != currentTurn) continue;
        vector<unique_ptr<Move>> possibleMoves = p->getMoves(*this);
        moves.insert(
            moves.end(), 
            make_move_iterator(possibleMoves.begin()), 
            make_move_iterator(possibleMoves.end())
        );
    }
    return moves;
} 

vector<unique_ptr<Move>> Game::getCaptures() {
    vector<unique_ptr<Move>> moves = getPossibleMoves();
    vector<unique_ptr<Move>> filteredMoves;
    filteredMoves.reserve(moves.size());
    for (unique_ptr<Move>& m : moves) {
        if (m->captured) {
            filteredMoves.emplace_back(move(m));
        }
    }
    return filteredMoves;
}

vector<unique_ptr<Move>> Game::getValidMoves() {
    vector<unique_ptr<Move>> moves = getPossibleMoves();
    vector<unique_ptr<Move>> filteredMoves;
    filteredMoves.reserve(moves.size());
    for (unique_ptr<Move>& m : moves) {
        if (m->moved->canDoMove(*this, m->xTo, m->yTo)) {
            filteredMoves.emplace_back(move(m));
        }
    }
    return filteredMoves;
}

bool Game::inCheck() {
    for (Piece* p : pieces) {
        if (p->type == currentTurn) continue;
        vector<unique_ptr<Move>> possibleMoves = p->getMoves(*this);
        for (unique_ptr<Move>& m : possibleMoves) {
            if (m->captured && m->captured->getPieceType() == PieceName::KING) {
                return true;
            }
        }
    }
    return false;
}

unsigned long long Game::zobristScore(Piece* p) {
    int flatBoardLoc = Board::width * p->yPos + p->xPos;
    int flatZobristLoc = 12 * flatBoardLoc;
    return zobristValues[flatZobristLoc + 2 * p->getPieceType() + (p->type == 1) + 1];
}