#include "agent.hpp"
#include <vector>

Agent::Agent() {
}

Move& Agent::getMove(Game& game) {
    vector<Move*> moves = game.getValidMoves();
    for (int i = 1; i < moves.size(); i++) {
        delete moves[i];
    }
    return *moves[0];
}

void Agent::doMove(Game& game) {
    Move& agentChoice = getMove(game);
    game.movePiece(agentChoice.moved, agentChoice.xTo, agentChoice.yTo);
}