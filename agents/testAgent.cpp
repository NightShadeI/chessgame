#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>

#include "testAgent.hpp"
#include "../appConfig.hpp"

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::milliseconds;

const int transpositionTableSize = 64000;
const int immediateMateScore = 100000;
const int positiveInfinity = 9999999;
const int negativeInfinity = -positiveInfinity;

bool IsMateScore (int score) {
    const int maxMateDepth = 1000;
    return abs(score) > immediateMateScore - maxMateDepth;
}

int CorrectRetrievedMateScore (int score, int numPlySearched) {
    if (IsMateScore (score)) {
        int sign = (score > 0) - (score < 0);
        return (score * sign - numPlySearched) * sign;
    }
    return score;
}

int CorrectMateScoreForStorage (int score, int numPlySearched) {
    if (IsMateScore (score)) {
        int sign = (score > 0) - (score < 0);
        return (score * sign + numPlySearched) * sign;
    }
    return score;
}

testAgent::testAgent(int d) {
    depth = d;
}

int testAgent::movementScore(Move* m, Move* optimalMove) {
    Piece* movedPiece = m->moved;
    if (
        optimalMove &&
        optimalMove->moved == movedPiece &&
        optimalMove->xTo == optimalMove->xTo &&
        optimalMove->yTo == optimalMove->yTo
    ) return 10000;
    Piece* capturedPiece = m->captured;
    int pushScore = 10 * ((m->yTo - m->yFrom) * -movedPiece->type);
    if (capturedPiece) {
        return 10 * capturedPiece->getPieceValue() - movedPiece->getPieceValue() + pushScore;
    }
    return 0;
}

int testAgent::quiescence(Game& game, int alpha, int beta, int mult) {
    int eval = mult * game.getGameScore();
    if (eval >= beta) return beta;
    if (eval > alpha) {
        alpha = eval;
    }
    vector<unique_ptr<Move>> moves = game.getCaptures();
    std::sort(moves.begin(), moves.end(), [this](unique_ptr<Move>& a, unique_ptr<Move>& b) {
        return movementScore(a.get(), nullptr) > movementScore(b.get(), nullptr);
    });
    for (unique_ptr<Move>& m : moves) {
        game.movePiece(m->moved, m->xTo, m->yTo);
        eval = -quiescence(game, -beta, -alpha, -mult);
        game.undoMove();
        movesExplored++;
        if (eval >= beta) return beta;
        if (eval > alpha) {
            alpha = eval;
        }
    }
    return alpha;
}

// Based on negamax
int testAgent::bruteForce(Game& game, int depth, int plyFromRoot, int alpha, int beta, int mult) {
    if (plyFromRoot > 0) {
        // DOUBLE CHECK
        if (game.seenPositions[game.zobristHash] >= 2) {
            return 0;
        }
        alpha = max(alpha, -immediateMateScore + plyFromRoot);
        beta = min(beta, immediateMateScore - plyFromRoot);
        if (alpha >= beta) return alpha;
    }

    unique_ptr<ttEntry>& entry = tTable[game.zobristHash];
    if (entry && entry->depth >= depth) {
        tableHits++;
        int correctedScore = CorrectRetrievedMateScore (entry->score, plyFromRoot);
        if (entry->type == ttType::EXACT) {
            return correctedScore;
        }
        if (entry->type == ttType::UPPERBOUND && correctedScore <= alpha) {
            return correctedScore;
        }
        if (entry->type == ttType::LOWERBOUND && correctedScore >= beta) {
            return correctedScore;
        }
    }

    if (depth == 0) {
        int evaluation = quiescence(game, alpha, beta, -mult);
        return evaluation;
    }
    Move* entryMove = entry ? entry->bestMove.get() : nullptr;
    vector<unique_ptr<Move>> moves = game.getValidMoves();
    std::sort(moves.begin(), moves.end(), [this, entryMove](unique_ptr<Move>& a, unique_ptr<Move>& b) {
        return movementScore(a.get(), entryMove) > movementScore(b.get(), entryMove);
    });
    if (moves.size() == 0) {
        int mateScore = immediateMateScore - plyFromRoot;
        return -mateScore;
    }
    ttType evalType = ttType::UPPERBOUND;
    unique_ptr<Move> bestMoveInThisPosition = nullptr;
    for (unique_ptr<Move>& m : moves) {
        game.movePiece(m->moved, m->xTo, m->yTo);
        int eval = -bruteForce(game, depth - 1, plyFromRoot + 1, -beta, -alpha, -mult);
        game.undoMove();
        movesExplored++;

        if (eval >= beta) {
            unique_ptr<ttEntry> newEntry = make_unique<ttEntry>();
            newEntry->depth = depth;
            int ttScore = CorrectMateScoreForStorage(beta, plyFromRoot);
            newEntry->score = ttScore;
            newEntry->type = ttType::LOWERBOUND;
            newEntry->bestMove = move(m);
            tTable[game.zobristHash] = move(newEntry);
            return beta;
        }

        if (eval > alpha) {
            evalType = ttType::EXACT;
            bestMoveInThisPosition = move(m);
            alpha = eval;
        }
    }
    unique_ptr<ttEntry> newEntry = make_unique<ttEntry>();
    newEntry->depth = depth;
    int ttScore = CorrectMateScoreForStorage(alpha, plyFromRoot);
    newEntry->score = ttScore;
    newEntry->type = evalType;
    newEntry->bestMove = move(bestMoveInThisPosition);
    tTable[game.zobristHash] = move(newEntry);
    return alpha;
}

unique_ptr<Move> testAgent::getMove(Game& game) {
    movesExplored = 0;
    tableHits = 0;
    bestMove = nullptr;
    auto t1 = high_resolution_clock::now();
    float score;
    for (int i = 1; i <= 6; i++) {
        score = (float)bruteForce(game, i, 0, negativeInfinity, positiveInfinity, game.currentTurn)/10;
        auto t2 = high_resolution_clock::now();
        duration<double, std::milli> currentTime = t2 - t1;
    }
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;
    cout << "Explored a total of " << movesExplored << " moves in " << ms_double.count() << " ms" << endl;
    cout << "Rate of nodes : " << movesExplored/ms_double.count() << "/ms" << endl;
    cout << "Hit the table a total of " << tableHits << " times" << endl;
    cout << "Best score: " << score << endl;
    unique_ptr<ttEntry>& rootEntry = tTable[game.zobristHash];
    unique_ptr<Move> bestMove = move(rootEntry->bestMove);
    tTable.clear();
    return bestMove;
}