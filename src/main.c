#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "move.h"
#include "movegen.h"
#include "search.h"
#include "print.h"
#include "eval.h"

#include "magic.h"

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
	Unmove unmove;
	while(!quit){
		legalMoves.length = 0;//clear array
		generateMoves(&board, &legalMoves);
		
		//print board
		if(board.whitesTurn) printf(" ---white goes---\n");//"whites turn" and "white to go" are not centered
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
					" perft - perft from current position\n"
					" test - perft from preset positions\n"
					" eval - eval of current position\n"
					" debug - prints bitboards individualy\n"
					" magic - create a magic number for every square\n"
					" refine - improve current magic numbers\n"
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
			char fen[MAX_FEN_SIZE];
			makeFen(&board, fen);
			perft(fen, depth, 0, true);
			continue;
		}

		if(strcmp(&input[start], "test") == 0){
			//perft results from https://www.chessprogramming.org/Perft_Results
			char positions[6][MAX_FEN_SIZE] = {
				STARTPOS_FEN,//startpos
				"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -",//p2
				"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",//p3
				"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",//p4
				"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",//p5
				"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"//p6
			};
			int results[18] = {
				197281, 4865609, 119060324,//startpos
				97862,  4085603, 193690690,//p2
				674624, 11030083,178633661,//p3
				9467,   422333,  15833292,//p4
				62379,  2103487, 89941194,//p5
				89890,  3894594, 164075551//p6
			};
			int depths[18] = {
				4,5,6,//startpos
				3,4,5,//p2
				5,6,7,//p3
				3,4,5,//p4
				3,4,5,//p5
				3,4,5,//p6
			};
			float time = 0;
			for(int i = 0; i<18; i++){
				printf("%d) ", i);
				time += perft(positions[i/3], depths[i], results[i], false);
			}
			printf("Total time: %fs", time);
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

		if(strcmp(&input[start], "eval") == 0){
			printf(" %d\n", evaluate(&board));
			continue;
		}

		if(strcmp(&input[start], "undo") == 0){
			unmakeMove(&board, unmove);
			continue;
		}

		if(strcmp(&input[start], "debug") == 0){
			printBoardDebug(&board);
			continue;
		}

		if(strcmp(&input[start], "refine") == 0){
			getInput(" iterations:", input, 100);
			int iter = atoi(input);
			refineMagics(iter);
			continue;
		}

		if(strcmp(&input[start], "search") == 0){
			getInput(" depth:", input, 100);
			int depth = atoi(input);
			printMove(search(board, depth));
			continue;
		}

		if(isMove(&input[start])){
			Move move;
			move.from = squareToIndex(&input[start]);
			move.to = squareToIndex(&input[start+2]);
			move.type = M_NORMAL;
			switch(tolower(input[start+4])){
				case 'n':
					move.type = M_PROMO_KNIGHT;
					break;
				case 'r':
					move.type = M_PROMO_ROOK;
					break;
				case 'b':
					move.type = M_PROMO_BISHOP;
					break;
				case 'q':
					move.type = M_PROMO_QUEEN;
					break;
			}
			if(board.squares[move.from] == P_KING){
				if(move.to-move.from == -2 || move.to-move.from == 2){
					move.type = M_CASTLE;
				}
			}
			printMove(move);
			printf("\n");
			unmove = makeMove(&board, move);
			SET_BIT64(highlighted, move.to);
			SET_BIT64(highlighted, move.from);
			continue;
		}
		
		if(isSquare(&input[start])){
			int from = squareToIndex(&input[start]);
			for(int i = 0; i<legalMoves.length; i++){
				if(legalMoves.moves[i].from == from){
					SET_BIT64(highlighted, legalMoves.moves[i].to);
					printMove(legalMoves.moves[i]);
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
