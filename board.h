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

#define RANK_1 0x00000000000000FF
#define RANK_2 0x000000000000FF00
#define RANK_3 0x0000000000FF0000
#define RANK_4 0x00000000FF000000
#define RANK_5 0x000000FF00000000
#define RANK_6 0x0000FF0000000000
#define RANK_7 0x00FF000000000000
#define RANK_8 0xFF00000000000000

#define FILE_A 0x0101010101010101
#define FILE_B 0x0202020202020202
#define FILE_C 0x0404040404040404
#define FILE_D 0x0808080808080808
#define FILE_E 0x1010101010101010
#define FILE_F 0x2020202020202020
#define FILE_G 0x4040404040404040
#define FILE_H 0x8080808080808080

void loadFEN(Board *board, char* fen);
