#pragma once

#include <SDL2/SDL.h>
#include <memory>
#include <utility>
#include <string>
#include <exception>
#include <optional>

namespace Sdl {

using Keycode = SDL_Keycode;

namespace Events {
        auto constexpr quit = SDL_QUIT;
        auto constexpr key_down = SDL_KEYDOWN;
}

// TODO Use scancodes
namespace Keycodes {
        auto constexpr left = SDLK_LEFT;
        auto constexpr right = SDLK_RIGHT;
        auto constexpr up = SDLK_UP;
        auto constexpr space = SDLK_SPACE;
}

using Keyboard = Uint8 const*;
using Window   = SDL_Window;
using Renderer = SDL_Renderer;
using Texture  = SDL_Texture;

struct WindowDeleter {
        void operator()(Window* window) const noexcept;
};

struct RendererDeleter {
        void operator()(Renderer* Renderer) const noexcept;
};

struct TextureDeleter {
        void operator()(Texture* texture) const noexcept;
};

using UniqueWindow   = std::unique_ptr<Window, WindowDeleter>;
using UniqueRenderer = std::unique_ptr<Renderer, RendererDeleter>;
using UniqueTexture  = std::unique_ptr<Texture, TextureDeleter>;
using Color = SDL_Color;
using Rect  = SDL_Rect;
using Event = SDL_Event;
using Ticks = Uint32;

Color constexpr white { .r = 255, .g = 255, .b = 255, .a = 255 };

class Error : public std::exception {
public:
        char const* what() const noexcept override;
};

class InitGuard {
public:
        InitGuard();
        ~InitGuard();
        InitGuard(InitGuard const&) = delete;
        InitGuard(InitGuard&&) = delete;
        InitGuard& operator=(InitGuard const&) = delete;
        InitGuard& operator=(InitGuard&&) = delete;
};

class RendererColorGuard {
public:
        RendererColorGuard(Renderer& Renderer, Color color);
        ~RendererColorGuard();
        RendererColorGuard(RendererColorGuard const&) = delete;
        RendererColorGuard(RendererColorGuard&&) = delete;
        RendererColorGuard& operator=(RendererColorGuard const&) = delete;
        RendererColorGuard& operator=(RendererColorGuard&&) = delete;

private:
        Renderer& renderer_;
        Color previous_color_;
};

struct Dimensions {
        int width;
        int height;
};

UniqueWindow create_window(std::string const& title, int width, int height);
UniqueRenderer create_renderer(Window& window, Color color=white);
void set_render_color(Renderer& Renderer, Color color);
Color get_render_color(Renderer& Renderer);
void render_clear(Renderer& Renderer);
void render_present(Renderer& Renderer);

enum class Flip {
        none = SDL_FLIP_NONE,
        vertical = SDL_FLIP_VERTICAL,
        horizontal = SDL_FLIP_HORIZONTAL
};

void render_rect(Renderer& Renderer, Rect rect, Color color);
void render_copy(Renderer& Renderer,
                 Texture& texture,
                 Rect source,
                 Rect destination,
                 Flip flip=Flip::none,
                 double angle=0.);

UniqueTexture load_texture(Renderer& Renderer, std::string path);
Dimensions texture_dimensions(Texture& texture);
Dimensions renderer_dimensions(Sdl::Renderer& Renderer);

Ticks get_ticks() noexcept;
std::optional<Event> poll_event();

}

