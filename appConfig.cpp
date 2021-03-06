#include <random>

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

std::string DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
// std::string DEFAULT_FEN = "8/k7/3p4/p2P1p2/P2P1P2/8/8/K7";

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
            if (!generatedPiece->isStartingLocation()) {
                generatedPiece->totalMoves = 1;
            }
            game.setupPiece(generatedPiece);
            game.setBit(gameRow, gameCol);
            if (generatedPiece->getPieceType() == PieceName::KING) {
                if (generatedPiece->type == 1) {
                    game.whiteKing = generatedPiece;
                } else {
                    game.blackKing = generatedPiece;
                }
            }
            gameCol++;
        }
    }
    // TODO: Add support for castling, enpessant etc
}

void setupHashes(Game& game) {
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<unsigned long long> distribution;
    game.zobristHash = 0;
    const size_t hashArrSize = sizeof(game.zobristValues)/sizeof(unsigned long long);
    for (int i = 0; i < hashArrSize; i++) {
        game.zobristValues[i] = distribution(generator);
    }
}