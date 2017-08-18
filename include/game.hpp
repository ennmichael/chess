#pragma once

#include "boost/variant.hpp"
#include "boost/signals2/signal.hpp"
#include "utils.hpp"
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

struct Piece {
  Piece_kind kind;
  Board_position position;
  Piece_color color;
  
  int x() const;
  int y() const;

  Board_positions possible_jump_positions(Board const&) const;
  char character_representation() const;
};

class Board {
public:
  static Board default_ordered();
  
  static constexpr auto width = 8;

  static constexpr auto height = 8;

  void for_each_piece(std::function<void(Piece)> const&) const;

private:
  explicit Board(std::vector<Piece>);

  std::vector<Piece> pieces_;
};

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

using Key = int;

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

