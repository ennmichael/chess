#pragma once

#include "boost/variant.hpp"
#include "boost/signals2/signal.hpp"
#include "utils.hpp"
#include <vector>
#include <functional>
#include <unordered_map>

namespace Chess::Game {

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

using Positions = std::vector<utils::Position>;

struct Piece {
  Piece_kind kind;
  utils::Position position;
  Piece_color color;
  
  int x() const;
  int y() const;

  Positions possible_jump_positions(Board const&) const;
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

using Key = char;

class Handler {
public:
  using Callback = std::function<void()>;
  using Callbacks = std::vector<Callback>;

  void on_key(Key const, Callback const&);
  void start_input_loop();
  void quit_input_loop();

private:
  Callbacks callbacks_for_key(Key) const;
  void dispatch_key(Key);
  void dispatch_input();

  std::unordered_map<Key, std::vector<Callback>> callbacks_;
  bool input_loop_running_ = false;
};

}

}

