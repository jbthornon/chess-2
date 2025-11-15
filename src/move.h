#pragma once
#include "board.h"


enum MOVE_TYPE{
	M_PROMO_KNIGHT = P_KNIGHT,
	M_PROMO_BISHOP = P_BISHOP,
	M_PROMO_ROOK = P_ROOK,
	M_PROMO_QUEEN = P_QUEEN,
	M_NORMAL,
	M_CASTLE,
};

typedef struct Move{
	int to;
	int from;
	enum MOVE_TYPE type;
}Move;

typedef struct Unmove{
	//to and from refer to the to and from of the original move
	int to;
	int from;
	int captured;
	int capturedSquare;
	char castlingRights;
	int enPassant;	
	enum MOVE_TYPE type;
}Unmove;

bool moveIsPromo(Move move);
Unmove makeMove(Board* board, Move move);
void unmakeMove(Board* board, Unmove unmove);
