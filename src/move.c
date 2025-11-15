#include "move.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static void movePiece(Board* board, int from, int to){
	int piece = board->squares[from];
	int captured = board->squares[to];
	//update square-centric board
	board->squares[to] = piece;
	board->squares[from] = P_EMPTY;

	//update bitboards
	RESET_BIT64(board->bitboards[piece], from);
	SET_BIT64(board->bitboards[piece], to);
	if(captured != P_EMPTY)
		RESET_BIT64(board->bitboards[captured], to);
}

bool moveIsPromo(Move move){
	return (move.type != M_NORMAL && move.type != M_CASTLE);
}

Unmove makeMove(Board* board, Move move){
	int piece = board->squares[move.from];
	int captured = board->squares[move.to];
	int pieceType = piece-(board->color);
	int capturedType = captured-(board->enemyColor);
	Unmove unmove;
	unmove.to = move.to;
	unmove.from = move.from;
	unmove.captured = captured;
	unmove.capturedSquare = move.to;
	unmove.enPassant = board->enPassant;
	unmove.castlingRights = board->castlingRights;
	unmove.type = move.type;
	
	movePiece(board, move.from, move.to);
	
	//Promotion
	if(moveIsPromo(move)){
		int promotion = move.type+board->color;//M_PROMO_QUEEN == P_QUEEN ect
		board->squares[move.to] = promotion;
		RESET_BIT64(board->bitboards[piece], move.to);
		SET_BIT64(board->bitboards[promotion], move.to);
	}
	
	//Castling
	if(move.type == M_CASTLE){
		//move the rook
		if(move.to == 62)//g8
			movePiece(board, 63, 61);//h8->f8

		if(move.to == 58)//c8
			movePiece(board, 56, 59);//a8->d8

		if(move.to == 6)//g1
			movePiece(board, 7, 5);//h1->f1

		if(move.to == 2)//c1
			movePiece(board, 0, 3);//a1->d1

		//because any castling move is a king move, castling rights will be correctly updated later 
	}

	//En Passant
	//correctly handle en passant capture
	if(pieceType == P_PAWN && move.to == board->enPassant){
		int capturedSquare = move.to+8;
		if(move.to>32)
			capturedSquare = move.to-8;
		captured = board->squares[capturedSquare];
		unmove.captured = captured;
		unmove.capturedSquare = capturedSquare;
		board->squares[capturedSquare] = P_EMPTY;
		RESET_BIT64(board->bitboards[captured], capturedSquare);
	}

	//set en passant target square if move was a double pawn push
	board->enPassant = -1;
	if(pieceType == P_PAWN){
		//set en passant if move was a double push
		if(move.to-move.from == 16)
			board->enPassant = move.to-8;
		if(move.to-move.from == -16)
			board->enPassant = move.to+8;
	}

	//Castling Rights
	if(pieceType == P_KING){
		if(board->whitesTurn)
			board->castlingRights &= ~(CR_WHITE_KINGSIDE|CR_WHITE_QUEENSIDE);
		else
			board->castlingRights &= ~(CR_BLACK_KINGSIDE|CR_BLACK_QUEENSIDE);
	}

	if(pieceType == P_ROOK){
		switch(move.from){
			case 7: //h1
				board->castlingRights &= ~CR_WHITE_KINGSIDE;
				break;
			case 0: //a1
				board->castlingRights &= ~CR_WHITE_QUEENSIDE;
				break;
			case 63: //h8
				board->castlingRights &= ~CR_BLACK_KINGSIDE;
				break;
			case 56: //a8
				board->castlingRights &= ~CR_BLACK_QUEENSIDE;
				break;
		}
	}

	if(capturedType == P_ROOK){
		switch(move.to){
			case 7: //h1
				board->castlingRights &= ~CR_WHITE_KINGSIDE;
				break;
			case 0: //a1
				board->castlingRights &= ~CR_WHITE_QUEENSIDE;
				break;
			case 63: //h8
				board->castlingRights &= ~CR_BLACK_KINGSIDE;
				break;
			case 56: //a8
				board->castlingRights &= ~CR_BLACK_QUEENSIDE;
				break;
		}
	}
	
	board->whitesTurn = !board->whitesTurn;
	updatePerspectiveVariables(board);
	return unmove;
}


void unmakeMove(Board* board, Unmove unmove){
	//restore board state
	board->enPassant = unmove.enPassant;
	board->castlingRights = unmove.castlingRights;
	board->whitesTurn = !board->whitesTurn;
	updatePerspectiveVariables(board);

	//move pieces back
	movePiece(board, unmove.to, unmove.from);

	//undo capture/en passant
	if(unmove.captured != P_EMPTY){
		board->squares[unmove.capturedSquare] = unmove.captured;
		SET_BIT64(board->bitboards[unmove.captured], unmove.capturedSquare);
	}
	
	if(unmove.type == M_CASTLE){
		//move rook back
		if(unmove.to == 62)//g8
			movePiece(board, 61, 63);//f8->h8

		if(unmove.to == 58)//c8
			movePiece(board, 59, 56);//d8->a8

		if(unmove.to == 6)//g1
			movePiece(board, 5, 7);//f1->h1

		if(unmove.to == 2)//c1
			movePiece(board, 3, 0);//d1->a1	
	}

	//promotion
	if(unmove.type != M_NORMAL && unmove.type != M_CASTLE){
		int piece = board->squares[unmove.from];
		board->squares[unmove.from] = P_PAWN+board->color;
		RESET_BIT64(board->bitboards[piece], unmove.from);
		SET_BIT64(board->bitboards[P_PAWN+board->color], unmove.from);
	}
}
