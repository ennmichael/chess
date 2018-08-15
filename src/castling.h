#pragma once

#include "chess.h"

namespace Chess {

Rule castling_rule();
void castling(Board& board, Move move);
void undo_castling(Board& board, Move move);

}

