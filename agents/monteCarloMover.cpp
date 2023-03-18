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

Move MonteCarloMover::getMove(Game& game) {
    unordered_map<Move*, int> totalScore;
    unordered_map<Move*, int> timesExplored;
    vector<Move> moves = game.getValidMoves();
    const int numMoves = moves.size();
    for (int t = 1; t <= trails; t++) {
        int randIdx = rand() % numMoves;
        Move& selectedMove = moves[randIdx];
        game.movePiece(selectedMove);
        int d;
        for (d = 1; d < depth; d++) {
            vector<Move> someResponses = game.getValidMoves();
            if (someResponses.size() == 0) break;
            randIdx = rand() % someResponses.size();
            Move& randomMove = someResponses[randIdx];
            game.movePiece(randomMove);
        }
        totalScore[&selectedMove] += game.getGameScore() > 0 ? 1 : -1;
        timesExplored[&selectedMove]++;
        for (int i = 1; i < d; i++) {
            game.undoMove();
        }
        game.undoMove();
    }
    Move& bestMove = moves[0];
    float bestAverage = 2000;
    for (Move& m : moves) {
        int score = totalScore[&m] * game.currentTurn;
        int trials = timesExplored[&m];
        if (trials == 0) continue;
        float average = (float)score/trials;
        if (average < bestAverage) {
            bestAverage = average;
            bestMove = m;
        }
    }
    return bestMove;
}