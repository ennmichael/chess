#pragma once

#include "game.hpp"
#include <exception>

namespace Chess::Visual {

struct Exception : std::exception {};

struct Colors_not_supported : Exception {
  char const* what() const noexcept override;
};

class Scoped_curses {
public:
  Scoped_curses();
  ~Scoped_curses();

  Scoped_curses(Scoped_curses const&) =delete;
  Scoped_curses(Scoped_curses&&) =delete;
  Scoped_curses& operator=(Scoped_curses const&) =delete;
  Scoped_curses& operator=(Scoped_curses&&) =delete;

private:
  static void initialize_color_pairs();
};

constexpr auto color_count = 4;

namespace Color_ids {
  constexpr auto red_white = 0;
  constexpr auto red_black = 1;
  constexpr auto blue_white = 3;
  constexpr auto blue_black = 4;
}

using Screen_position = utils::Position;

class Board_view { // TODO Rename into Game_view
public:
  explicit Board_view(Game::Board const&, Game::Input::Field_selection const&);

  void display();

private:
  static constexpr auto field_width = 4;
  static constexpr auto field_height = 2;
  
  void draw_pieces();
  void draw_piece(Game::Piece const);

  static void draw_tiles();
  static void draw_tile(Screen_position const upper_left_pos);
  static void draw_field(Screen_position const upper_left_pos, char const);
  static Screen_position upper_left_screen_pos(Game::Board_position const);

  Game::Board const& board_;
  Game::Input::Field_selection const& field_selection_;
};

}

