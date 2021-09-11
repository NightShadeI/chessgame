#include "bruteForceMover.hpp"
#include <vector>
#include <iostream>
#include <algorithm>

#include <chrono>

using namespace std;
using std::chrono::high_resolution_clock;
using std::chrono::duration;
using std::chrono::milliseconds;

BruteForceMover::BruteForceMover(int d) {
    depth = d;
}

// Based on negamax
int BruteForceMover::bruteForce(Game& game, int mult, int alpha, int beta, int d) {
    if (d >= depth) return mult * game.getGameScore();
    vector<Move*> responses = game.getValidMoves();
    // Check-mate!
    if (responses.size() == 0) return 10000;
    std::sort(responses.begin(), responses.end(), [](Move* a, Move* b) {
        if (a->captured && !b->captured) return true;
        if (!a->captured) return false;
        return abs(a->captured->getPieceValue()) > abs(b->captured->getPieceValue());
    });
    int highestScore = -100000;
    for (Move* r : responses) {
        game.movePiece(r->moved, r->xTo, r->yTo);
        int movementScore = bruteForce(game, -mult, -beta, -alpha, d+1);
        // if (d == 0) {
        //     cout << r->moved->getPieceName() << " " << r->xTo << " " << r->yTo << " " << movementScore << " " << alpha << " " << beta << endl;
        // }
        // if (d == 1) {
        //     cout << "1: " << r->moved->getPieceName() << " " << r->xTo << " " << r->yTo << " " << movementScore << " " << alpha << " " << beta << endl;
        // }
        if (movementScore > highestScore) {
            highestScore = movementScore;
            alpha = movementScore;
            if (d == 0) {
                bestMove = r;
            }
        }
        game.undoMove();
        movesExplored++;
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
    float score = -(float)bruteForce(game, game.currentTurn, -100000, 100000)/10;
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;
    cout << "Explored a total of " << movesExplored << " moves in " << ms_double.count() << " ms" << endl;
    cout << "Best score: " << score << endl;
    return *bestMove;
}