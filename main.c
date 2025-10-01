#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "board.h"
#include "move.h"
#include "movegen.h"

#include "print.h"

static bool isSquare(char* str){
	if(str[0]<'a' || str[0]>'h') return false;
	if(str[1]<'1' || str[1]>'8') return false;
	return true;
}
static bool isMove(char* str){
	if(isSquare(str) && isSquare(&str[2])) return true;
	return false;
}

void cli(){
	char* fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
	Board board;

	char input[100];
	loadFEN(&board, fen);
	board.turn = 0;
	bool quit = false;
	u64 highlighted = (u64)0;
	MoveArray legalMoves = moveArrayCreate();
	while(!quit){
		moveArrayDestroy(&legalMoves);
		legalMoves = generateMoves(&board);
		if(board.turn == 1) printf("---blacks turn---\n");
		else printf("---whites turn---\n");
		printBoard(&board, highlighted);
		highlighted = (u64)0;
		printf(" :");
		fgets(input, sizeof(input), stdin);

		//remove whitespace characters from input	
		int start = -1;
		for(int i = 0; i<sizeof(input); i++){
			if(!isspace(input[i]) && start == -1)
				start = i;
			if((isspace(input[i]) || input[i] == 0) && start != -1){
				input[i] = 0;
				break;
			}
		}

		if(strcmp(&input[start], "tst") == 0){
			MoveArray ma = generateMoves(&board);
			for(int i = 0; i<ma.length; i++){
				printMove(ma.moves[i]);
			}
			printf("size:%d\n", ma.size);
			printf("len:%d\n", ma.length);
			continue;
		}

		if(strcmp(&input[start], "exit") == 0){
			quit = true;
			continue;
		}

		if(strcmp(&input[start], "help") == 0){
			printf( " ---help---\n"
					" help - show help menu\n"
					" exit - exit program\n"
					" <a-h><1-8><a-h><1-8> - make a move (eg d2d4)\n"
					);
			continue;
		}

		if(isMove(&input[start])){
			Move move;
			move.from = boardIndex(input[start]-'a',input[start+1]-'1');
			move.to = boardIndex(input[start+2]-'a',input[start+3]-'1');
			makeMove(&board, move);
			BBSet(highlighted, move.to);
			BBSet(highlighted, move.from);
			continue;
		}
		
		if(isSquare(&input[start])){
			int from = boardIndex(input[start]-'a',input[start+1]-'1');
			for(int i = 0; i<legalMoves.length; i++){
				if(legalMoves.moves[i].from == from) BBSet(highlighted, legalMoves.moves[i].to);
			}
			continue;
		}

		printf("command unrecognised\n");
	}
	moveArrayDestroy(&legalMoves);
}

int main(int argc, char* argv[]){
	generateMoveTables();
	cli();
	return 0;
}
