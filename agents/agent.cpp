#include <vector>
#include <memory>

#include "agent.hpp"

Agent::Agent() {
}

Move Agent::getMove(Game& game) {
    vector<Move> moves = game.getValidMoves();
    return moves[0];
}

void Agent::doMove(Game& game) {
    Move agentChoice = getMove(game);
    game.movePiece(agentChoice);
}