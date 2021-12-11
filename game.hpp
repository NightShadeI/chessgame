#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>

#include "pieces/piece.hpp"
#include "threatTile.hpp"
#include "threatTile.hpp"
#include "board.hpp"

using namespace std;

#ifndef GAME_H
#define GAME_H

class Game {
    public:
        Game();
        int currentTurn;
        int totalMoves;
        int gameScore;
        // Use 12 for 12 different pieces. Add 1 to indicate who's turn it is
        unsigned long long zobristValues[12 * Board::width * Board::height + 1];
        unsigned long long zobristHash;
        unordered_map<unsigned long long, int> seenPositions;
        bool useThreatMap;
        static const int PAWN_WEIGHT;
        static const int KING_CLOSE_WEIGHT;
        Piece* whiteKing;
        Piece* blackKing;
        vector<unique_ptr<Move>> moveHistory;
        vector<vector<Piece*>> board;
        vector<vector<ThreatTile*>> threatMap;
        unordered_set<Piece*> pieces;
        void toggleThreats();
        void setupPiece(Piece* newPiece);
        void addThreat(Piece* attackingPiece, int xPos, int yPos);
        bool removeThreat(Piece* oldAttacker, int xPos, int yPos);
        int distanceToEnemyKing(Piece* p);
        bool inCheck();
        Piece* getPieceAt(int boardX, int boardY);
        // Retuns how many times this position has been seen
        int movePiece(Piece* p, int newX, int newY);
        void undoMove();
        vector<unique_ptr<Move>> getPossibleMoves();
        vector<unique_ptr<Move>> getCaptures();
        vector<unique_ptr<Move>> getValidMoves();
        float getGameScore();
        inline unsigned long long zobristScore(Piece* p);
    private:
        void constructBoard();
};

#endif