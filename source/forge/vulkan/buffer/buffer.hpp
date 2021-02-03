
#pragma once
#include "../../__common.hpp"
#include "../common.hpp"

namespace forge
{
namespace vulkan
{
class buffer
{
protected:
    /* CONTEXT OBJECTS */
    class device& device;

    /* VULKAN OBJECTS */
    vk::DeviceSize          size;
    vk::BufferUsageFlagBits usage;
    vk::MemoryPropertyFlags property;
    vk::MemoryRequirements  memory_reqs;

public:
    vk::UniqueDeviceMemory memory;
    /** @brief Stores vertex inputs to pass to shaders. */
    vk::UniqueBuffer data;

    buffer(class device&);
    void start();
};
} // namespace vulkan
} // namespace forge