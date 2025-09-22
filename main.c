#include <stdio.h>

#include <stdlib.h>

#include "print.h"

void error(char* message){
	fprintf(stderr,"ERROR %s\n" ,message);
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]){
	char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
	Board board;
	loadFEN(&board, fen);
	printBoardDebug(&board);
	return 0;
}
