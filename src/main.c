#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "move.h"
#include "movegen.h"
#include "search.h"
#include "print.h"

#include "magic.h"

static bool isSquare(char* str){
	if(str[0]<'a' || str[0]>'h') return false;
	if(str[1]<'1' || str[1]>'8') return false;
	return true;
}

static int squareToIndex(char* str){
	return BOARD_INDEX(str[0]-'a',str[1]-'1');
}

static bool isMove(char* str){
	if(isSquare(str) && isSquare(&str[2])) return true;
	return false;
}

static void getInput(const char* prompt, char* input, int size){
	printf("%s", prompt);
	fgets(input, size, stdin);
}

void cli(){
	Board board;
	loadFEN(&board, STARTPOS_FEN);
	
	char input[100];
	bool quit = false;
	u64 highlighted = (u64)0;
	MoveArray legalMoves;
	legalMoves.length = 0;
	while(!quit){
		legalMoves.length = 0;//clear array
		generateMoves(&board, &legalMoves);
		
		//print board
		if(board.whitesTurn) printf(" ---white goes---\n");//"blacks turn" and "black to go" are not centered
		else                 printf(" ---black goes---\n");
		printBoard(&board, highlighted);
		highlighted = (u64)0;

		getInput(" :", input, 100);

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

		if(strcmp(&input[start], "exit") == 0){
			quit = true;
			continue;
		}

		if(strcmp(&input[start], "help") == 0){
			printf( " ---help---\n"
					" help - show help menu\n"
					" exit - exit program\n"
					" threat - highlight threatened squares\n"
					" index - print board indices\n"
					" fen - print board fen\n"
					" load - load board from fen\n"
					" <square><square><promotion> - make a move (eg b1c3)\n"
					" <square> - show legal moves from a square (eg b1)\n"
					" press enter to continue\n"
					);
			fgets(input, sizeof(input), stdin);
			continue;
		}

		if(strcmp(&input[start], "magic") == 0){
			magicSearch();
			continue;
		}

		if(strcmp(&input[start], "threat") == 0){
			for(int i = 0; i<64; i++){
				if(squareThreatenedBy(&board, i, board.enemyColor)) SET_BIT64(highlighted, i);
			}
			continue;
		}

		if(strcmp(&input[start], "index") == 0){
			printIndices();
			continue;
		}

		if(strcmp(&input[start], "perft") == 0){
			getInput(" depth:", input, 100);
			int depth = atoi(input);
			perft(STARTPOS_FEN, depth, 0);
			continue;
		}

		if(strcmp(&input[start], "fen") == 0){
			char fen[MAX_FEN_SIZE];
			makeFen(&board, fen);
			printf("%s\n", fen);
			continue;
		}

		if(strcmp(&input[start], "load") == 0){
			char fen[MAX_FEN_SIZE];
			getInput(" fen:", fen, MAX_FEN_SIZE);
			printf("%s\n", fen);
			loadFEN(&board, fen);
			continue;
		}

		if(isMove(&input[start])){
			Move move;
			move.from = squareToIndex(&input[start]);
			move.to = squareToIndex(&input[start+2]);
			switch(tolower(input[start+4])){
				case 'n':
					move.promotion = P_KNIGHT;
					break;
				case 'r':
					move.promotion = P_ROOK;
					break;
				case 'b':
					move.promotion = P_BISHOP;
					break;
				case 'q':
					move.promotion = P_QUEEN;
					break;
				default:
					move.promotion = P_EMPTY;
					break;
			}
			printMove(&board, move);
			printf("\n");
			makeMove(&board, move);
			SET_BIT64(highlighted, move.to);
			SET_BIT64(highlighted, move.from);
			continue;
		}
		
		if(isSquare(&input[start])){
			int from = squareToIndex(&input[start]);
			for(int i = 0; i<legalMoves.length; i++){
				if(legalMoves.moves[i].from == from){
					SET_BIT64(highlighted, legalMoves.moves[i].to);
					printMove(&board, legalMoves.moves[i]);
					printf("\n");
				}
			}
			continue;
		}

		printf("command unrecognised\n");
	}
}

int main(int argc, char* argv[]){
	fillMoveTables();
	cli();
	return 0;
}
