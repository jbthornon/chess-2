#include "move.h"

void makeMove(Board* board, Move move){
	int piece = board->squares[move.from];
	int captured = board->squares[move.to];
	
	board->squares[move.to] = piece;
	board->squares[move.from] = P_EMPTY;

	BBReset(board->bitboards[piece], move.from);
	
	if(captured != P_EMPTY)
		BBReset(board->bitboards[captured], move.to);
	BBSet(board->bitboards[piece], move.to);
	
	board->turn = !board->turn;
}
