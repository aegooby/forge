
#pragma once
#include "../__common.hpp"
#include "../input.hpp"
#include "../sdl.hpp"

#include <string>

namespace forge
{
class window
{
private:
    class keyboard& keyboard;
    class mouse&    mouse;

public:
    std::size_t width  = 0;
    std::size_t height = 0;

    const std::string title = std::string();

private:
    const bool resizable  = false;
    bool       fullscreen = false;
    bool       __open     = false;

public:
    sdl::window* handle = nullptr;

    window(class keyboard& keyboard, class mouse& mouse,
           const std::string& = std::string(), bool = false);

    void start(std::size_t = 1366, std::size_t = 768, bool = false);
    void stop();

    bool open() const;

    void update();

    window(const window&) = delete;
    window(window&&)      = delete;
    window& operator=(const window&) = delete;
    window& operator=(window&&) = delete;
};
} // namespace forge