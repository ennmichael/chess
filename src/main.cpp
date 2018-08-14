#include "ui.h"
#include "sdl++.h"
#include "chess.h"
#include "graphics.h"

namespace {

unsigned constexpr window_width = Chess::field_width * Chess::board_size;
unsigned constexpr window_height = Chess::field_height * Chess::board_size;

// Maybe this is Redraw instead of FrameAdvance?
template <class Redraw, class DispatchKey>
void main_loop(Redraw const& redraw, DispatchKey const& dispatch_key)
{
        auto quit = false;
        while (!quit) {
                redraw();
                while (std::optional<Sdl::Event> const event = Sdl::poll_event()) {
                        switch (event->type) {
                                case Sdl::Events::quit:
                                        quit = true;
                                        break;
                                case Sdl::Events::key_down:
                                        dispatch_key(event->key.keysym.sym);
                                        break;
                        }
                }
        }
}

}

int main(int, char**)
{
        using namespace std::string_literals;

        Sdl::InitGuard _;
        Sdl::UniqueWindow window = Sdl::create_window("chess"s,
                                                     window_width,
                                                     window_height);
        Sdl::UniqueRenderer renderer = Sdl::create_renderer(*window);
        auto pieces = Sdl::load_texture(*renderer, "pieces.png"s);
        Chess::PieceSelector selector(Chess::Position {
                .x = 0, .y = Chess::board_size - 1
        });
        Chess::Game game;

        auto const redraw = [&]
        {
                Sdl::render_clear(*renderer);
                Chess::draw_board(*renderer, *pieces, game.board());
                Chess::draw_piece_selector(*renderer, selector, game.on_turn());
                Sdl::render_present(*renderer);
        };

        auto const dispatch_key = [&](Sdl::Keycode keycode) {
                switch (keycode) {
                        case Sdl::Keycodes::left:
                                selector.move_left();
                                break;
                        case Sdl::Keycodes::right:
                                selector.move_right();
                                break;
                        case Sdl::Keycodes::up:
                                selector.move_up();
                                break;
                        case Sdl::Keycodes::down:
                                selector.move_down();
                                break;
                        case Sdl::Keycodes::space:
                                if (std::optional move = selector.select())
                                        game.try_move(*move);
                                break;
                        case Sdl::Keycodes::u:
                                game.undo_move();
                                break;
                        case Sdl::Keycodes::r:
                                game.redo_move();
                                break;
                }
        };

        main_loop(redraw, dispatch_key);
}
