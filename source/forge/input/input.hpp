
#pragma once
#include "../__common.hpp"
#include "../sdl.hpp"
#include "codes.hpp"

#include <array>

namespace forge
{
/**
 * @brief These macros were defined in another file for the sake of conserving
 *        space in this file since they are very long.
 * @see input_enum.hpp
 */
KEY_ENUM();
MODIFIER_ENUM();
MOUSE_ENUM();

template<std::size_t size, typename... types>
bool multi_test(const std::array<bool, size>& array, types... args)
{
    bool result = true;
    for (auto& code : { args... })
        result = result && array.at(std::size_t(code));
    return result;
}
template<std::size_t size, typename... types>
void multi_set(std::array<bool, size>& array, bool value, types... args)
{
    for (auto& code : { args... }) array.at(std::size_t(code)) = value;
}

class keyboard
{
protected:
    std::array<bool, std::size_t(keycode::size)>  __scan;
    std::array<bool, std::size_t(keycode::size)>  __down;
    std::array<bool, std::size_t(keycode::size)>  __up;
    std::array<bool, std::size_t(modifier::size)> __modifier;

public:
    keyboard()
    {
        __scan.fill(false);
        __down.fill(false);
        __up.fill(false);
    }
    ~keyboard()
    {
        __scan.fill(false);
        __down.fill(false);
        __up.fill(false);
    }
    template<typename... types>
    bool scan(types... args) const
    {
        return multi_test(__scan, args...);
    }
    template<typename... types>
    bool down(types... args) const
    {
        return multi_test(__down, args...);
    }
    template<typename... types>
    bool up(types... args) const
    {
        return multi_test(__up, args...);
    }
    template<typename... types>
    void scan_set(bool value, types... args)
    {
        multi_set(__scan, value, args...);
    }
    template<typename... types>
    void down_set(bool value, types... args)
    {
        multi_set(__down, value, args...);
    }
    template<typename... types>
    void up_set(bool value, types... args)
    {
        multi_set(__up, value, args...);
    }
    void scan_clear()
    {
        __scan.fill(false);
    }
    void down_clear()
    {
        __down.fill(false);
    }
    void up_clear()
    {
        __up.fill(false);
    }
    template<typename... types>
    bool modifier(types... args) const
    {
        return multi_test(__modifier, args...);
    }
    template<typename... types>
    void modifier_set(bool value, types... args)
    {
        multi_set(__modifier, value, args...);
    }
    void update()
    {
        auto __mod = sdl::modifier_state();
        modifier_set(bool(__mod & KMOD_ALT), modifier::ALT);
        modifier_set(bool(__mod & KMOD_CTRL), modifier::CTRL);
        modifier_set(bool(__mod & KMOD_GUI), modifier::GUI);
        modifier_set(bool(__mod & KMOD_SHIFT), modifier::SHIFT);
        modifier_set(bool(__mod == KMOD_NONE), modifier::NONE);
    }

    /** @brief Checks for CMD-W or CTRL-W to close the window. */
    bool window_close() const
    {
#if defined(FORGE_OS_MACOS)
        return (down(keycode::W) && modifier(modifier::GUI));
#elif defined(FORGE_OS_WINDOWS)
        return (down(keycode::W) && modifier(modifier::CTRL));
#else
        return false;
#endif
    }

    keyboard(const keyboard&) = delete;
    keyboard(keyboard&&)      = delete;
    keyboard& operator=(const keyboard&) = delete;
    keyboard& operator=(keyboard&&) = delete;
};

class mouse
{
protected:
    /** @brief Keycode state storages. */
    std::array<bool, std::size_t(mousecode::size)> __scan;
    std::array<bool, std::size_t(mousecode::size)> __down;
    std::array<bool, std::size_t(mousecode::size)> __up;

public:
    /** @brief Movement and position of mouse on screen. */
    vector_2 position;
    vector_2 movement;

public:
    mouse()
    {
        __scan.fill(false);
        __down.fill(false);
        __up.fill(false);
    }
    ~mouse()
    {
        __scan.fill(false);
        __down.fill(false);
        __up.fill(false);
    }
    template<typename... types>
    bool scan(types... args) const
    {
        return multi_test(__scan, args...);
    }
    template<typename... types>
    bool down(types... args) const
    {
        return multi_test(__down, args...);
    }
    template<typename... types>
    bool up(types... args) const
    {
        return multi_test(__up, args...);
    }
    template<typename... types>
    void scan_set(bool value, types... args)
    {
        multi_set(__scan, value, args...);
    }
    template<typename... types>
    void down_set(bool value, types... args)
    {
        multi_set(__down, value, args...);
    }
    template<typename... types>
    void up_set(bool value, types... args)
    {
        multi_set(__up, value, args...);
    }
    void scan_clear()
    {
        __scan.fill(false);
    }
    void down_clear()
    {
        __down.fill(false);
    }
    void up_clear()
    {
        __up.fill(false);
    }
    void update()
    {
        position = sdl::mouse_position();
        movement = sdl::mouse_movement();
    }

    mouse(const mouse&) = delete;
    mouse(mouse&&)      = delete;
    mouse& operator=(const mouse&) = delete;
    mouse& operator=(mouse&&) = delete;
};
} // namespace forge