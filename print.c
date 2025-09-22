#include <stdio.h>

#include "print.h"

void printBB(u64 bb){
	printf("  a b c d e f g h\n");//header
	for(int y = 7; y>=0; y--){
		printf("%c", '1'+y);
		for(int x = 0; x<8; x++){
			if(BBGet(bb, x, y))
				printf("\x1b[42;30m");//background color green, fg color black
			else
				printf("\x1b[47;30m");//background color white, fg color black

			printf("  \x1b[0m");//"  " + color reset
		}
		printf("\n");
	}
}

void printBoard(Board *board, u64 highlighted){
	char* pieceChars = "PNBRQKpnbrqk";
	printf("  a b c d e f g h\n");//header
	for(int y = 7; y>=0; y--){
		printf("%c", '1'+y);
		for(int x = 0; x<8; x++){
			if((x+y)%2 == 0)
				printf("\x1b[42;30m");//background color green, fg color black
			else
				printf("\x1b[47;30m");//background color white, fg color black
			if(BBGet(highlighted, x, y))
				printf("\x1b[43;30m");

			int piece = board->squares[x][y];
			if(piece == P_EMPTY){
				printf("  \x1b[0m");
				continue;
			}
			int index = piece&MP_PIECE;
			if(index>=0 && index<6){
				if(piece&MP_BLACK) index+=6;
				printf("%c ", pieceChars[index]);
			}
			else
				printf("!?");

			printf("\x1b[0m");//color reset
		}
		printf("\n");
	}
}

void printBoardDebug(Board *board){
	printBoard(board, 0);
	printf("bitboards\n");
	printf("----white pieces----\n");
	for(int i = 0; i<6; i++)
		printBoard(board, board->whitePieces[i]);

	printf("----black pieces----\n");
	for(int i = 0; i<6; i++)
		printBoard(board, board->blackPieces[i]);
}
