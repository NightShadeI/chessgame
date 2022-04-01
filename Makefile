all: compile link run

compile:
	g++ -O3 -I sfml/src/include -c game.cpp
	g++ -O3 -I sfml/src/include -c main.cpp
	g++ -O3 -I sfml/src/include -c board.cpp
	g++ -O3 -I sfml/src/include -c appConfig.cpp
	g++ -O3 -I sfml/src/include -c pieces/piece.cpp
	g++ -O3 -I sfml/src/include -c pieces/pawn.cpp
	g++ -O3 -I sfml/src/include -c pieces/rook.cpp
	g++ -O3 -I sfml/src/include -c pieces/knight.cpp
	g++ -O3 -I sfml/src/include -c pieces/bishop.cpp
	g++ -O3 -I sfml/src/include -c pieces/queen.cpp
	g++ -O3 -I sfml/src/include -c pieces/king.cpp
	g++ -O3 -I sfml/src/include -c agents/agent.cpp
	g++ -O3 -I sfml/src/include -c agents/randomMover.cpp
	g++ -O3 -I sfml/src/include -c agents/bruteForceMover.cpp


link: 
	g++.exe -O3 -static -static-libgcc -static-libstdc++ main.o game.o board.o appConfig.o piece.o pawn.o rook.o knight.o bishop.o queen.o king.o agent.o bruteForceMover.o randomMover.o -o libs/main -L sfml/src/lib -lsfml-graphics -lsfml-window -lsfml-system

run:
	./libs/main.exe