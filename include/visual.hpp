#pragma once

#include "game.hpp"
#include "ncurses.h"
#include "boost/functional/hash.hpp"
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
};

using Color = int;

struct Color_pair {
  Color foreground;
  Color background;
};

bool operator==(Color_pair const, Color_pair const);

}

template <>
struct std::hash<Chess::Visual::Color_pair> {
  std::size_t operator()(Chess::Visual::Color_pair const color_pair) const
  {
    std::size_t seed = 0;
    boost::hash_combine(seed, color_pair.foreground);
    boost::hash_combine(seed, color_pair.background);
    return seed;
  }
};

namespace Chess::Visual {

class Colors {
public:
  Color color_for_pair(Color_pair const);

private:
  using Color_id = int;

  bool is_inited(Color_pair const) const;
  void init_color_pair(Color_pair const);

  std::unordered_map<Color_pair, Color_id> color_ids_;
  Color_id new_color_id_ = 1;
};

using Screen_position = utils::Position;

class Board_view { // TODO Rename into Game_view
public:
  explicit Board_view(Game::Board const&, Game::Input::Field_selection const&);

  void display();

private:
  static constexpr auto field_width = 4;
  static constexpr auto field_height = 2;
  
  void draw_tiles();
  void draw_tile(Screen_position const);
  bool tile_is_selected(Screen_position const) const;
  
  static Screen_position upper_left_screen_pos(Game::Board_position const);
  static Game::Board_position board_position(Screen_position const);
  
  Game::Board const* board_;
  Game::Input::Field_selection const* field_selection_;
  Colors colors_;
};

char piece_character_representation(Game::Piece const);

class Scoped_color {
public:
  Scoped_color(Color const);
  ~Scoped_color();

  Scoped_color(const Scoped_color&) =delete;
  Scoped_color(Scoped_color&&) =delete;
  Scoped_color& operator=(const Scoped_color&) =delete;
  Scoped_color& operator=(Scoped_color&&) =delete;

private:
  Color color_;
};

void mvaddch_colored(int const y, int const x, chtype const, Color const);

}

