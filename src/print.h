#pragma once
#include "board.h"
#include "move.h"

void printBB(u64 bb);
void printBin(u64 x);
void printBoard(Board *board, u64 highlighted);
void printBoardDebug(Board *board);
void printMove(Move m);
void printIndices();

char pieceToChar(int piece);
