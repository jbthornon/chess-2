#include <string.h>
#include <ctype.h>

#include "board.h"

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
