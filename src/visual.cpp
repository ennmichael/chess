#include "visual.hpp"
#include "utils.hpp"
#include "ncurses.h"

namespace Chess::Visual {

Scoped_curses::Scoped_curses()
{
  initscr();
  raw();
  noecho();
  keypad(stdscr, true);
  curs_set(0);
}

Scoped_curses::~Scoped_curses()
{
  endwin();
}

Board_view::Board_view(Game::Board const& board)
  : board_(board)
{}

void Board_view::display()
{
  draw_tiles();
  // TODO draw all pieces
}

void Board_view::draw_tiles()
{
  for (auto x = 0; x < Game::Board::width; ++x)
    for (auto y = 0; y < Game::Board::height; ++y)
      draw_tile(field_position(utils::Position{x, y}));
}

void Board_view::draw_tile(utils::Position const upper_left_pos)
{
  auto const draw_corners = [&] {
    mvaddch(upper_left_pos.y,   upper_left_pos.x,   ACS_ULCORNER);
    mvaddch(upper_left_pos.y+2, upper_left_pos.x,   ACS_LLCORNER);
    mvaddch(upper_left_pos.y,   upper_left_pos.x+4, ACS_URCORNER);
    mvaddch(upper_left_pos.y+2, upper_left_pos.x+4, ACS_LRCORNER);  
  };

  auto const draw_frame = [&] {
    mvaddch(upper_left_pos.y+1, upper_left_pos.x,   ACS_VLINE);
    mvaddch(upper_left_pos.y+1, upper_left_pos.x+4, ACS_VLINE);

    mvaddch(upper_left_pos.y,   upper_left_pos.x+2, ACS_HLINE);
    mvaddch(upper_left_pos.y,   upper_left_pos.x+3, ACS_HLINE);
    mvaddch(upper_left_pos.y,   upper_left_pos.x+1, ACS_HLINE);
    mvaddch(upper_left_pos.y+2, upper_left_pos.x+2, ACS_HLINE);
    mvaddch(upper_left_pos.y+2, upper_left_pos.x+3, ACS_HLINE);
    mvaddch(upper_left_pos.y+2, upper_left_pos.x+1, ACS_HLINE);
  };
  
  draw_corners();
  draw_frame();
}

void Board_view::draw_piece(Game::Piece const piece)
{
  auto const upper_left_pos = field_position(piece.position);
  mvaddch(
    upper_left_pos.y+1, 
    upper_left_pos.x+2, 
    piece.character_representation()
  );
}

utils::Position Board_view::field_position(utils::Position const board_pos)
{
  return utils::Position{
    board_pos.x * field_width, 
    board_pos.y * field_height
  };
}

}

