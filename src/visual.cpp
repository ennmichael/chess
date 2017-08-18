#include "visual.hpp"
#include "utils.hpp"
#include "ncurses.h"
#include <unordered_map>

namespace Chess::Visual {

char const* Colors_not_supported::what() const noexcept
{
  return "Required colors are not supported on this terminal.";
}

Scoped_curses::Scoped_curses()
{
  if (!has_colors())
    throw Colors_not_supported();

  initscr();
  start_color();

  if (COLOR_PAIRS < color_count)
    throw Colors_not_supported();

  initialize_color_pairs();
  raw();
  noecho();
  keypad(stdscr, true);
  curs_set(0);
}

Scoped_curses::~Scoped_curses()
{
  endwin();
}

void Scoped_curses::initialize_color_pairs()
{
  init_pair(red_white, COLOR_RED, COLOR_WHITE);
  init_pair(red_black, COLOR_RED, COLOR_BLACK);
  init_pair(blue_white, COLOR_BLUE, COLOR_WHITE);
  init_pair(blue_black, COLOR_BLUE, COLOR_BLACK); 
}

Board_view::Board_view(
  Game::Board const& board, 
  Game::Input::Field_selection const& field_selection)
  : board_(board)
  , field_selection_(field_selection)
{}

void Board_view::display()
{
  draw_tiles();
  draw_pieces();
}

void Board_view::draw_pieces()
{
  board_.for_each_piece([this](auto const piece) {
    draw_piece(piece);
  });
}

void Board_view::draw_piece(Game::Piece const piece)
{
  // TODO Highlight appropriately
  // TODO Maybe the frame should be what actually turns black,
  // TODO that would solve quite a few issues

  auto const [x, y] = upper_left_screen_pos(piece.position);
  mvaddch(y+1, x+2, piece.character_representation());
}

void Board_view::draw_tiles()
{
  for (auto x = 0; x < Game::Board::width; ++x)
    for (auto y = 0; y < Game::Board::height; ++y)
      draw_tile(upper_left_screen_pos(Game::Board_position{x, y}));
}

void Board_view::draw_tile(Screen_position const upper_left_pos)
{
  auto const [x, y] = upper_left_pos;

  auto const draw_corners = [&] {
    mvaddch(y,   x,   ACS_PLUS);
    mvaddch(y+2, x,   ACS_PLUS);
    mvaddch(y,   x+4, ACS_PLUS);
    mvaddch(y+2, x+4, ACS_PLUS);  
  };

  auto const draw_frame = [&] {
    mvaddch(y+1, x,   ACS_VLINE);
    mvaddch(y+1, x+4, ACS_VLINE);

    mvaddch(y,   x+2, ACS_HLINE);
    mvaddch(y,   x+3, ACS_HLINE);
    mvaddch(y,   x+1, ACS_HLINE);
    mvaddch(y+2, x+2, ACS_HLINE);
    mvaddch(y+2, x+3, ACS_HLINE);
    mvaddch(y+2, x+1, ACS_HLINE);
  };
  
  draw_corners();
  draw_frame();
}

Screen_position upper_left_screen_pos(Game::Board_position const board_pos)
{
  return utils::Position{
    board_pos.x * field_width, 
    board_pos.y * field_height
  };
}

}

