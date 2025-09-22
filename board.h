#include <stdint.h>
#include <stdbool.h>

typedef uint64_t u64;

//bits 0-3 are the piece type, if bit 4 is set it is black
//---cpppp

typedef enum Piece{
	P_EMPTY = -1, //for squarewise board only
	P_PAWN,
	P_KNIGHT,
	P_BISHOP,
	P_ROOK,
	P_QUEEN,
	P_KING
}Piece;

//peice masks
#define MP_BLACK 0b00010000
#define MP_PIECE 0b00001111 //ignore color

typedef struct Board{
	int squares[8][8];//x,y
	u64 whitePieces[6];
	u64 blackPieces[6];
}Board;

//bit 0 in a bitboard represents a1, bit 1 b1, bit 2 c1, ect
/*
	8 9 ...
	0 1 2 3 4 5 6 7
*/

#define BBGet(bb, x, y)  ((bb)&((u64)1<<(((y)*8)+(x))))
#define BBSet(bb, x, y)  ((bb)|=((u64)1<<(((y)*8)+(x))))

void loadFEN(Board *board, char* fen);
