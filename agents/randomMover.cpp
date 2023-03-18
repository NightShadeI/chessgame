#include "randomMover.hpp"
#include <time.h>
#include <random>
#include <vector>

RandomMover::RandomMover() {
    srand(time(NULL));
}

Move RandomMover::getMove(Game& game) {
    vector<Move> moves = game.getValidMoves();
    int randIdx = rand() % moves.size();
    return moves[randIdx];
}