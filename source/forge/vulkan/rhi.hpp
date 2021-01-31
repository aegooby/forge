
#pragma once
#include "../__common.hpp"
#include "../math.hpp"
#include "../sdl.hpp"
#include "../window.hpp"

#include <array>
#include <vulkan/vulkan.hpp>

namespace forge
{
namespace vulkan
{
static constexpr vk::Format format = vk::Format::eB8G8R8A8Unorm;
class context
{
private:
    class window& window;

public:
    /**
     * @brief Connects application to Vulkan library.
     */
    vk::UniqueInstance instance;
    /** @brief Connects Vulkan instance to window. Requires "VK_KHR_surface". */
    vk::UniqueSurfaceKHR surface;

    context(class window&);
    void start();
};
struct queue
{
    std::uint32_t index = ~0u;
    vk::Queue     queue;
};
class device
{
private:
    class context& context;

public:
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
class command
{
private:
    class device&   device;
    class render&   render;
    class pipeline& pipeline;
    class buffer&   buffer;

public:
    /** @brief Command pool. No idea what this does yet. */
    vk::UniqueCommandPool pool;
    /** @brief Command buffers. No idea what this does yet. */
    std::vector<vk::UniqueCommandBuffer> buffers;

    command(class device&, class render&, class pipeline&, class buffer&);
    void start();
};
} // namespace vulkan
class rhi
{
private:
    /* CONTEXT OBJECTS */
    class window& window;

protected:
    vulkan::context  context  = vulkan::context(window);
    vulkan::device   device   = vulkan::device(context);
    vulkan::render   render   = vulkan::render(context, device);
    vulkan::pipeline pipeline = vulkan::pipeline(device, render);
    vulkan::buffer   buffer   = vulkan::buffer(device);
    vulkan::command command = vulkan::command(device, render, pipeline, buffer);

public:
    rhi(class window&);
    ~rhi() = default;

    /* START */
    void start();

    /* DRAWING */
    /** @todo Temp. */
    void draw();

    /* UTIL */
    void query_extent();
};
} // namespace forge