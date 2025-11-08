#include <stdio.h>

#include "print.h"

char pieceToChar(int piece){
	if(piece<-1 || piece>P_KING+6) return '?';
	const char* pieceChars = " PNBRQKpnbrqk";
	return pieceChars[piece+1];
}

void printBB(u64 bb){
	printf("  a b c d e f g h\n");//header
	for(int y = 7; y>=0; y--){
		printf("%c", '1'+y);
		for(int x = 0; x<8; x++){
			if(GET_BIT64(bb, BOARD_INDEX(x, y)))
				printf("\x1b[42;30m");//background color green, fg color black
			else
				printf("\x1b[47;30m");//background color white, fg color black

			printf("  \x1b[0m");//"  " + color reset
		}
		printf("\n");
	}
}

void printBin(u64 x){
	for(int i = 63; i>0; i--){
		if(GET_BIT64(x, i)) printf("1");
		else printf("0");
	}
	printf("\n");
}

void printBoard(Board *board, u64 highlighted){
	printf(" a b c d e f g h\n");
	for(int y = 7; y>=0; y--){
		printf("%c", '1'+y);
		for(int x = 0; x<8; x++){
			if((x+y)%2 == 0)
				printf("\x1b[42;30m");//background color green, fg color black
			else
				printf("\x1b[47;30m");//background color white, fg color black
			
			if(GET_BIT64(highlighted, BOARD_INDEX(x, y)))
				printf("\x1b[43;30m");//background color yellow, fg color black

			int piece = board->squares[BOARD_INDEX(x,y)];
			printf("%c \x1b[0m", pieceToChar(piece));
		}
		printf("%c\n", '1'+y);
	}
	printf(" a b c d e f g h\n");
	if(board->enPassant != -1)
		printf("En Passant at %c%d \n", 'a'+(board->enPassant%8), 1+board->enPassant/8);
}

void printIndices(){
	printf(" a b c d e f g h\n");
	for(int y = 7; y>=0; y--){
		printf("%c", '1'+y);
		for(int x = 0; x<8; x++){
			if((x+y)%2 == 0)
				printf("\x1b[42;30m");//background color green, fg color black
			else
				printf("\x1b[47;30m");//background color white, fg color black
			
			printf("%d", BOARD_INDEX(x,y));
			if(BOARD_INDEX(x,y)<10) printf(" ");

			printf("\x1b[0m");//color reset
		}
		printf("%c\n", '1'+y);
	}
	printf(" a b c d e f g h\n");
}

void printBoardDebug(Board *board){
	printBoard(board, 0);
	printf("bitboards\n");
	printf("----white pieces----\n");
	for(int i = 0; i<6; i++)
		printBoard(board, board->bitboards[i]);

	printf("----black pieces----\n");
	for(int i = 6; i<12; i++)
		printBoard(board, board->bitboards[i]);
}

void printMove(Board* board, Move m){
	if(board->squares[m.from] == P_PAWN && m.promotion != P_EMPTY){
		printf("%c%c%c%c%c", 'a' + (m.from%8), '1' + (m.from/8), 'a' + (m.to%8), '1' + (m.to/8), pieceToChar(m.promotion));
	}
	else{
		printf("%c%c%c%c", 'a' + (m.from%8), '1' + (m.from/8), 'a' + (m.to%8), '1' + (m.to/8));
	}
}
