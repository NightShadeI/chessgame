#include "game.hpp"
#include "move.hpp"
#include "board.hpp"
#include "appConfig.hpp"
#include "pieces/queen.hpp"

using namespace std;

const int Game::KING_CLOSE_WEIGHT = 1;
const int Game::KING_CASTLE_WEIGHT = 6;
const int Game::DEVELOPMENT_WEIGHT = 3;

void Game::setupPiece(Piece* newPiece) {
    newPiece->loadTexture();
    pieces.insert(newPiece);
    int pieceX = newPiece->xPos;
    int pieceY = newPiece->yPos;
    board[pieceY][pieceX] = newPiece;
    zobristHash ^= zobristScore(newPiece);
}

void Game::constructBoard() {
    const int BOARD_SIZE = 8;
    board.resize(BOARD_SIZE);
    for (int i = 0; i < BOARD_SIZE; i++) {
        board[i].resize(BOARD_SIZE);
    }
    bitBoard = 0;
    setupHashes(*this);
    setupBoard(*this, DEFAULT_FEN);
    seenPositions[zobristHash]++;
}

Game::Game() {
    constructBoard();
    totalMoves = 0;
    currentTurn = 1;
    gameScore = 0;
}

inline int getPieceStartRow(Piece* p) {
    int pieceType = p->type;
    int pieceDir = -pieceType;
    bool isPawn = p->getPieceType() == PieceName::PAWN;
    int startRow = pieceType == 1 ? 7 : 0;
    return startRow + isPawn * pieceDir;
}

void Game::basicUndoMove(const int fromX, const int fromY, const int toX, const int toY) {
    setBit(toY, toX);
    setBit(fromY, fromX);
    Piece* myPiece = getPieceAt(toX, toY);
    Move myMove{myPiece, nullptr, fromX, fromY, toX, toY};
    zobristHash ^= zobristScore(myPiece);
    myPiece->undoMove(*this, myMove);
    zobristHash ^= zobristScore(myPiece);
    board[toY][toX] = nullptr;
    board[fromY][fromX] = myPiece;
}

void Game::basicMove(const int fromX, const int fromY, const int toX, const int toY) {
    setBit(fromY, fromX);
    setBit(toY, toX);
    Piece* myPiece = getPieceAt(fromX, fromY);
    Move myMove{myPiece, nullptr, fromX, fromY, toX, toY};
    zobristHash ^= zobristScore(myPiece);
    myPiece->doMove(*this, myMove);
    zobristHash ^= zobristScore(myPiece);
    board[fromY][fromX] = nullptr;
    board[toY][toX] = myPiece;
}

void Game::performCastle(MoveType moveType) {
    switch (moveType) {
        case MoveType::WHITE_KINGSIDE_CASTLE:
            basicMove(7, 7, 5, 7);
            break;
        case MoveType::WHITE_QUEENSIDE_CASTLE:
            basicMove(0, 7, 3, 7);
            break;
        case MoveType::BLACK_KINGSIDE_CASTLE:
            basicMove(7, 0, 5, 0);
            break;
        case MoveType::BLACK_QUEENSIDE_CASTLE:
            basicMove(0, 0, 3, 0);
            break;
    }
}

void Game::undoCastle(MoveType moveType) {
    switch (moveType) {
        case MoveType::WHITE_KINGSIDE_CASTLE:
            basicUndoMove(7, 7, 5, 7);
            break;
        case MoveType::WHITE_QUEENSIDE_CASTLE:
            basicUndoMove(0, 7, 3, 7);
            break;
        case MoveType::BLACK_KINGSIDE_CASTLE:
            basicUndoMove(7, 0, 5, 0);
            break;
        case MoveType::BLACK_QUEENSIDE_CASTLE:
            basicUndoMove(0, 0, 3, 0);
            break;
    }
}

int Game::movePiece(Move& move) {
    
    // Setup different parameters
    Piece* p = move.moved;
    int newX = move.xTo;
    int newY = move.yTo;
    int pieceType = p->type;
    board[p->yPos][p->xPos] = nullptr;
    zobristHash ^= zobristScore(p);
    Piece* toCapture = board[newY][newX];
    bool isPawn = p->getPieceType() == PieceName::PAWN;
    bool isPromotion = isPawn && (newY == 0 || newY == Board::height-1);

    // Check for castles
    if (move.moveType > 0) {
        gameScore += KING_CASTLE_WEIGHT * -pieceType;
        performCastle(move.moveType);
    }

    // Handle some scoring related to how close to king
    if (p->getPieceType() != PieceName::KING) {
        gameScore -= KING_CLOSE_WEIGHT * (p->yPos - newY);
    }

    // Handle some scoring for developing pieces
    if (p->isDevelopmentalPiece() && p->totalMoves == 0) {
        gameScore += DEVELOPMENT_WEIGHT * -pieceType;
    }

    // Handle logic when piece captured
    if (toCapture) {
        pieces.erase(toCapture);
        gameScore += (toCapture->getPieceValue()) * -pieceType;
        gameScore += KING_CLOSE_WEIGHT * (getPieceStartRow(toCapture) - toCapture->yPos);
        zobristHash ^= zobristScore(toCapture);
    } else {
        setBit(newY, newX);
    }

    // Perform the move (We must do this before promotion, due to promotions zobrist score)
    setBit(p->yPos, p->xPos);
    unique_ptr<Move> myMove = make_unique<Move>(Move{p, toCapture, p->xPos, p->yPos, newX, newY, move.moveType});
    p->doMove(*this, *myMove);
    moveHistory.emplace_back(std::move(myMove));

    // Handle logic when piece promoted
    if (isPromotion) {
        Queen* queen = new Queen(newX, newY, pieceType);
        queen->totalMoves = p->totalMoves;
        pieces.erase(p);
        setupPiece(queen);
        gameScore -= 80 * pieceType;
    } else {
        board[newY][newX] = p;
        zobristHash ^= zobristScore(p);
    }

    // Update the rest of the game state as needed
    totalMoves++;
    currentTurn = -currentTurn;
    zobristHash ^= zobristValues[0];
    return ++seenPositions[zobristHash];
}

void Game::undoMove() {
    if (!--seenPositions[zobristHash]) {
        seenPositions.erase(zobristHash);
    }

    // Setup different parameters
    unique_ptr<Move> moveTaken = move(moveHistory[totalMoves-1]);
    moveHistory.pop_back();
    Piece* movedPiece = moveTaken->moved;
    Piece* captured = moveTaken->captured;
    Piece* undoPiece = board[moveTaken->yTo][moveTaken->xTo];
    int pieceType = movedPiece->type;
    bool isPromotion = movedPiece != undoPiece;

    // Check for castles
    if (moveTaken->moveType > 0) {
        gameScore -= KING_CASTLE_WEIGHT * -pieceType;
        undoCastle(moveTaken->moveType);
    }

    // Handle some scoring for developing pieces
    if (movedPiece->isDevelopmentalPiece() && movedPiece->totalMoves == 1) {
        gameScore -= DEVELOPMENT_WEIGHT * -pieceType;
    }

    // Handle some scoring related to how close to king
    if (movedPiece->getPieceType() != PieceName::KING) {
        gameScore += KING_CLOSE_WEIGHT * (moveTaken->yFrom - moveTaken->yTo);
    }

    // Handle logic when piece captured
    if (captured) {
        pieces.insert(captured);
        gameScore -= captured->getPieceValue() * -pieceType;
        gameScore -= KING_CLOSE_WEIGHT * (getPieceStartRow(captured) - captured->yPos);
        zobristHash ^= zobristScore(captured);
    } else {
        setBit(moveTaken->yTo, moveTaken->xTo);
    }

    // Handle logic when piece promoted
    if (isPromotion) {
        pieces.erase(undoPiece);
        zobristHash ^= zobristScore(undoPiece);
        delete undoPiece;
        pieces.insert(movedPiece);
        gameScore += 80 * pieceType;
    } else {
        zobristHash ^= zobristScore(movedPiece);
    }

    // Update the rest of the game state as needed
    setBit(moveTaken->yFrom, moveTaken->xFrom);
    board[moveTaken->yFrom][moveTaken->xFrom] = movedPiece;
    board[moveTaken->yTo][moveTaken->xTo] = captured;
    totalMoves--;
    currentTurn = -currentTurn;
    movedPiece->undoMove(*this, *moveTaken);
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
        if (m->moved->vigorousCanDoMove(*this, *m)) {
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

void Game::setBit(int gameRow, int gameCol) {
    const std::uint64_t bitPosition = gameRow * Board::height + gameCol;
    bitBoard ^= (static_cast<std::uint64_t>(1) << bitPosition);
}