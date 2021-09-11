#include "piecePropagators.hpp"
#include "propagator.hpp"

namespace PiecePropagators {

    void knightSetup(Game& game, Piece* knight, bool destructure) {
        const std::vector<std::pair<int, int>> dirs = {
            {-2,  1},
            {-1,  2},
            { 1,  2},
            { 2,  1},
            { 2, -1},
            { 1, -2},
            {-1, -2},
            {-2, -1}
        };
        for (auto& p: dirs) {
            int nextX = knight->xPos + p.first;
            int nextY = knight->yPos + p.second;
            if (destructure) {
                game.removeThreat(knight, nextX, nextY);
            } else {
                game.addThreat(knight, nextX, nextY);
            }
        }
    }

    void bishopSetup(Game& game, Piece* bishop, bool destructure) {
        Propagator propagator(bishop, nullptr);
        const std::vector<std::pair<int, int>> dirs = {
            {-1,  1},
            { 1,  1},
            { 1, -1},
            {-1, -1}
        };
        for (auto& p: dirs) {
            propagator.updater(game, bishop->xPos + p.first, bishop->yPos + p.second,  p.first, p.second, !destructure);
        }
    }

    void rookSetup(Game& game, Piece* rook, bool destructure) {
        Propagator propagator(rook, nullptr);
        const std::vector<std::pair<int, int>> dirs = {
            { 0,  1},
            { 1,  0},
            { 0, -1},
            {-1,  0}
        };
        for (auto& p: dirs) {
            propagator.updater(game, rook->xPos + p.first, rook->yPos + p.second, p.first, p.second, !destructure);
        }
    }

    void queenSetup(Game& game, Piece* queen, bool destructure) {
        // A queen is just really a combination of a rook and a bishop
        rookSetup(game, queen, destructure);
        bishopSetup(game, queen, destructure);
    }

    void kingSetup(Game& game, Piece* king, bool destructure) {
        const std::vector<std::pair<int, int>> dirs = {
            {-1,  1},
            { 0,  1},
            { 1,  1},
            { 1,  0},
            { 1, -1},
            { 0, -1},
            {-1, -1},
            {-1,  0}
        };
        for (auto& p: dirs) {
            int nextX = king->xPos + p.first;
            int nextY = king->yPos + p.second;
            if (destructure) {
                game.removeThreat(king, nextX, nextY);
            } else {
                game.addThreat(king, nextX, nextY);
            }
        }
    }

    void pawnSetup(Game& game, Piece* pawn, bool destructure) {
        if (destructure) {
            game.removeThreat(pawn, pawn->xPos - 1, pawn->yPos - pawn->type);
            game.removeThreat(pawn, pawn->xPos + 1, pawn->yPos - pawn->type);
        } else {
            game.addThreat(pawn, pawn->xPos - 1, pawn->yPos - pawn->type);
            game.addThreat(pawn, pawn->xPos + 1, pawn->yPos - pawn->type);
        }
    }

    void knightUpdater(Game& game, Piece* knight, int newX, int newY) {
        const std::vector<std::pair<int, int>> dirs = {
            {-2,  1},
            {-1,  2},
            { 1,  2},
            { 2,  1},
            { 2, -1},
            { 1, -2},
            {-1, -2},
            {-2, -1}
        };
        for (auto& p: dirs) {
            int prevX = knight->xPos + p.first;
            int prevY = knight->yPos + p.second;
            int nextX = newX + p.first;
            int nextY = newY + p.second;
            game.removeThreat(knight, prevX, prevY);
            game.addThreat(knight, nextX, nextY);
        }
    }

    void bishopUpdater(Game& game, Piece* bishop, int newX, int newY) {
        Propagator propagator(bishop, nullptr);
        int xDiff = newX - bishop->xPos;
        int yDiff = newY - bishop->yPos;
        // negative gradient
        if (xDiff > 0 == yDiff > 0) {
            propagator.updater(game, bishop->xPos + 1, bishop->yPos - 1,  1, -1, false);
            propagator.updater(game, bishop->xPos - 1, bishop->yPos + 1, -1,  1, false);
            propagator.updater(game, newX + 1, newY - 1,  1, -1, true);
            propagator.updater(game, newX - 1, newY + 1, -1,  1, true);
        } else {
            propagator.updater(game, bishop->xPos + 1, bishop->yPos + 1,  1,   1, false);
            propagator.updater(game, bishop->xPos - 1, bishop->yPos - 1, -1,  -1, false);
            propagator.updater(game, newX + 1, newY + 1,  1,   1, true);
            propagator.updater(game, newX - 1, newY - 1, -1,  -1, true);
        }
    }

    void rookUpdater(Game& game, Piece* rook, int newX, int newY) {
        Propagator propagator(rook, nullptr);
        if (rook->xPos == newX) {
            propagator.updater(game, rook->xPos, rook->yPos - 1, 0, -1, false);
            propagator.updater(game, rook->xPos, rook->yPos + 1, 0,  1, false);
            propagator.updater(game, newX, newY - 1, 0, -1, true);
            propagator.updater(game, newX, newY + 1, 0,  1, true);
        } else {
            propagator.updater(game, rook->xPos + 1, rook->yPos,  1, 0, false);
            propagator.updater(game, rook->xPos - 1, rook->yPos, -1, 0, false);
            propagator.updater(game, newX + 1, newY,  1, 0, true);
            propagator.updater(game, newX - 1, newY, -1, 0, true);
        }
    }

    void queenUpdater(Game& game, Piece* queen, int newX, int newY) {
        Propagator propagator(queen, nullptr);
        // Check if the queen did a rook or bishop move
        if (queen->xPos == newX || queen->yPos == newY) {
            rookUpdater(game, queen, newX, newY);
            // Remove old diagonal threats
            propagator.updater(game, queen->xPos - 1, queen->yPos - 1, -1, -1, false);
            propagator.updater(game, queen->xPos - 1, queen->yPos + 1, -1,  1, false);
            propagator.updater(game, queen->xPos + 1, queen->yPos - 1,  1, -1, false);
            propagator.updater(game, queen->xPos + 1, queen->yPos + 1,  1,  1, false);

            // Add new diagonal threats
            propagator.updater(game, newX - 1, newY - 1, -1, -1, true);
            propagator.updater(game, newX - 1, newY + 1, -1,  1, true);
            propagator.updater(game, newX + 1, newY - 1,  1, -1, true);
            propagator.updater(game, newX + 1, newY + 1,  1,  1, true);
        } else {
            bishopUpdater(game, queen, newX, newY);
            // Remove old vertical and horizontal threats
            propagator.updater(game, queen->xPos, queen->yPos - 1, 0, -1, false);
            propagator.updater(game, queen->xPos, queen->yPos + 1, 0,  1, false);
            propagator.updater(game, queen->xPos + 1, queen->yPos - 1,  1, 0, false);
            propagator.updater(game, queen->xPos - 1, queen->yPos + 1, -1, 0, false);

            // Add new vertical and horizontal threats
            propagator.updater(game, newX, newY - 1, 0, -1, true);
            propagator.updater(game, newX, newY + 1, 0,  1, true);
            propagator.updater(game, newX + 1, newY - 1,  1, 0, true);
            propagator.updater(game, newX - 1, newY + 1, -1, 0, true);
        }
    }

    void kingUpdater(Game& game, Piece* king, int newX, int newY) {
        int xDir = newX - king->xPos;
        int yDir = newY - king->yPos;

        if (xDir) {
            game.addThreat(king, newX + xDir, newY - 1);
            game.addThreat(king, newX + xDir, newY);
            game.addThreat(king, newX + xDir, newY + 1);
            game.removeThreat(king, king->xPos - xDir, newY - 1);
            game.removeThreat(king, king->xPos - xDir, newY);
            game.removeThreat(king, king->xPos - xDir, newY + 1);
        }

        if (yDir) {
            game.addThreat(king, newX - 1, newY + yDir);
            game.addThreat(king, newX, newY + yDir);
            game.addThreat(king, newX + 1, newY + yDir);
            game.removeThreat(king, newX - 1, newY - yDir);
            game.removeThreat(king, newX, newY - yDir);
            game.removeThreat(king, newX + 1, newY - yDir);
        }

        // Where it moves is no longer a threat, where it was is
        game.removeThreat(king, newX, newY);
        game.addThreat(king, king->xPos, king->yPos);
    }

    void pawnUpdater(Game& game, Piece* pawn, int newX, int newY) {
        game.removeThreat(pawn, pawn->xPos + 1, pawn->yPos - pawn->type);
        game.removeThreat(pawn, pawn->xPos - 1, pawn->yPos - pawn->type);
        game.addThreat(pawn, newX + 1, newY - pawn->type);
        game.addThreat(pawn, newX - 1, newY - pawn->type);
    }
}