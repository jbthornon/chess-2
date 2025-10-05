#include <string.h>
#include <ctype.h>

#include "board.h"
#include "error.h"

void loadFEN(Board *board, char* fen){
	int index = 0;
	int x = 0;
	int y = 7;
	for(int i = 0; i<12; i++){
		board->bitboards[i] = (u64)0;
	}
	while(index<strlen(fen)){
		if(isdigit(fen[index])){
			for(int i = 0; i<fen[index]-'0'; i++){
				board->squares[boardIndex(x,y)] = P_EMPTY;
				x++;
			}
			index++;
		}
		if(fen[index] == '/'){
			y--;
			x = 0;
			index++;
		};
		if(isalpha(fen[index])){
			int piece = 0;
			switch(tolower(fen[index])){
				case 'p':
					piece = P_PAWN;
					break;
				case 'n':
					piece = P_KNIGHT;
					break;
				case 'b':
					piece = P_BISHOP;
					break;
				case 'r':
					piece = P_ROOK;
					break;
				case 'k':
					piece = P_KING;
					break;
				case 'q':
					piece = P_QUEEN;
					break;
			}
			if(islower(fen[index]))
				piece += 6;
			BBSet(board->bitboards[piece], boardIndex(x, y));
			board->squares[boardIndex(x,y)] = piece;
			x++;
			index++;
		}
	}	
}

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

int bitScanForward(u64 bb) {
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
u64 BBSignedShift(u64 x, int s){
	if(s>0) return x<<s;
	return x>>(-s);
}

int countBits(u64 x){
	int count = 0;
	while(x){
		count++;
		x&=x-1;//reset ls1b
	}
	return count;
}
