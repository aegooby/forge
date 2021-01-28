
#include "sdl.hpp"

#include <cassert>
#include <string>
#include <vector>

namespace forge
{
namespace sdl
{
error::error(const char* what_arg) noexcept
    : __base(std::string(what_arg) + " (" + SDL_GetError() + ")")
{ }
error::error(const std::string& what_arg) noexcept
    : __base(what_arg + " (" + SDL_GetError() + ")")
{ }
error::~error() = default;

void start()
{
    if (SDL_Init(SDL_INIT_VIDEO)) throw error("Failed to initialize SDL");
}
void stop()
{
    SDL_Quit();
}

window* create_window(const std::string& title, std::size_t width,
                      std::size_t height, bool fullscreen, bool resizable)
{
    auto          pos   = SDL_WINDOWPOS_CENTERED;
    std::uint32_t flags = (SDL_WINDOW_VULKAN | SDL_WINDOW_SHOWN);
    if (fullscreen) flags |= SDL_WINDOW_FULLSCREEN;
    if (resizable) flags |= SDL_WINDOW_RESIZABLE;
    auto __title = title.c_str();
    auto handle  = SDL_CreateWindow(__title, pos, pos, width, height, flags);
    return handle ?: throw error("Failed to create window");
}
void destroy_window(window* handle)
{
    SDL_DestroyWindow(handle);
}

bool poll_event(event& event)
{
    return static_cast<bool>(SDL_PollEvent(&event));
}

vector_2 mouse_position()
{
    int x = 0, y = 0;
    SDL_GetMouseState(&x, &y);
    return vector_2(static_cast<float>(x), static_cast<float>(y));
}
vector_2 mouse_movement()
{
    int x = 0, y = 0;
    SDL_GetRelativeMouseState(&x, &y);
    return vector_2(static_cast<float>(x), static_cast<float>(y));
}

keymod modifier_state()
{
    return SDL_GetModState();
}

namespace vulkan
{
std::vector<const char*> extensions(window* handle)
{
    unsigned int count;
    if (!SDL_Vulkan_GetInstanceExtensions(handle, &count, nullptr))
        throw std::runtime_error("Failed to get Vulkan extension count");
    auto result = std::vector<const char*>(count);
    if (!SDL_Vulkan_GetInstanceExtensions(handle, &count, result.data()))
        throw std::runtime_error("Failed to get Vulkan extensions");
    return result;
}
surface create_surface(window* handle, instance instance)
{
    assert(instance);
    VkSurfaceKHR result = nullptr;
    if (!SDL_Vulkan_CreateSurface(handle, instance, &result))
        throw std::runtime_error("Failed to create Vulkan surface");
    return result;
}
std::pair<std::uint32_t, std::uint32_t> extent(window* handle)
{
    int width = 0, height = 0;
    SDL_Vulkan_GetDrawableSize(handle, &width, &height);
    return std::make_pair(std::uint32_t(width), std::uint32_t(height));
}
} // namespace vulkan

} // namespace sdl
} // namespace forge