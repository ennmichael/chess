#include "visual.hpp"
#include "game.hpp"
#include "boost/signals2.hpp"

#include "ncurses.h"

// TODO We can test the input handler, so we should

int main()
{
  Chess::Visual::Scoped_curses scoped_curses;

  auto board = Chess::Game::Board::default_ordered();
  auto field_selection = Chess::Game::Input::Field_selection::centered();
  Chess::Visual::Board_view view(board, field_selection);
  view.display();

  Chess::Game::Input::Handler input_handler;

  input_handler.on_key('q', [&] {
    input_handler.quit_input_loop();
  });

  input_handler.start_input_loop();
}
