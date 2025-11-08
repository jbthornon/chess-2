#pragma once
#include "board.h"
#include "move.h"
#include "movegen.h"

void perft(char* fen, int depth, int expected, bool divided);
