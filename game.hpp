#include "pieces/piece.hpp"
#include "threatTile.hpp"
#include "threatTile.hpp"
#include <unordered_set>
#include <vector>

using namespace std;

#ifndef GAME_H
#define GAME_H

class Game {
    public:
        Game();
        int currentTurn;
        int totalMoves;
        int gameScore;
        bool useThreatMap;
        static const int PAWN_WEIGHT;
        static const int KING_CLOSE_WEIGHT;
        Piece* whiteKing;
        Piece* blackKing;
        vector<Move*> moveHistory;
        vector<vector<Piece*>> board;
        vector<vector<ThreatTile*>> threatMap;
        unordered_set<Piece*> pieces;
        void toggleThreats();
        void setupPiece(Piece* newPiece);
        void addThreat(Piece* attackingPiece, int xPos, int yPos);
        bool removeThreat(Piece* oldAttacker, int xPos, int yPos);
        int distanceToEnemyKing(Piece* p);
        Piece* getPieceAt(int boardX, int boardY);
        void movePiece(Piece* p, int newX, int newY);
        void undoMove();
        vector<Move*> getPossibleMoves();
        vector<Move*> getCaptures();
        vector<Move*> getValidMoves();
        float getGameScore();
    private:
        void constructBoard();
};

#endif