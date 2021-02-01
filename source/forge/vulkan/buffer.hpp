
#pragma once
#include "../__common.hpp"
#include "common.hpp"

namespace forge
{
namespace vulkan
{
class buffer
{
private:
    class device& device;

public:
    vk::UniqueDeviceMemory memory;
    /** @brief Stores vertex inputs to pass to shaders. */
    vk::UniqueBuffer vertex;

    buffer(class device&);
    void start();
};
} // namespace vulkan
} // namespace forge