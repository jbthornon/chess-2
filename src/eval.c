#include "eval.h"

static int countMaterial(Board* board);

int evaluate(Board* board){
	return countMaterial(board);
}

static int countMaterial(Board* board){
	int count = 0;
	count += countBits(board->bitboards[P_PAWN+board->color])*100;
	count += countBits(board->bitboards[P_KNIGHT+board->color])*300;
	count += countBits(board->bitboards[P_BISHOP+board->color])*300;
	count += countBits(board->bitboards[P_ROOK+board->color])*500;
	count += countBits(board->bitboards[P_QUEEN+board->color])*900;

	count -= countBits(board->bitboards[P_PAWN+board->enemyColor])*100;
	count -= countBits(board->bitboards[P_KNIGHT+board->enemyColor])*300;
	count -= countBits(board->bitboards[P_BISHOP+board->enemyColor])*300;
	count -= countBits(board->bitboards[P_ROOK+board->enemyColor])*500;
	count -= countBits(board->bitboards[P_QUEEN+board->enemyColor])*900;
	return count;
}
