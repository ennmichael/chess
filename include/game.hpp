#pragma once

#include "boost/variant.hpp"
#include "boost/optional.hpp"
#include "boost/signals2/signal.hpp"
#include "utils.hpp"
#include "ncurses.h"
#include <vector>
#include <functional>
#include <unordered_map>

namespace Chess::Game {

using Board_position = utils::Position;

namespace Piece_colors {
  struct Red {};
  struct Blue {};
}

using Piece_color = boost::variant<Piece_colors::Red, 
                                   Piece_colors::Blue>;

namespace Piece_kinds {
  struct Pawn {};
  struct Bishop {}; // TODO More of these
}

using Piece_kind = boost::variant<Piece_kinds::Pawn,
                                  Piece_kinds::Bishop>;

struct Board;

using Board_positions = std::vector<Board_position>;

class Piece_position {
public:
  explicit Piece_position(Board_position const);

  int x() const;
  int y() const;
  Board_position value() const;

  void move(Board_position const);
  bool was_changed() const;

private:
  Board_position value_;
  bool was_changed_ = false;
};

class Piece {
  Piece_kind kind;
  Piece_position position;
  Piece_color color;

  int x() const;
  int y() const;

  void move(Board_position const);
  bool was_moved() const;

  Board_positions possible_jump_positions(Board const&) const; 
};

class Board {
public:
  static Board default_ordered();
  
  static constexpr auto width = 8;

  static constexpr auto height = 8;
  
  void for_each_piece(std::function<void(Piece)> const&) const;

private:
  Board();

  std::vector<Piece> pieces_;
};

using Board = std::vector<Piece>;
using Source_piece = Board::iterator;
using Target_piece = Board::iterator;


struct Player_move {
  Board_position source_position;
  Board_position target_position;

  void pull(Board&) const;
  Source_piece source_piece(Board&) const;
  boost::optional<Target_piece> target_piece(Board&) const;
};

// TODO Compile
// Where do we get the Player_move struct from?
// Write Input and Visual

namespace Input { 

class Field_selection {
public:
  static Field_selection centered();

  Board_position position() const;
  int x() const;
  int y() const;

  void move_left();
  void move_right();
  void move_up();
  void move_down();

private:
  explicit Field_selection(Board_position);

  Board_position position_;
};

using Key = chtype;

class Handler {
private:
  using Callback_signature = void();

public:
  using Callback = std::function<Callback_signature>;
  
  static Handler for_field_selection(Field_selection&);

  void on_key(Key const, Callback const&);
  void start_input_loop();
  void quit_input_loop();

private:
  using Signal = boost::signals2::signal<Callback_signature>;

  void dispatch_key(Key);
  void dispatch_input();

  std::unordered_map<Key, Signal> signals_;
  bool input_loop_running_ = false;
};

}

}

