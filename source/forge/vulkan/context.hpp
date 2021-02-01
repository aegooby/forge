
#pragma once
#include "../__common.hpp"
#include "../window.hpp"
#include "common.hpp"

namespace forge
{
namespace vulkan
{
class context
{
private:
    class window& window;

public:
    /** @brief Connects application to Vulkan library. */
    vk::UniqueInstance instance;
    /** @brief Connects Vulkan instance to window. Requires "VK_KHR_surface". */
    vk::UniqueSurfaceKHR surface;

    context(class window&);
    void start();
};
} // namespace vulkan
} // namespace forge