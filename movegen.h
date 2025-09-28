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

//returns an array of all legal moves
MoveArray generateMoves(Board* board); 
