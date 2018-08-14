#include "graphics.h"
#include <cassert>

namespace Chess {

namespace {

Sdl::Rect field_src_rect(Piece piece)
{
        auto const y_mult = [&]
        {
                switch (piece.side) {
                        case Side::dark: return 0;
                        case Side::light: return 1;
                        default:
                                assert(false);
                                return 0;
                }
        };

        auto const x_mult = [&]
        {
                switch (piece.kind) {
                        case Piece::Kind::queen: return 0;
                        case Piece::Kind::king: return 1;
                        case Piece::Kind::rook: return 2;
                        case Piece::Kind::knight: return 3;
                        case Piece::Kind::bishop: return 4;
                        case Piece::Kind::pawn: return 5;
                        default:
                                assert(false);
                                return 0;
                }
        };

        return Sdl::Rect {
                .x = field_width * x_mult(),
                .y = field_height * y_mult(),
                .w = field_width,
                .h = field_height
        };
}

Sdl::Rect field_dst_rect(Position pos)
{
        return Sdl::Rect {
                .x = pos.x * field_width,
                .y = pos.y * field_height,
                .w = field_width,
                .h = field_height
        };
}

void draw_field(Sdl::Renderer& renderer, Position pos, Sdl::Color color)
{
        Sdl::render_rect(renderer, field_dst_rect(pos), color);
}

void draw_background(Sdl::Renderer& renderer, Position pos)
{
        Sdl::Color constexpr light {.r = 0xD1, .g = 0x8B, .b = 0x47, .a = 0xFF};
        Sdl::Color constexpr dark {.r = 0xFF, .g = 0xCE, .b = 0x9E, .a = 0xFF};
        auto const color = ((pos.x + pos.y) % 2 == 0) ? light : dark;
        draw_field(renderer, pos, color);
}

void draw_piece(Sdl::Renderer& renderer,
                Sdl::Texture& pieces,
                Piece piece,
                Position pos)
{
        if (piece != Piece::none()) {
                Sdl::Rect const src = field_src_rect(piece);
                Sdl::Rect const dst = field_dst_rect(pos);
                Sdl::render_copy(renderer, pieces, src, dst);
        }
}

}

void draw_piece_selector(Sdl::Renderer& renderer, PieceSelector selector, Side on_turn)
{
        if (on_turn == Side::none)
                return;

        int constexpr alpha = 0x69;
        Sdl::Color constexpr transparent_white {
                .r = 0xFF, .g = 0xFF, .b = 0xFF, .a = alpha
        };
        Sdl::Color constexpr transparent_black {
                .r = 0x00, .g = 0x00, .b = 0x00, .a = alpha
        };
        Sdl::Color constexpr transparent_red {
                .r = 0xFF, .g = 0x00, .b = 0x00, .a = alpha
        };

        auto const draw_current_position = [&]
        {
                auto const color = (on_turn == Side::light) ?
                        transparent_white : transparent_black;
                draw_field(renderer, selector.current_position(), color);
        };

        auto const draw_selected_position = [&]
        {
                if (std::optional pos = selector.selected_position())
                        draw_field(renderer, *pos, transparent_red);
        };

        draw_current_position();
        draw_selected_position();
}

void draw_board(Sdl::Renderer& renderer, Sdl::Texture& pieces, Board const& board)
{
        for (int x = 0; x < board_size; ++x) {
                for (int y = 0; y < board_size; ++y) {
                        auto const piece = board[x][y];
                        Position const pos {x, y};
                        draw_background(renderer, pos);
                        draw_piece(renderer, pieces, piece, pos);
                }
        }
}

}

