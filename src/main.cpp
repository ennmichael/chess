#include "sdl++.h"

using namespace std::string_literals;

namespace {

unsigned constexpr window_width = 500;
unsigned constexpr window_height = 500;

// Maybe this is Redraw instead of FrameAdvance?
template <class FrameAdvance, class DispatchKey>
void main_loop(FrameAdvance const& frame_advance, DispatchKey const& dispatch_key)
{
        auto quit = false;

        while (!quit) {
                while (std::optional<Sdl::Event> const event = Sdl::poll_event()) {
                        switch (event->type) {
                                case Sdl::Events::quit:
                                        quit = true;
                                        break;
                                case Sdl::Events::key_down:
                                        dispatch_key(event->keysym->sym);
                                        break;
                        }
                }
        }
}

}

int main(int, char**)
{
        Sdl::InitGuard _;

        Sdl::UniqueWindow window = Sdl::create_window("chess"s,
                                                     window_width,
                                                     window_height);
        Sdl::UniqueRenderer renderer = Sdl::create_renderer(*window);

        main_loop([] {});
}
