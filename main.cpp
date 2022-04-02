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
        if (selectedPiece) {
            selectedPiece->setDrag(mousePos.x - offsetX, mousePos.y - offsetY);
        }
        window.display();  

        Event event;  
        while (window.pollEvent(event))
        {
            switch (event.type) {
                case Event::Closed:
                    window.close();
                    break;
                case Event::MouseButtonPressed:
                {
                    offsetX = mousePos.x;
                    offsetY = mousePos.y;
                    int boardX = mousePos.x / Board::tileSize;
                    int boardY = mousePos.y / Board::tileSize;
                    Piece* cantidatePiece = game.getPieceAt(boardX, boardY);
                    if (cantidatePiece && cantidatePiece->type == game.currentTurn) {
                        selectedPiece = cantidatePiece;
                    }
                    break;
                }
                case Event::MouseButtonReleased:
                {
                    if (!selectedPiece) break;
                    int boardX = mousePos.x / Board::tileSize;
                    int boardY = mousePos.y / Board::tileSize;
                    if (selectedPiece->vigorousCanDoMove(game, boardX, boardY)) {
                        game.movePiece(selectedPiece, boardX, boardY);
                    } else {
                        selectedPiece->setDrag(0, 0);
                    }
                    selectedPiece = nullptr;
                    break;
                }
                case Event::KeyPressed:
                    switch (event.key.code) {
                        case Keyboard::Z:
                            // No move is before the first move, so ignore if no moves done
                            if (game.totalMoves == 0) break;
                            game.undoMove();
                            break;
                        case Keyboard::R:
                            randomAgent.doMove(game);
                            break;
                        case Keyboard::W:
                            dumbestAgent.doMove(game);
                            break;
                        case Keyboard::E:
                            dumberAgent.doMove(game);
                            break;
                        case Keyboard::T:
                            strongerAgent.doMove(game);
                            break;
                        case Keyboard::Y:
                            strongestAgent.doMove(game);
                            break;
                        case Keyboard::U:
                            deadlyAgent.doMove(game);
                            break;
                        case Keyboard::I:
                            deadlyWaitableAgent.doMove(game);
                            break;
                    }
                    break;
            } 
        }
    }
    return 0;
}