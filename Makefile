all:
	g++ -Isrc/Include -Lsrc/lib -o main main.cpp game.cpp abilities.cpp player.cpp -lSDL2 -lSDL2_image