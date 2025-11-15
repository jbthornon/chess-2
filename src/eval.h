#include <limits.h>
#include "board.h"

#define LOSS_EVAL -INT_MAX //INT_MIN is too big to be inverted
#define WIN_EVAL -LOSS_EVAL

int evaluate(Board* board);//returns the evaluation for the current position
