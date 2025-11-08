#include <stdio.h>

#include "search.h"
#include "print.h"

static int perftSearch(Board board, Move move, int depth);

void perft(char* fen, int depth, int expected, bool divided){
	Board board;
	loadFEN(&board, fen);
	int result = 0;

	int enemyKing = P_KING+board.enemyColor;
	if(squareThreatenedBy(&board, bitScanForward(board.bitboards[enemyKing]), board.color)) {
		printf("cannot run perft, in check\n");
		return;
	}
	
	MoveArray legalMoves;
	legalMoves.length = 0;
	generateMoves(&board, &legalMoves);
	for(int i = 0; i<legalMoves.length; i++){
		int nodeCount = perftSearch(board, legalMoves.moves[i], depth-1);
		result+=nodeCount;
		if(divided){
			printMove(&board, legalMoves.moves[i]);
			printf(": %d\n", nodeCount);
		}
	}

	if(expected != 0) printf("Depth: %d, %d/%d, Error: %d\n", depth, result, expected, result-expected);
	if(expected == 0) printf("Depth: %d, %d\n", depth, result);
}

static int perftSearch(Board board, Move move, int depth){
	makeMove(&board, move);
	
	int enemyKing = P_KING+board.enemyColor;
	if(board.bitboards[enemyKing] == 0) printf("AAA\n");
	if(board.bitboards[P_KING+board.color] == 0) printf("AAA\n");
	if(squareThreatenedBy(&board, bitScanForward(board.bitboards[enemyKing]), board.color)) {
		return 0;
	}
	
	if(depth == 0) return 1;
	int nodeCount = 0;
	MoveArray legalMoves;
	legalMoves.length = 0;
	generateMoves(&board, &legalMoves);
	for(int i = 0; i<legalMoves.length; i++){
		nodeCount += perftSearch(board, legalMoves.moves[i], depth-1);
	}
	return nodeCount;
}
