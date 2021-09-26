#include "randomMover.hpp"
#include <time.h>
#include <random>
#include <vector>

RandomMover::RandomMover() {
    srand(time(NULL));
}

unique_ptr<Move> RandomMover::getMove(Game& game) {
    vector<unique_ptr<Move>> moves = game.getValidMoves();
    int randIdx = rand() % moves.size();
    return move(moves[randIdx]);
}