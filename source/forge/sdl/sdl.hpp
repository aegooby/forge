
#pragma once
#include "../__common.hpp"
#include "../math.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <stdexcept>
#include <utility>

namespace forge
{
namespace sdl
{
using window = SDL_Window;

class error : public std::runtime_error
{
public:
    using __base = std::runtime_error;

public:
    error(const char* what_arg) noexcept;
    error(const std::string& what_arg) noexcept;
    virtual ~error() override;
};

void    start();
void    stop();
window* create_window(const std::string&, std::size_t, std::size_t, bool);
void    destroy_window(window*);

using event = SDL_Event;

bool poll_event(event&);

namespace events
{
static constexpr auto window_close     = SDL_WINDOWEVENT_CLOSE;
static constexpr auto quit             = SDL_QUIT;
static constexpr auto key_down         = SDL_KEYDOWN;
static constexpr auto key_up           = SDL_KEYUP;
static constexpr auto mousebutton_down = SDL_MOUSEBUTTONDOWN;
static constexpr auto mousebutton_up   = SDL_MOUSEBUTTONUP;
} // namespace events

vector_2 mouse_position();
vector_2 mouse_movement();

using keymod = SDL_Keymod;

keymod modifier_state();

namespace vulkan
{
std::vector<const char*> extensions(window*);
using surface = VkSurfaceKHR;
using instance = VkInstance;
surface create_surface(window*, instance);
std::pair<std::uint32_t, std::uint32_t> extent(window*);
}

} // namespace sdl
} // namespace forge