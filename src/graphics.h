#pragma once

#include "ui.h"
#include "sdl++.h"
#include "chess.h"
#include <unordered_map>

namespace Chess {

int constexpr field_width = 60;
int constexpr field_height = 60;

void draw_piece_selector(Sdl::Renderer& renderer, PieceSelector selector, Side on_turn);
void draw_board(Sdl::Renderer& renderer, Sdl::Texture& pieces, Board const& board);

}

