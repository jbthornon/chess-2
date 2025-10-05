#include "move.h"

#include <math.h>

void makeMove(Board* board, Move move){
	int piece = board->squares[move.from];
	int captured = board->squares[move.to];
	
	//update square-centric board
	board->squares[move.to] = piece;
	board->squares[move.from] = P_EMPTY;

	//update bitboards
	BBReset(board->bitboards[piece], move.from);
	BBSet(board->bitboards[piece], move.to);
	
	if(captured != P_EMPTY)
		BBReset(board->bitboards[captured], move.to);
	
	//en passant
	board->color = board->turn? 6:0;
	//correctly handle en passant capture
	if(piece-board->color == P_PAWN && move.to == board->enPassant){
		int capturedSquare = move.to+8;
		if(move.to>32)
			capturedSquare = move.to-8;
		captured = board->squares[capturedSquare];
		board->squares[capturedSquare] = P_EMPTY;
		BBReset(board->bitboards[captured], capturedSquare);
	}
	//set en passant target square if move was a double pawn push
	board->enPassant = -1;
	if(piece-board->color == P_PAWN){
		//set en passant if move was a double push
		if(move.to-move.from == 16)
			board->enPassant = move.to-8;
		if(move.to-move.from == -16)
			board->enPassant = move.to+8;
	}
	board->turn = !board->turn;
}
