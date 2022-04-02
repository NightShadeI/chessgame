#include <vector>
#include <memory>

#include "agent.hpp"

Agent::Agent() {
}

unique_ptr<Move> Agent::getMove(Game& game) {
    vector<unique_ptr<Move>> moves = game.getValidMoves();
    return move(moves[0]);
}

void Agent::doMove(Game& game) {
    unique_ptr<Move> agentChoice = getMove(game);
    game.movePiece(*agentChoice);
}