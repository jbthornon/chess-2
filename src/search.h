#pragma once
#include "board.h"
#include "move.h"
#include "movegen.h"

Move search(Board board, int depth);

float perft(char* fen, int depth, int expected, bool divided);//returns time of search in seconds
