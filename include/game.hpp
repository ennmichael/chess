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

struct Board;

struct Piece {
  Piece_kind kind;
  Piece_position position;
  Piece_color color;

  void move(Board_position const);
  bool was_moved() const;

  Board_positions possible_jump_positions(Board const&) const; 
};

struct Board {
  static constexpr auto width = 8;
  static constexpr auto height = 8;

  static Board default_ordered();

  using Pieces = std::vector<Piece>;
  using Field_iteration_callback = 
    std::function<void(boost::optional<Piece>, Board_position)>;

  void iterate_fields(Field_iteration_callback const) const;
  boost::optional<Piece> piece_at_position(Board_position const) const;

  Pieces pieces;
};

using Key = chtype;

class Keyboard_signals {
private:
  using Callback_signature = void();

public:
  using Callback = std::function<Callback_signature>;

  void connect(Key const, Callback const&);
  void dispatch_key(Key const) const;
  void dispatch_input() const;

private:
  using Signal = boost::signals2::signal<Callback_signature>;

  std::unordered_map<Key, Signal> signals_;
};

using Source_piece = Board::Pieces::iterator;
using Target_position = Board_position;

class Player_move {
public:
  void pull(Board&);
  bool ready() const;

private:
  boost::optional<Source_piece> source_piece_;
  boost::optional<Target_position> target_position_;
};

struct Signals {
  Keyboard_signals keyboard;
  boost::signals2::signal<void(Board_position)> position_selected;
  boost::signals2::signal<void(Player_move)> player_decided;
  boost::signals2::signal<void()> frame_advance;
  // TODO The frame_advance signal is technically redundant
  // because redrawing can be done only when needed anyways.
  // So it should be replaced
};

class Field_selection {
public:
  static Field_selection centered();

  Board_position position() const;
  int x() const;
  int y() const;

  void connect_to_signals(Signals&);

private:
  explicit Field_selection(Board_position);

  void move_left();
  void move_right();
  void move_up();
  void move_down();

  Board_position position_;
};

class Input_loop {
public:
  explicit Input_loop(Signals const&);

  void start();
  void quit();

private:
  Signals const* signals_;
  bool running_ = false;
};

}

