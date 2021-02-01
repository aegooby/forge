
#pragma once
#include "../__common.hpp"
#include "common.hpp"
#include "vertex.hpp"

namespace forge
{
namespace vulkan
{
class pipeline
{
private:
    class device& device;
    class render& render;

public:
    /** @brief Allows passing dynamic values into shaders. */
    vk::UniquePipelineLayout layout;
    /** @brief Indicates that the image view buffer is ready to be drawn on. */
    vk::UniqueSemaphore available;
    /** @brief Indicates that rendering is done. */
    vk::UniqueSemaphore finished;
    /** @brief Graphics pipeline. */
    vk::UniquePipeline graphics;

    pipeline(class device&, class render&);
    void start();
};
} // namespace vulkan
} // namespace forge