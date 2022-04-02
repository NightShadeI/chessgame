all: compile link run

compile:
	g++ -O3 -I sfml/src/include -c game.cpp -std=c++2a
	g++ -O3 -I sfml/src/include -c main.cpp -std=c++2a
	g++ -O3 -I sfml/src/include -c board.cpp -std=c++2a
	g++ -O3 -I sfml/src/include -c appConfig.cpp -std=c++2a
	g++ -O3 -I sfml/src/include -c pieces/piece.cpp -std=c++2a
	g++ -O3 -I sfml/src/include -c pieces/pawn.cpp -std=c++2a
	g++ -O3 -I sfml/src/include -c pieces/rook.cpp -std=c++2a
	g++ -O3 -I sfml/src/include -c pieces/knight.cpp -std=c++2a
	g++ -O3 -I sfml/src/include -c pieces/bishop.cpp -std=c++2a
	g++ -O3 -I sfml/src/include -c pieces/queen.cpp -std=c++2a
	g++ -O3 -I sfml/src/include -c pieces/king.cpp -std=c++2a
	g++ -O3 -I sfml/src/include -c agents/agent.cpp -std=c++2a
	g++ -O3 -I sfml/src/include -c agents/randomMover.cpp -std=c++2a
	g++ -O3 -I sfml/src/include -c agents/bruteForceMover.cpp -std=c++2a


link: 
	g++ -O3 -std=c++2a main.o game.o board.o appConfig.o piece.o pawn.o rook.o knight.o bishop.o queen.o king.o agent.o bruteForceMover.o randomMover.o -o libs/main -L sfml/src/lib -lsfml-graphics -lsfml-window -lsfml-system

run:
	./libs/main.exe