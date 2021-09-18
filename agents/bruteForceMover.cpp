#include <vector>
#include <iostream>
#include <algorithm>
#include <chrono>

#include "bruteForceMover.hpp"
#include "../appConfig.hpp"

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::milliseconds;

BruteForceMover::BruteForceMover(int d) {
    depth = d;
}

int BruteForceMover::movementScore(Move* m) {
    Piece* movedPiece = m->moved;
    Piece* capturedPiece = m->captured;
    int pushScore = 10 * ((m->yTo - m->yFrom) * -movedPiece->type);
    if (capturedPiece) {
        return 10 * capturedPiece->getPieceValue() - movedPiece->getPieceValue() + pushScore;
    }
    return pushScore;
}

int BruteForceMover::quiescence(Game& game, int mult, int alpha, int beta, int d) {
    movesExplored++;
    int nulMove = mult * game.getGameScore();
    if (nulMove >= beta) return -nulMove;
    vector<Move*> responses = game.getCaptures();
    // Check-mate!
    // We subtract d to prioritise checkmates in fewer moves
    if (responses.size() == 0) return -nulMove;
    std::sort(responses.begin(), responses.end(), [this](Move* a, Move* b) {
        return movementScore(a) > movementScore(b);
    });
    int highestScore = nulMove;
    alpha = max(alpha, highestScore);
    for (Move* r : responses) {
        game.movePiece(r->moved, r->xTo, r->yTo);
        int movementScore;
        if (r->captured->getPieceName() == "King") {
            movementScore = 10000 - 10*d;
        } else {
            movementScore = quiescence(game, -mult, -beta, -alpha, d+1);
        }
        if (movementScore > highestScore) {
            highestScore = movementScore;
            alpha = movementScore;
        }
        game.undoMove();
        if (alpha >= beta) break;
    }
    for (Move* r : responses) {
        delete r;
    }
    return -highestScore;
}

// Based on negamax
int BruteForceMover::bruteForce(Game& game, int mult, int alpha, int beta, int d, bool allowInvalid) {
    movesExplored++;
    if (d >= depth) return quiescence(game, -mult, alpha, beta, d);
    vector<Move*> responses;
    if (allowInvalid) {
        responses = game.getPossibleMoves();
    } else {
        responses = game.getValidMoves();
    }
    // Check-mate!
    // We subtract d to prioritise checkmates in fewer moves
    if (responses.size() == 0) return 10000 - 10*d;
    std::sort(responses.begin(), responses.end(), [this](Move* a, Move* b) {
        return movementScore(a) > movementScore(b);
    });
    int highestScore = -100000;
    for (Move* r : responses) {
        game.movePiece(r->moved, r->xTo, r->yTo);
        int movementScore;
        Piece* capturedPiece = r->captured;
        if (capturedPiece && capturedPiece->getPieceName() == "King") {
            // Value of a king, again d subtracted to prioritise fewer move mates
            movementScore = 10000 - 10*d; 
        } else {
            movementScore = bruteForce(game, -mult, -beta, -alpha, d+1, allowInvalid);
        }
        if (movementScore > highestScore) {
            highestScore = movementScore;
            alpha = movementScore;
            if (d == 0) {
                bestMove = r;
            }
        }
        game.undoMove();
        if (alpha >= beta) break;
    }
    for (Move* r : responses) {
        if (r == bestMove) continue;
        delete r;
    }
    return -highestScore;
}

Move& BruteForceMover::getMove(Game& game) {
    movesExplored = 0;
    bestMove = nullptr;
    auto t1 = high_resolution_clock::now();
    float score = -(float)bruteForce(game, game.currentTurn, -100000, 100000, 0, !game.useThreatMap)/10;
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;
    cout << "Explored a total of " << movesExplored << " moves in " << ms_double.count() << " ms" << endl;
    cout << "Rate of nodes : " << movesExplored/ms_double.count() << "/ms" << endl;
    cout << "Best score: " << score << endl;
    return *bestMove;
}