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
    vector<unique_ptr<Move>> responses = game.getCaptures();
    if (responses.size() == 0) return nulMove;
    std::sort(responses.begin(), responses.end(), [this](unique_ptr<Move>& a, unique_ptr<Move>& b) {
        return movementScore(a.get(), nullptr) > movementScore(b.get(), nullptr);
    });

    // Evaluate the moves
    alpha = max(alpha, nulMove);
    int highestScore = nulMove;
    for (unique_ptr<Move>& r : responses) {
        int seen = game.movePiece(r->moved, r->xTo, r->yTo);
        int movementScore;
        if (seen >= 2) {
            movementScore = 0;
        } else if (r->captured->getPieceType() == PieceName::KING) {
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
    vector<unique_ptr<Move>> responses = game.getPossibleMoves();
    if (responses.size() == 0) return -MATE_SCORE;

    Move* entryMove = entry ? entry->bestMove.get() : nullptr;
    std::sort(responses.begin(), responses.end(), [this, entryMove](unique_ptr<Move>& a, unique_ptr<Move>& b) {
        return movementScore(a.get(), entryMove) > movementScore(b.get(), entryMove);
    });
    int highestScore = NEGATIVE_INF;
    unique_ptr<Move> bestMove = nullptr;
    for (unique_ptr<Move>& r : responses) {
        // Igore the move if we already analysed it
        int seen = game.movePiece(r->moved, r->xTo, r->yTo);
        int movementScore;
        Piece* capturedPiece = r->captured;
        if (seen >= 2) {
            movementScore = 0;
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
            bestMove = move(r);
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
    newEntry->bestMove = move(bestMove);
    newEntry->score = correctedMateScore;
    if (highestScore <= oldAlpha) {
        newEntry->type = ttType::UPPERBOUND;
        newEntry->bestMove = nullptr;
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
        if (entry && entry->bestMove) {
            Piece* movedPiece = entry->bestMove->moved;
            float positionScore = (float)entry->score/10;
            game.movePiece(movedPiece, entry->bestMove->xTo, entry->bestMove->yTo);
            cout << i << ": Moved " << movedPiece->getPieceName() << " to (" << entry->bestMove->xTo << ", " << entry->bestMove->yTo << ") for " << positionScore << endl;
        } else {
            cout << "No more for " << i << " for some reason" << endl;
            break;
        }
        traceDistance = i;
    }
    for (int i = 1; i <= traceDistance; i++) {
        game.undoMove();
    }
}

unique_ptr<Move> BruteForceMover::getMove(Game& game) {

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
    cout << "Explored a total of " << movesExplored << " moves in " << totalDuration.count() << " ms" << endl;
    cout << "Rate of nodes : " << movesExplored/totalDuration.count() << "/ms" << endl;
    cout << "Hit the table a total of " << tableHits << " times" << endl;
    cout << "Reached depth of : " << reached << endl;
    cout << "Best score: " << score/10 << endl;

    // Return the best move
    unique_ptr<ttEntry>& rootEntry = tTable[game.zobristHash];
    unique_ptr<Move> bestMove = move(rootEntry->bestMove);
    tTable.clear();
    return bestMove;
}