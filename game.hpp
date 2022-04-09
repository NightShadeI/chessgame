#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>

#include "pieces/piece.hpp"
#include "board.hpp"
#include "move.hpp"

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
        std::uint64_t bitBoard;
        unordered_map<unsigned long long, int> seenPositions;
        static const int KING_CLOSE_WEIGHT;
        static const int KING_CASTLE_WEIGHT;
        static const int DEVELOPMENT_WEIGHT;
        Piece* whiteKing;
        Piece* blackKing;
        vector<unique_ptr<Move>> moveHistory;
        vector<vector<Piece*>> board;
        unordered_set<Piece*> pieces;
        void setupPiece(Piece* newPiece);
        int distanceToEnemyKing(Piece* p);
        bool inCheck();
        inline Piece* getPieceAt(int boardX, int boardY) { return board[boardY][boardX]; }
        // Retuns how many times this position has been seen
        int movePiece(Move& move);
        void performCastle(MoveType castleType);
        void undoCastle(MoveType castleType);
        void undoMove();
        vector<unique_ptr<Move>> getPossibleMoves();
        vector<unique_ptr<Move>> getCaptures();
        vector<unique_ptr<Move>> getValidMoves();
        inline int getGameScore() { return gameScore; }
        inline unsigned long long zobristScore(Piece* p);
        void setBit(int gameRow, int gameCol);
        void unsetBit(int gameRow, int gameCol);
    private:
        void constructBoard();
        void basicMove(const int fromX, const int fromY, const int toX, const int toY);
        void basicUndoMove(const int fromX, const int fromY, const int toX, const int toY);
};

#endif