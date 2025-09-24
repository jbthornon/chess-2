#include <stdint.h>
#include <stdbool.h>

typedef uint64_t u64;


//a black peice is a white peice +6
typedef enum Piece{
	P_EMPTY = -1, //for squarewise board only
	P_PAWN,
	P_KNIGHT,
	P_BISHOP,
	P_ROOK,
	P_QUEEN,
	P_KING
}Piece;


typedef struct Board{
	int squares[8][8];//x,y
	u64 bitboards[12];
}Board;

//bit 0 in a bitboard represents a1, bit 1 b1, bit 2 c1, ect
/*
	8 9 ...
	0 1 2 3 4 5 6 7
*/

#define BBGet(bb, x, y)  ((bb)&((u64)1<<(((y)*8)+(x))))
#define BBSet(bb, x, y)  ((bb)|=((u64)1<<(((y)*8)+(x))))

void loadFEN(Board *board, char* fen);
