#include <SFML/Graphics.hpp>

#include "game.hpp"
#include "board.hpp"
#include "move.hpp"
#include "agents/agent.hpp"
#include "agents/randomMover.hpp"
#include "agents/bruteForceMover.hpp"
#include "appConfig.hpp"

using namespace std;
using namespace sf;

int main() {
    
    Image myicon;
    myicon.loadFromFile("img/icon.png");
    ContextSettings settings;
    settings.antialiasingLevel = 8;
    const int windowWidth = Board::width * Board::tileSize;
    const int windowHeight = Board::height * Board::tileSize;
    RenderWindow window(VideoMode(windowWidth, windowHeight), "Chess", Style::Close, settings);

    window.setIcon(333, 447, myicon.getPixelsPtr());
    window.setFramerateLimit(60);

    Game game;

    // Agent will look N moves into the future
    const int N = 3;
    BruteForceMover dumbestAgent(N-2);
    BruteForceMover dumberAgent(N-1);
    BruteForceMover agent(N);
    BruteForceMover strongerAgent(N+1);
    BruteForceMover strongestAgent(N+2);
    BruteForceMover deadlyAgent(N+3, true);
    BruteForceMover deadlyWaitableAgent(N+5);
    RandomMover randomAgent;

    Piece* selectedPiece = nullptr;
    int offsetX;
    int offsetY;

    while (window.isOpen()) {
        
        Vector2i mousePos = Mouse::getPosition(window);

        window.clear();
        Board::render(window, game);

        for (Piece* p: game.pieces) {
            p->render(window);
        }
        window.display();  

        Event event;  

        while (window.pollEvent(event))
        {          
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == Event::MouseButtonPressed) {
                offsetX = mousePos.x;
                offsetY = mousePos.y;
                int boardX = mousePos.x / Board::tileSize;
                int boardY = mousePos.y / Board::tileSize;
                Piece* cantidatePiece = game.getPieceAt(boardX, boardY);
                if (cantidatePiece && cantidatePiece->type == game.currentTurn) {
                    selectedPiece = cantidatePiece;
                }
            }

            if (selectedPiece && event.type == Event::MouseButtonReleased) {
                int boardX = mousePos.x / Board::tileSize;
                int boardY = mousePos.y / Board::tileSize;
                if (selectedPiece->vigorousCanDoMove(game, boardX, boardY)) {
                    game.movePiece(selectedPiece, boardX, boardY);
                } else {
                    selectedPiece->setDrag(0, 0);
                }
                selectedPiece = nullptr;
            }

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Z) {
                game.undoMove();
            }

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::W) {
                dumbestAgent.doMove(game);
            }


            if (event.type == Event::KeyPressed && event.key.code == Keyboard::R) {
                randomAgent.doMove(game);
            }

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::T) {
                strongerAgent.doMove(game);
            }

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::E) {
                dumberAgent.doMove(game);
            }

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Y) {
                strongestAgent.doMove(game);
            }

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::U) {
                deadlyAgent.doMove(game);
            }

            if (event.type == Event::KeyPressed && event.key.code == Keyboard::I) {
                deadlyWaitableAgent.doMove(game);
            }

            if (selectedPiece) {
                selectedPiece->setDrag(mousePos.x - offsetX, mousePos.y - offsetY);
            }
        }

    }

    return 0;
}