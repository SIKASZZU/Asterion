all:
	g++ -Isrc/Include -Lsrc/lib -o main *.cpp -lSDL2 -lSDL2_image