
#pragma once
#include "../__common.hpp"
#include "../math.hpp"
#include "buffer.hpp"
#include "command.hpp"
#include "common.hpp"
#include "context.hpp"
#include "device.hpp"
#include "pipeline.hpp"
#include "render.hpp"

#include <array>

namespace forge
{
class rhi
{
private:
    /* CONTEXT OBJECTS */
    class window& window;

protected:
    vulkan::context         context  = vulkan::context(window);
    vulkan::device          device   = vulkan::device(context);
    vulkan::render          render   = vulkan::render(context, device);
    vulkan::pipeline        pipeline = vulkan::pipeline(device, render);
    vulkan::buffers::vertex vertex   = vulkan::buffers::vertex(device);
    vulkan::command command = vulkan::command(device, render, pipeline, vertex);

public:
    rhi(class window&);
    ~rhi() = default;

    /* START */
    void start();

    /* DRAWING */
    /** @todo Temp. */
    void draw();
};
} // namespace forge