#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "error.h"
#include "print.h"

static int charToPiece(char c){
	int piece = 0;
	switch(tolower(c)){
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
		default:
			error("charToPiece(), received invalid char");
	}
	if(islower(c))
		piece += 6;
	return piece;
}

void loadFEN(Board *board, char* fen){
	int len = strlen(fen);
	int index = 0;
	for(int i = 0; i<12; i++){
		board->bitboards[i] = (u64)0;
	}
	for(int i = 0; i<64; i++) board->squares[i] = P_EMPTY;
	for(int i = 0; i<4; i++) board->canCastle[0] = false;
	board->halfmoveClock = 0;
	board->fullmoveClock = 0;
	
	for(int y = 7; y>=0; y--){
		int x = 0;
		while(x<8){
			if(isalpha(fen[index])){
				int piece = charToPiece(fen[index]);
				board->squares[BOARD_INDEX(x,y)] = piece;
				SET_BIT64(board->bitboards[piece], BOARD_INDEX(x,y));
				x++;
				index++;
			}
			if(isdigit(fen[index])){
				x += fen[index] - '0';
				index++;
			}
		}
		if(fen[index] != '/' && y!=0) error("loadFEN() missing '/'");
		index++;
	}
	if(index>=(len-1)) error("loadFEN() fen is cut short or invalid");

	if(fen[index] == 'w') board->whitesTurn = true;
	else board->whitesTurn = false;
	index++;
	
	index++;//skip ' '
	
	if(index>=(len-1)) error("loadFEN() fen is cut short or invalid");
	if(fen[index] == '-'){
		index++;
	}else{
		while(fen[index] != ' '){
			if(fen[index] == 'K') board->canCastle[0] = true;
			if(fen[index] == 'Q') board->canCastle[1] = true;
			if(fen[index] == 'k') board->canCastle[2] = true;
			if(fen[index] == 'q') board->canCastle[3] = true;
			index++;
		}
	}
	index++;//skip ' '
	
	updatePerspectiveVariables(board);

	if(index>=(len-1)) return; //accept strings that dont include half/full move counters
	board->halfmoveClock = atoi(&fen[index]);
	while(fen[index] != ' ') if(index++>=(len-1)) return;

	index++;//skip ' '

	if(index>=(len-1)) return;
	board->fullmoveClock = atoi(&fen[index]);
}

void makeFen(Board *board, char* fen){
	int index = 0;
	for(int y = 7; y>=0; y--){
		int emptyCount = 0;
		for(int x = 0; x<8; x++){
			int piece = board->squares[BOARD_INDEX(x,y)];
			if(piece == P_EMPTY) emptyCount++;
			if(piece != P_EMPTY){
				if(emptyCount != 0) fen[index++] = '0' + emptyCount;
				emptyCount = 0;
				fen[index++] = pieceToChar(piece);
			}
		}
		if(emptyCount != 0) fen[index++] = '0' + emptyCount;
		if(y != 0) fen[index++] = '/';
	}

	fen[index++] = ' ';
	fen[index++] = (board->whitesTurn)? 'w':'b';

	fen[index++] = ' ';
	if(board->canCastle[0]) fen[index++] = 'K';
	if(board->canCastle[1]) fen[index++] = 'Q';
	if(board->canCastle[2]) fen[index++] = 'k';
	if(board->canCastle[3]) fen[index++] = 'q';
	if(fen[index-1] == ' ') fen[index++] = '-';

	sprintf(&fen[index], " %d %d", board->halfmoveClock, board->fullmoveClock);
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
		in the last(most significant) 6 bits of the result, which can be used as an index in a lookup table
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

void updatePerspectiveVariables(Board* board){
	board->color = board->whitesTurn? 0 : 6;
	board->enemyColor = board->whitesTurn? 6 : 0;
	board->occupancy = 0;
	for(int i = 0; i<12; i++){
		board->occupancy |= board->bitboards[i];
	}

	board->friendlyPieces = 0;
	for(int i = board->color; i<board->color+6; i++){
		board->friendlyPieces |= board->bitboards[i];
	}
	board->enemyPieces = board->occupancy & (~board->friendlyPieces);
}
