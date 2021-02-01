
#pragma once
#include "../__common.hpp"
#include "../window.hpp"
#include "common.hpp"

namespace forge
{
namespace vulkan
{
class render
{
private:
    class context& context;
    class device&  device;

public:
    /** @todo Triple buffering. */
    static constexpr std::size_t buffer_count = 2;
    struct extent
    {
        vk::Extent2D value;

        void update(class window&);
    } extent;
    /** @brief Contains the frame buffers used in rendering and swapping. */
    vk::UniqueSwapchainKHR swapchain;
    /** @brief Views into the buffers in the swapchain. */
    std::vector<vk::UniqueImageView> image_views;
    /** @brief Render pass used in graphics pipeline. */
    vk::UniqueRenderPass pass;
    /** @brief Frame buffers used in rendering. */
    std::vector<vk::UniqueFramebuffer> framebuffers;

    render(class context&, class device&);
    void start();
};
} // namespace vulkan
} // namespace forge