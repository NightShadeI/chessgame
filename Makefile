all: compile link run

compile:
	g++ -I sfml/src/include -c game.cpp
	g++ -I sfml/src/include -c main.cpp
	g++ -I sfml/src/include -c board.cpp
	g++ -I sfml/src/include -c appConfig.cpp
	g++ -I sfml/src/include -c pieces/piece.cpp
	g++ -I sfml/src/include -c pieces/pawn.cpp
	g++ -I sfml/src/include -c pieces/rook.cpp
	g++ -I sfml/src/include -c pieces/knight.cpp
	g++ -I sfml/src/include -c pieces/bishop.cpp
	g++ -I sfml/src/include -c pieces/queen.cpp
	g++ -I sfml/src/include -c pieces/king.cpp
	g++ -I sfml/src/include -c agents/agent.cpp
	g++ -I sfml/src/include -c agents/randomMover.cpp
	g++ -I sfml/src/include -c agents/bruteForceMover.cpp
	g++ -I sfml/src/include -c agents/monteCarloMover.cpp


link: 
	g++.exe -static -static-libgcc -static-libstdc++ main.o game.o board.o appConfig.o piece.o pawn.o rook.o knight.o bishop.o queen.o king.o agent.o bruteForceMover.o randomMover.o monteCarloMover.o -o libs/main -L sfml/src/lib -lsfml-graphics -lsfml-window -lsfml-system

run:
	./libs/main.exe