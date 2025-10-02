#pragma once
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
	int squares[64];
	u64 bitboards[12];
	int turn; //0 for white, 1 for black

	//stuff for move generation
	u64 friendlyBB;
	u64 enemyBB;
}Board;

//bit 0 in a bitboard represents a1, bit 1 b1, bit 2 c1, ect
//board indices are in the same order
/*
	8 9 ...
	0 1 2 3 4 5 6 7
*/

#define boardIndex(x,y) (((y)*8)+(x))
#define BBGet(bb, index) ((bb)&((u64)1<<(index)))
#define BBSet(bb, index) ((bb)|=((u64)1<<(index)))
#define BBReset(bb, index) ((bb)&=~((u64)1<<(index)))

#define RANK_1 ((u64)0x00000000000000FF)
#define RANK_2 ((u64)0x000000000000FF00)
#define RANK_3 ((u64)0x0000000000FF0000)
#define RANK_4 ((u64)0x00000000FF000000)
#define RANK_5 ((u64)0x000000FF00000000)
#define RANK_6 ((u64)0x0000FF0000000000)
#define RANK_7 ((u64)0x00FF000000000000)
#define RANK_8 ((u64)0xFF00000000000000)

#define FILE_A ((u64)0x0101010101010101)
#define FILE_B ((u64)0x0202020202020202)
#define FILE_C ((u64)0x0404040404040404)
#define FILE_D ((u64)0x0808080808080808)
#define FILE_E ((u64)0x1010101010101010)
#define FILE_F ((u64)0x2020202020202020)
#define FILE_G ((u64)0x4040404040404040)
#define FILE_H ((u64)0x8080808080808080)

void loadFEN(Board *board, char* fen);

int bitScanForward(u64 bb);
u64 BBSignedShift(u64 x, int s);
