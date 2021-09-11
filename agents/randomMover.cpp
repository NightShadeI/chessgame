#include "randomMover.hpp"
#include <time.h>
#include <random>
#include <vector>

RandomMover::RandomMover() {
    srand(time(NULL));
}

Move& RandomMover::getMove(Game& game) {
    vector<Move*> moves = game.getValidMoves();
    int randIdx = rand() % moves.size();
    for (int i = 0; i < moves.size(); i++) {
        if (i == randIdx) continue;
        delete moves[i];
    }
    return *moves[randIdx];
}