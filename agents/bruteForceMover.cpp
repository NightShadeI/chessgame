#include <vector>
#include <algorithm>
#include <chrono>
#include <iostream>

#include "bruteForceMover.hpp"
#include "../appConfig.hpp"

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::milliseconds;

static const int NEGATIVE_INF = -100000;
static const int POSITIVE_INF =  100000;
static const int MATE_SCORE   =  10000;
static const int MAX_DEPTH    =  100;
static const int MAX_SEARCH   =  1000;

inline bool isMateScore (int score) {
    return (unsigned)(score + (MATE_SCORE - MAX_DEPTH)) >= ((MATE_SCORE - MAX_DEPTH) << 1);
}

inline int correctRetrievedMateScore (int score, int numPlySearched) {
    if (isMateScore (score)) {
        int sign = (score > 0) - (score < 0);
        return (score * sign - numPlySearched) * sign;
    }
    return score;
}

inline int correctMateScoreForStorage (int score, int numPlySearched) {
    if (isMateScore (score)) {
        int sign = (score > 0) - (score < 0);
        return (score * sign + numPlySearched) * sign;
    }
    return score;
}

BruteForceMover::BruteForceMover(int d, bool timer) {
    depth = d;
    enableTimer = timer;
}

int BruteForceMover::movementScore(Move* m, Move* optimalMove) {
    Piece* movedPiece = m->moved;
    if (
        optimalMove &&
        optimalMove->moved == movedPiece &&
        optimalMove->xTo == m->xTo &&
        optimalMove->yTo == m->yTo
    ) return POSITIVE_INF;
    Piece* capturedPiece = m->captured;
    int pushScore = 10 * ((m->yTo - m->yFrom) * -movedPiece->type);
    if (capturedPiece) {
        return 10 * capturedPiece->getPieceValue() - movedPiece->getPieceValue() + pushScore;
    }
    return 0;
}

int BruteForceMover::quiescence(Game& game, int mult, int alpha, int beta, int d, int plyFromRoot) {

    // Terminal nodes
    alpha = max (alpha, -MATE_SCORE + plyFromRoot);
    beta = min (beta, MATE_SCORE - plyFromRoot);
    if (alpha >= beta) return alpha;

    int nulMove = mult * game.getGameScore();
    if (nulMove >= beta) return nulMove;

    // Generate and order the moves
    vector<Move> responses = game.getCaptures();
    if (responses.size() == 0) return nulMove;
    std::sort(responses.begin(), responses.end(), [this](Move& a, Move& b) {
        return movementScore(&a, nullptr) > movementScore(&b, nullptr);
    });

    // Evaluate the moves
    alpha = max(alpha, nulMove);
    int highestScore = nulMove;
    for (Move& r : responses) {
        int seen = game.movePiece(r);
        int movementScore;
        if (seen >= 2) {
            movementScore = 0;
        } else if (r.captured->getPieceType() == PieceName::KING) {
            movementScore = MATE_SCORE - plyFromRoot - 1;
        } else {
            movementScore = -quiescence(game, -mult, -beta, -alpha, d+1, plyFromRoot+1);
        }
        game.undoMove();
        movesExplored++;
        if (movementScore > highestScore) {
            highestScore = movementScore;
            alpha = max(alpha, movementScore);
        }
        if (alpha >= beta) break;
    }
    return highestScore;
}

int BruteForceMover::bruteForce(Game& game, int mult, int alpha, int beta, int d, int plyFromRoot) {

    int oldAlpha = alpha;
    unsigned long long positionHash = game.zobristHash;

    if (plyFromRoot > 0) {
        alpha = max (alpha, -MATE_SCORE + plyFromRoot);
        beta = min (beta, MATE_SCORE - plyFromRoot);
        if (alpha >= beta) return alpha;
    }

    // Tranposition table
    unique_ptr<ttEntry>& entry = tTable[positionHash];
    if (entry && entry->depth >= d) {
        tableHits++;
        int entryScore = correctRetrievedMateScore(entry->score, plyFromRoot);
        if (entry->type == ttType::EXACT) {
            return entryScore;
        }
        if (entry->type == ttType::LOWERBOUND) {
            alpha = max(alpha, entryScore);
        } else {
            beta = min(beta, entryScore);
        }
        if (alpha >= beta) return entryScore;
    }

    if (d == 0) return quiescence(game, -mult, alpha, beta, depth, plyFromRoot);

    // Generate and order responses
    vector<Move> responses = game.getPossibleMoves();
    if (responses.size() == 0) return -MATE_SCORE;

    std::optional<Move> entryMove = entry ? entry->bestMove : std::nullopt;
    std::sort(responses.begin(), responses.end(), [this, entryMove = entryMove.has_value() ? &entryMove.value() : nullptr](Move& a, Move& b) {
        return movementScore(&a, entryMove) > movementScore(&b, entryMove);
    });
    int highestScore = NEGATIVE_INF;
    MoveType lastMoveType = game.moveHistory.size() ? game.moveHistory.back().moveType : MoveType::NORMAL;
    Move* bestMove = nullptr;
    for (Move& r : responses) {
        // Igore the move if we already analysed it
        int seen = game.movePiece(r);
        int movementScore;
        Piece* capturedPiece = r.captured;
        const int pos = 64 * lastMoveType + 8 * r.yTo + r.xTo;
        if (seen >= 2) {
            movementScore = 0;
        } else if (illegalMoveDetection[pos]) {
            movementScore = MATE_SCORE - plyFromRoot - 1;
        } else if (capturedPiece && capturedPiece->getPieceType() == PieceName::KING) {
            movementScore = MATE_SCORE - plyFromRoot - 1;
        } else {
            movementScore = -bruteForce(game, -mult, -beta, -alpha, d-1, plyFromRoot+1);
        }
        game.undoMove();
        movesExplored++;
        if (movementScore > highestScore) {
            highestScore = movementScore;
            alpha = max(alpha, movementScore);
            bestMove = &r;
        }
        if (alpha >= beta) break;
    }

    int correctedMateScore = correctMateScoreForStorage(highestScore, plyFromRoot);
    // Check if we can't move anywhere without king being captured.
    // If this is the case, its either checkmate or stalemate
    if (highestScore < beta && correctedMateScore <=  2 - MATE_SCORE && !game.inCheck()) {
        highestScore = 0;
    }

    // Add to transposition table
    unique_ptr<ttEntry> newEntry = make_unique<ttEntry>();
    newEntry->bestMove = bestMove == nullptr ? std::nullopt : optional{*bestMove};
    newEntry->score = correctedMateScore;
    if (highestScore <= oldAlpha) {
        newEntry->type = ttType::UPPERBOUND;
        newEntry->bestMove = std::nullopt;
    } else if (highestScore >= beta) {
        newEntry->type = ttType::LOWERBOUND;
    } else {
        newEntry->type = ttType::EXACT;
    }
    newEntry->depth = d;
    tTable[positionHash] = move(newEntry);
    return highestScore;
}

void BruteForceMover::displayDebugTrace(Game& game, int toDepth) {
    int traceDistance = 0;
    for (int i = 1; i <= toDepth; i++) {
        unique_ptr<ttEntry>& entry = tTable[game.zobristHash];
        if (entry && entry->bestMove.has_value()) {
            Piece* movedPiece = entry->bestMove->moved;
            float positionScore = (float)entry->score/10;
            game.movePiece(*entry->bestMove);
            std::cout << i << ": Moved " << movedPiece->getPieceName() << " to (" << entry->bestMove->xTo << ", " << entry->bestMove->yTo << ") for " << positionScore << endl;
        } else {
            std::cout << "No more for " << i << " for some reason" << endl;
            break;
        }
        traceDistance = i;
    }
    for (int i = 1; i <= traceDistance; i++) {
        game.undoMove();
    }
}

Move BruteForceMover::getMove(Game& game) {

    // Initalise stats
    movesExplored = 0;
    tableHits = 0;
    float score;
    int reached = MAX_DEPTH;
    auto startTime = high_resolution_clock::now();

    // Perform Iterative deepening search
    for (int i = 1; i <= MAX_DEPTH; i++) {
        score = (float)bruteForce(game, game.currentTurn, NEGATIVE_INF, POSITIVE_INF, i, 0);
        auto passedTime = high_resolution_clock::now();
        duration<double, std::milli> passedDuration = passedTime - startTime;
        if (i >= depth && (!enableTimer || passedDuration.count() >= MAX_SEARCH)) {
            reached = i;
            break;
        };
    }

    // Log some of the stats
    auto endTime = high_resolution_clock::now();
    duration<double, std::milli> totalDuration = endTime - startTime;
    std::cout << "Explored a total of " << movesExplored << " moves in " << totalDuration.count() << " ms" << endl;
    std::cout << "Rate of nodes : " << movesExplored/totalDuration.count() << "/ms" << endl;
    std::cout << "Hit the table a total of " << tableHits << " times" << endl;
    std::cout << "Reached depth of : " << reached << endl;
    std::cout << "Best score: " << score/10 << endl;

    // Return the best move
    unique_ptr<ttEntry>& rootEntry = tTable[game.zobristHash];
    Move bestMove = rootEntry->bestMove.value();
    tTable.clear();
    return bestMove;
}