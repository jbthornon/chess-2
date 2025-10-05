#pragma once

#include "board.h"
#include "move.h"

typedef struct MoveArray{
	Move* moves;
	int length;//number of Moves actually used in moves
	int size; //number of Moves allocated in moves
}MoveArray;


MoveArray moveArrayCreate();
void moveArrayDestroy(MoveArray *ma);
void moveArrayAppend(MoveArray *ma, Move move);

//has to be run before generateMoves
void generateMoveTables();

void generateMoves(Board* board, MoveArray *ma);
void runTests();
