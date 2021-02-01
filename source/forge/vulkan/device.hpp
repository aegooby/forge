
#pragma once
#include "../__common.hpp"
#include "common.hpp"

namespace forge
{
namespace vulkan
{
class device
{
private:
    class context& context;

public:
    struct queue
    {
        std::uint32_t index = ~0u;
        vk::Queue     queue;
    };
    /** @brief Represents the GPU used. */
    vk::PhysicalDevice physical;
    /** @brief Interface for physical device(s). */
    vk::UniqueDevice logical;

    struct queues
    {
        queue graphics;
        queue present;

        bool                       same();
        std::vector<std::uint32_t> indices();
    } queues;

    device(class context&);
    void start();
};
} // namespace vulkan
} // namespace forge