#include "appConfig.hpp"
#include "pieces/piece.hpp"
#include "pieces/pawn.hpp"
#include "pieces/rook.hpp"
#include "pieces/knight.hpp"
#include "pieces/bishop.hpp"
#include "pieces/queen.hpp"
#include "pieces/king.hpp"
#include "move.hpp"
#include "board.hpp"
#include <iostream>

std::string DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

Piece* generatePiece(char FENchar, int x, int y) {
    int type = isupper(FENchar) ? 1 : -1;
    char lowerFENchar = type == 1 ? FENchar + 32 : FENchar;
    Piece* generatedPiece;
    switch(lowerFENchar) {
        case 'p':
            generatedPiece = new Pawn(x, y, type);
            break;
        case 'n':
            generatedPiece = new Knight(x, y, type);
            break;
        case 'b':
            generatedPiece = new Bishop(x, y, type);
            break;
        case 'r':
            generatedPiece = new Rook(x, y, type);
            break;
        case 'k':
            generatedPiece = new King(x, y, type);
            break;
        case 'q':
            generatedPiece = new Queen(x, y, type);
            break;
        default:
            throw std::invalid_argument("Unsupported FEN character");
    }
    return generatedPiece;
}

void setupBoard(Game& game, string& FENstring) {
    int gameRow = 0;
    int gameCol = 0;
    for (char c: FENstring) {
        if (c == ' ') break;
        if (c == '/') {
            gameRow++;
            gameCol = 0;
            continue;
        }
        if (isdigit(c)) {
            gameCol += (c - '0');
        } else {
            Piece* generatedPiece = generatePiece(c, gameCol, gameRow);
            game.setupPiece(generatedPiece);
            gameCol++;
        }
    }
    // TODO: Add support for castling, enpessant etc
}

vector<vector<ThreatTile*>> setupThreatMap(Game& game) {
    vector<vector<ThreatTile*>> threatMap;
    threatMap.resize(Board::height);
    for (auto& row : threatMap) {
        row.resize(Board::width);
        for (int i = 0; i < row.size(); i++) {
            ThreatTile* tile = new ThreatTile;
            tile->whiteCount = 0;
            tile->blackCount = 0;
            row[i] = tile;
        }
    }
    for (Piece* p : game.pieces) {
        vector<Move*> pieceMoves = p->getMoves(game);
        for (Move* m : pieceMoves) {
            if (p->getPieceName() != "Pawn" || p->xPos != m->xTo) {
                game.addThreat(p, m->xTo, m->yTo);
            } else {
            }
            delete m;
        }
    }
    return threatMap;
}