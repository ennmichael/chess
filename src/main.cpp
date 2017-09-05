#include "visual.hpp"
#include "game.hpp"
#include "boost/signals2.hpp"

#include "ncurses.h"

int main()
{
  Chess::Visual::Scoped_curses scoped_curses;

  auto board = Chess::Game::Board::default_ordered();
  auto field_selection = Chess::Game::Field_selection::centered();
  Chess::Visual::Board_view view(board, field_selection);

  Chess::Game::Signals signals;

  field_selection.connect_to_signals(signals);
  // TODO ^ Should this be a custom Signals constructor

  Chess::Game::Input_loop input_loop(signals);
  
  signals.keyboard.connect('q', [&]
  {
    input_loop.quit();
  });

  signals.frame_advance.connect([&]
  {
    view.display();
  });

  input_loop.start();

  return 0;
}

