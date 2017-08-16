#pragma once

#include "game.hpp"

namespace Chess::Visual {

class Scoped_curses {
public:
  Scoped_curses();
  ~Scoped_curses();

  Scoped_curses(Scoped_curses const&) =delete;
  Scoped_curses(Scoped_curses&&) =delete;
  Scoped_curses& operator=(Scoped_curses const&) =delete;
  Scoped_curses& operator=(Scoped_curses&&) =delete;
};

class Board_view {
public:
  explicit Board_view(Game::Board const&);

  void display();

private:
  static constexpr auto field_width = 5;
  static constexpr auto field_height = 3;

  static void draw_tiles();
  static void draw_tile(utils::Position const upper_left_pos);

  static void draw_piece(Game::Piece const piece);
  static void draw_field(utils::Position const upper_left_pos, char const);
  static utils::Position field_position(utils::Position const board_pos);

  Game::Board const& board_;
};

}

