
#include "window.hpp"

namespace forge
{
window::window(class keyboard& keyboard, class mouse& mouse,
               const std::string& title, bool resizable)
    : keyboard(keyboard), mouse(mouse), title(title), resizable(resizable)
{ }

void window::start(std::size_t width, std::size_t height, bool fullscreen)
{
    this->width      = width;
    this->height     = height;
    this->fullscreen = fullscreen;

    handle = sdl::create_window(title, width, height, fullscreen);
    __open = true;
}
void window::stop()
{
    __open = false;
    sdl::destroy_window(handle);
    handle = nullptr;
}
const bool window::open() const
{
    return __open;
}
void window::update()
{
    keyboard.down_clear();
    keyboard.up_clear();
    keyboard.update();

    mouse.down_clear();
    mouse.up_clear();
    mouse.update();

    sdl::event event;

    while (sdl::poll_event(event))
    {
        keycode   __keycode   = keycode(event.key.keysym.scancode);
        mousecode __mousecode = mousecode(event.button.button);
        switch (event.type)
        {
            case sdl::events::window_close:
            case sdl::events::quit:
                __open = false;
                break;
            case sdl::events::key_down:
                if (!event.key.repeat) keyboard.down_set(true, __keycode);
                keyboard.scan_set(true, __keycode);
                break;
            case sdl::events::key_up:
                keyboard.up_set(true, __keycode);
                keyboard.scan_set(false, __keycode);
                break;
            case sdl::events::mousebutton_down:
                mouse.down_set(true, __mousecode);
                mouse.scan_set(true, __mousecode);
                break;
            case sdl::events::mousebutton_up:
                mouse.up_set(true, __mousecode);
                mouse.scan_set(false, __mousecode);
                break;
        }
    }
}
} // namespace forge