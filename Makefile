all: cell
clean:
	rm -f *.o cell
.PHONY: all clean

cell: cell.c cell.h
	gcc cell.c -o cell -lSDL2 -ansi

