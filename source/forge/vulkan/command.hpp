
#pragma once
#include "../__common.hpp"
#include "common.hpp"

namespace forge
{
namespace vulkan
{
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
} // namespace forge