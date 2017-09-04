#include "game.hpp"
#include "utils.hpp"
#include <algorithm>
#include <utility>

namespace Chess::Game {

Piece_position::Piece_position(Board_position const pos)
  : value_(pos)
{}

int Piece_position::x() const
{
  return value_.x;
}

int Piece_position::y() const
{
  return value_.y;
}

Board_position Piece_position::value() const
{
  return value_;
}

void Piece_position::move(Board_position const new_pos)
{
  was_changed_ = true;
  value_ = new_pos;
}

bool Piece_position::was_changed() const
{
  return was_changed_;
}

int Piece::x() const
{
  return position.x();
}

int Piece::y() const
{
  return position.y();
}

void Piece::move(Board_position const new_pos)
{
  position.move(new_pos);
}

bool Piece::was_moved() const
{
  return position.was_changed();
}

Board_positions Piece::possible_jump_positions(Board const& board) const
{
  return utils::visit_variant<Board_positions>(
    kind,
    [&](Piece_kinds::Pawn const)
    {
      return utils::visit_variant<Board_positions>(color,
        [&](Piece_colors::Red const) -> Board_positions
        {
          return {position.value() + Board_position{0, 1}};
        },
        [&](Piece_colors::Blue const) -> Board_positions
        {
          return {position.value() + Board_position{0, -1}};
        }
      );
    },
    [&](Piece_kinds::Bishop const) -> Board_positions
    {
      return {};
    }
  );
}

Board default_ordered_board()
{
  return {}; // TODO
}

void Keyboard_signals::connect(Key const key, Callback const& callback)
{
  signals_[key].connect(callback);
}

void Keyboard_signals::dispatch_key(Key const key) const
{
  if (signals_.count(key))
    signals_.at(key)();
}

void Keyboard_signals::dispatch_input() const
{
  auto const key = getch();
  dispatch_key(key);
}

void Player_move::pull(Board&)
{
  // TODO actually move
  source_piece_ = boost::none;
  target_position_ = boost::none;
}

bool Player_move::ready() const
{
  return source_piece_ && target_position_;
}

Field_selection Field_selection::centered()
{
  return Field_selection(
    Board_position{board_width/2, board_height/2}
  );
}

Board_position Field_selection::position() const
{
  return position_;
}

int Field_selection::x() const
{
  return position_.x;
}

int Field_selection::y() const
{
  return position_.y;
}

void Field_selection::connect_to_signals(Signals& signals)
{
  signals.keyboard.connect(KEY_LEFT,  [this] { move_left(); });
  signals.keyboard.connect(KEY_RIGHT, [this] { move_right(); });
  signals.keyboard.connect(KEY_UP,    [this] { move_up(); });
  signals.keyboard.connect(KEY_DOWN,  [this] { move_down(); });
  signals.keyboard.connect(KEY_ENTER, 
    [this, &signals] 
    {
      signals.position_selected(position_);
    }
  );
}

void Field_selection::move_left()
{
  if (position_.x > 0)
    --position_.x;
}

void Field_selection::move_right()
{
  if (position_.x < board_width-1)
    ++position_.x;
}

void Field_selection::move_up()
{
  if (position_.y > 0)
    --position_.y;
}

void Field_selection::move_down()
{
  if (position_.y < board_height-1)
    ++position_.y;
}

Field_selection::Field_selection(Board_position position)
  : position_(position)
{}

Input_loop::Input_loop(Signals const& signals)
  : signals_(&signals)
{}

void Input_loop::start()
{
  running_ = true;
  while (running_) {
    signals_->frame_advance();
    signals_->keyboard.dispatch_input();
  }
}

void Input_loop::quit()
{
  running_ = false;
}

}

