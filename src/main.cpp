#include "visual.hpp"
#include "game.hpp"
#include "ncurses.h" // TODO Do not

// TODO We can test the input handler, so we should

int main()
{
  Chess::Visual::Scoped_curses scoped_curses;

  auto board = Chess::Game::Board::default_ordered();
  Chess::Visual::Board_view view(board);
  view.display();

  Chess::Game::Input::Handler input_handler;

  input_handler.on_key('q', [&] {
    input_handler.quit_input_loop();
  });

  input_handler.start_input_loop();
}
