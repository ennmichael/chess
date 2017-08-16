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

void Handler::on_key(Key const key, Callback const& callback)
{
  callbacks_[key].push_back(callback);
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

auto Handler::callbacks_for_key(Key key) const -> Callbacks
{
  return callbacks_.count(key) ? callbacks_.at(key) : Callbacks();
}

void Handler::dispatch_key(Key key)
{
  for (auto const& callback : callbacks_for_key(key))
    callback();
}

void Handler::dispatch_input()
{
  auto const key = getch();
  dispatch_key(key);
}

}

}

