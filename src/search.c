#include <stdio.h>

#include "search.h"
#include "print.h"

static int perftSearch(Board* board, int depth);

void perft(char* fen, int depth, int expected, bool divided){
	Board board;
	loadFEN(&board, fen);
	int result = 0;

	MoveArray legalMoves;
	legalMoves.length = 0;
	generateMoves(&board, &legalMoves);

	for(int i = 0; i<legalMoves.length; i++){
		Unmove unmove = makeMove(&board, legalMoves.moves[i]);
		int kingSquare = bitScanForward(board.bitboards[P_KING+board.enemyColor]);//colors swapped after makeMove
		if(!squareThreatenedBy(&board, kingSquare, board.color)){ //skip moves that put "us" in check(illegal)
			int nodeCount = perftSearch(&board, depth-1);
			result += nodeCount;
			if(divided){
				printMove(legalMoves.moves[i]);
				printf(": %d\n", nodeCount);
			}
		}
		unmakeMove(&board, unmove);
	
	}

	if(expected != 0) printf("Depth: %d, %d/%d, Error: %d\n", depth, result, expected, result-expected);
	if(expected == 0) printf("Depth: %d, %d\n", depth, result);
}

static int perftSearch(Board* board, int depth){
	if(depth == 0) return 1;

	MoveArray legalMoves;
	legalMoves.length = 0;
	generateMoves(board, &legalMoves);

	int nodeCount = 0;
	for(int i = 0; i<legalMoves.length; i++){
		Unmove unmove = makeMove(board, legalMoves.moves[i]);
		int kingSquare = bitScanForward(board->bitboards[P_KING+board->enemyColor]);//colors swapped after makeMove
		if(!squareThreatenedBy(board, kingSquare, board->color)) //skip moves that put "us" in check(illegal)
			nodeCount += perftSearch(board, depth-1);
		unmakeMove(board, unmove);
	}
	return nodeCount;
}
