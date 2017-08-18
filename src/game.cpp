#include "game.hpp"
#include "utils.hpp"
#include "ncurses.h"
#include <algorithm>
#include <utility>

namespace Chess::Game {

int Piece::x() const
{
  return position.x;
}

int Piece::y() const
{
  return position.y;
}

char Piece::character_representation() const
{
  return utils::visit_variant<char>(kind,
    [](Piece_kinds::Pawn const)
    {
      return 'P';
    },
    [](Piece_kinds::Bishop const)
    {
      return 'B';
    }
  );
}

Board Board::default_ordered()
{
  return Board({}); // TODO
}

void Board::for_each_piece(std::function<void(Piece)> const& fun) const 
{
  std::for_each(pieces_.cbegin(), pieces_.cend(), fun);
}

Board::Board(std::vector<Piece> pieces)
  : pieces_(std::move(pieces))
{}

namespace Input {

Field_selection Field_selection::centered()
{
  return Field_selection(
    Board_position{Board::width/2, Board::height/2}
  );
}

Board_position Field_selection::position() const
{
  return position_;
}

int Field_selection::x() const
{
  return position().x();
}

int Field_selection::y() const
{
  return position().y();
}

void Field_selection::move_left()
{
  if (position_.x > 0)
    --position_.x;
}

void Field_selection::move_right()
{
  if (position_.x < Board::width-1)
    ++position_.x;
}

void Field_selection::move_up()
{
  if (position_.y > 0)
    --position_.y;
}

void Field_selection::move_down()
{
  if (position_.y < Board::height-1)
    ++position.y;
}

Field_selection::Field_selection(Board_position position)
  : position_(position)
{}

Handler Handler::for_field_selection(Field_selection& selection)
{
  Handler handler;
  handler.on_key();
  return handler;
}

void Handler::on_key(Key const key, Callback const& callback)
{
  signals_[key].connect(callback);
}

void Handler::start_input_loop()
{
  input_loop_running_ = true;
  while (input_loop_running_)
    dispatch_input();
}

void Handler::quit_input_loop()
{
  input_loop_running_ = false;
}

void Handler::dispatch_key(Key key)
{
  signals_[key]();
}

void Handler::dispatch_input()
{
  auto const key = getch();
  dispatch_key(key);
}

}

}

