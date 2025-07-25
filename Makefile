CC=gcc -g
PROGRAMS=solveuri

all: $(PROGRAMS)

Graph.o: Graph.c Graph.h
	$(CC) -c Graph.c -Wall

Utils.o: Utils.c Utils.h
	$(CC) -c Utils.c -Wall 

Solver.o: Solver.c Solver.h
	$(CC) -c Solver.c -Wall

GraphParser.o: GraphParser.c GraphParser.h
	$(CC) -c GraphParser.c -Wall

Rectangle.o: Rectangle.c Rectangle.h
	$(CC) -c Rectangle.c -Wall

Editor.o: Editor.c Editor.h
	$(CC) -c Editor.c -Wall

solveuri: Solveuri.o Graph.o Solver.o Utils.o Rectangle.o GraphParser.o Editor.o
	$(CC) Solveuri.o Graph.o Solver.o Utils.o GraphParser.o Rectangle.o Editor.o -o solveuri -lm -Wall


	
clean:
	rm -f $(PROGRAMS) *.o
