#include <malloc.h>
#include "movegen.h"
#include "error.h"

static const int index64[64] = {
    0,  1, 48,  2, 57, 49, 28,  3,
   61, 58, 50, 42, 38, 29, 17,  4,
   62, 55, 59, 36, 53, 51, 43, 22,
   45, 39, 33, 30, 24, 18, 12,  5,
   63, 47, 56, 27, 60, 41, 37, 16,
   54, 35, 52, 21, 44, 32, 23, 11,
   46, 26, 40, 15, 34, 20, 31, 10,
   25, 14, 19,  9, 13,  8,  7,  6
};

/**
 * bitScanForward
 * @author Martin Läuter (1997)
 *         Charles E. Leiserson
 *         Harald Prokop
 *         Keith H. Randall
 * "Using de Bruijn Sequences to Index a 1 in a Computer Word"
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @return index (0..63) of least significant one bit
 */
static int bitScanForward(u64 bb) {
	if(bb == 0) error("Bitscan forward requires input not to be 0\n");
	const u64 debruijn64 = (u64)(0x03f79d71b4cb0a89);
	/*
		multiply the isolated ls1b with a 64 bit debruijn sequence to get an index to the index64 table
		multiplying a number with a single bit set acts as a shift by the base 2 log of the number (0b1000*x is x<<4)
		because every substring of a debrujin sequence is unique, each possible ls1b will result in a unique 6 bit sequence
		in the last(most significant) 6 bits of the result, which can be used as in index in a lookup table
	*/
	return index64[((bb & -bb) * debruijn64) >> 58];
}

//positive is left shift, or up the board
static u64 BBSignedShift(u64 x, int s){
	if(s>0) return x<<s;
	return x>>(-s);
}

MoveArray moveArrayCreate(){
	MoveArray ma;
	ma.length = 0;
	ma.size = 10;//revisit this later
	ma.moves = malloc(sizeof(Move)*ma.size);
	if(ma.moves == NULL) error("failed to allocate memory for moveArray\n");
	return ma;	
}

void moveArrayDestroy(MoveArray* ma){
	free(ma->moves);
	ma->moves = NULL;
}

void moveArrayAppend(MoveArray* ma, Move move){
	if(ma->length>=ma->size){
		ma->size*=1.5;
		ma->moves = realloc(ma->moves, sizeof(Move)*ma->size);
		if(ma->moves == NULL) error("failed to allocate memory for moveArray\n");
	}
	ma->moves[ma->length++] = move;
}

static void genPawnMoves(Board* board, MoveArray* ma);

MoveArray generateMoves(Board* board){
	MoveArray ma = moveArrayCreate();
	genPawnMoves(board, &ma);
	return ma;
}


//used inside genPawnMoves
static void addPawnMoves(u64 destinations, int shift, MoveArray* ma){
	while(destinations){
		int square = bitScanForward(destinations);
		Move m = (Move){.to = square, .from = square-shift};
		moveArrayAppend(ma, m);
		destinations &= destinations-1;//reset ls1b
	}
}

static void genPawnMoves(Board* board, MoveArray* ma){
	int shift    = (board->turn == 0)? 8 : -8;
	int color    = (board->turn == 0)? 0 : 6;
	u64 homeRank = (board->turn == 0)? RANK_2 : RANK_7;
	u64 leftFile = (board->turn == 0)? FILE_A : FILE_H;
	u64 rightFile = (board->turn == 0)? FILE_H : FILE_A;

	u64 occupancy = 0;
	for(int i = 0; i<12; i++){
		occupancy |= board->bitboards[i];
	}

	u64 enemys = 0;
	for(int i = color; i<color+6; i++){
		enemys |= board->bitboards[i];
	}
	enemys = occupancy &(~enemys);

	//forward
	u64 destinations = BBSignedShift(board->bitboards[P_PAWN+color], shift)&(~occupancy);
	addPawnMoves(destinations, shift, ma);
	
	//double forward
	destinations = BBSignedShift(board->bitboards[P_PAWN+color]&homeRank, shift*2)&(~occupancy);
	addPawnMoves(destinations, shift*2, ma);

	//left captures 
	destinations = BBSignedShift(board->bitboards[P_PAWN+color]&(~leftFile), shift-1)&(enemys);
	addPawnMoves(destinations, shift-1, ma);

	//right captures 
	destinations = BBSignedShift(board->bitboards[P_PAWN+color]&(~rightFile), shift+1)&(enemys);
	addPawnMoves(destinations, shift+1, ma);
}
