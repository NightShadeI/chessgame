#include "monteCarloMover.hpp"
#include <time.h>
#include <random>
#include <unordered_map>
#include <vector>

MonteCarloMover::MonteCarloMover(int d, int t) {
    depth = d;
    trails = t;
    srand(time(NULL));
}

unique_ptr<Move> MonteCarloMover::getMove(Game& game) {
    unordered_map<Move*, int> totalScore;
    unordered_map<Move*, int> timesExplored;
    vector<unique_ptr<Move>> moves = game.getValidMoves();
    const int numMoves = moves.size();
    for (int t = 1; t <= trails; t++) {
        int randIdx = rand() % numMoves;
        unique_ptr<Move>& selectedMove = moves[randIdx];
        game.movePiece(*selectedMove);
        int d;
        for (d = 1; d < depth; d++) {
            vector<unique_ptr<Move>> someResponses = game.getValidMoves();
            if (someResponses.size() == 0) break;
            randIdx = rand() % someResponses.size();
            unique_ptr<Move>& randomMove = someResponses[randIdx];
            game.movePiece(*randomMove);
        }
        totalScore[selectedMove.get()] += game.getGameScore() > 0 ? 1 : -1;
        timesExplored[selectedMove.get()]++;
        for (int i = 1; i < d; i++) {
            game.undoMove();
        }
        game.undoMove();
    }
    unique_ptr<Move>& bestMove = moves[0];
    float bestAverage = 2000;
    for (unique_ptr<Move>& m : moves) {
        int score = totalScore[m.get()] * game.currentTurn;
        int trials = timesExplored[m.get()];
        if (trials == 0) continue;
        float average = (float)score/trials;
        if (average < bestAverage) {
            bestAverage = average;
            bestMove = move(m);
        }
    }
    return move(bestMove);
}