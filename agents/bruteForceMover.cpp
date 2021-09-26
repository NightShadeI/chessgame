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

BruteForceMover::BruteForceMover(int d) {
    depth = d;
}

int BruteForceMover::movementScore(Move* m, Move* optimalMove) {
    Piece* movedPiece = m->moved;
    if (
        optimalMove &&
        optimalMove->moved == movedPiece &&
        optimalMove->xTo == optimalMove->xTo &&
        optimalMove->yTo == optimalMove->yTo
    ) return POSITIVE_INF;
    Piece* capturedPiece = m->captured;
    int pushScore = 10 * ((m->yTo - m->yFrom) * -movedPiece->type);
    if (capturedPiece) {
        return 10 * capturedPiece->getPieceValue() - movedPiece->getPieceValue() + pushScore;
    }
    return 0;
}

int BruteForceMover::quiescence(Game& game, int mult, int alpha, int beta, int d, int mateIn) {

    // Terminal nodes
    if (mateIn == 0) return 0;
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
            movementScore = MATE_SCORE;
        } else {
            movementScore = -quiescence(game, -mult, -beta, -alpha, d+1, mateIn-1);
        }
        if (movementScore >= MATE_SCORE - MAX_DEPTH) {
            movementScore--;
            mateIn = MATE_SCORE - movementScore;
        } else if (movementScore <= MAX_DEPTH - MATE_SCORE ) {
            movementScore++;
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

int BruteForceMover::bruteForce(Game& game, int mult, int alpha, int beta, int d, int mateIn, bool allowInvalid) {

    // Terminal nodes
    if (mateIn == 0) return 0;
    int oldAlpha = alpha;
    unsigned long long positionHash = game.zobristHash;

    // Tranposition table
    unique_ptr<ttEntry>& entry = tTable[positionHash];
    if (entry && entry->depth >= d) {
        tableHits++;
        int entryScore = entry->score;
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
    if (d == 0) return quiescence(game, -mult, alpha, beta, depth, mateIn);
 
    Move* entryMove = entry ? entry->bestMove.get() : nullptr;
    if (entryMove) {
        mateIn = min(mateIn, MATE_SCORE - abs(entry->score));
    }

    // Generate and order responses
    vector<unique_ptr<Move>> responses;
    if (allowInvalid) {
        responses = game.getPossibleMoves();
    } else {
        responses = game.getValidMoves();
    }
    if (responses.size() == 0) return -MATE_SCORE;
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
            movementScore = MATE_SCORE;
        } else {
            movementScore = -bruteForce(game, -mult, -beta, -alpha, d-1, mateIn-1, allowInvalid);
        }
        // Implies some mate was found for ME
        // We use 9900 since realistically we will never go to a depth higher than 100
        if (movementScore >= MATE_SCORE - MAX_DEPTH) {
            movementScore--;
            mateIn = MATE_SCORE - movementScore;
        } else if (movementScore <= MAX_DEPTH - MATE_SCORE) {
            movementScore++;
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

    // Add to transposition table
    unique_ptr<ttEntry> newEntry = make_unique<ttEntry>();
    newEntry->bestMove = move(bestMove);
    newEntry->score = highestScore;
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
        score = (float)bruteForce(game, game.currentTurn, NEGATIVE_INF, POSITIVE_INF, i, MAX_DEPTH, !game.useThreatMap);
        auto passedTime = high_resolution_clock::now();
        duration<double, std::milli> passedDuration = passedTime - startTime;
        if (i >= depth && passedDuration.count() >= MAX_SEARCH) {
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