
#include "command.hpp"

#include "buffer.hpp"
#include "device.hpp"
#include "pipeline.hpp"
#include "render.hpp"

namespace forge
{
namespace vulkan
{
command::command(class device& device, class render& render,
                 class pipeline& pipeline, class buffer& buffer)
    : device(device), render(render), pipeline(pipeline), buffer(buffer)
{ }
void command::start()
{
    /* POOL */
    auto pool_info = vk::CommandPoolCreateInfo();
    pool_info.setQueueFamilyIndex(device.queues.graphics.index);
    pool = device.logical->createCommandPoolUnique(pool_info);

    /* BUFFERS */
    auto info = vk::CommandBufferAllocateInfo();
    info.setCommandPool(pool.get());
    info.setLevel(vk::CommandBufferLevel::ePrimary);
    info.setCommandBufferCount(render.framebuffers.size());
    buffers = device.logical->allocateCommandBuffersUnique(info);

    auto& graphics = device.queues.graphics;
    auto& present  = device.queues.present;

    graphics.queue = device.logical->getQueue(graphics.index, 0);
    present.queue  = device.logical->getQueue(present.index, 0);

    for (size_t i = 0; i < buffers.size(); i++)
    {
        auto begin_info = vk::CommandBufferBeginInfo();
        buffers.at(i)->begin(begin_info);
        auto clear_values     = vk::ClearValue();
        auto render_pass_info = vk::RenderPassBeginInfo();
        render_pass_info.setRenderPass(render.pass.get());
        render_pass_info.setFramebuffer(render.framebuffers.at(i).get());
        auto& extent = render.extent.value;
        render_pass_info.setRenderArea(vk::Rect2D({ 0, 0 }, extent));
        render_pass_info.setClearValueCount(1);
        render_pass_info.setPClearValues(&clear_values);

        buffers.at(i)->beginRenderPass(render_pass_info,
                                       vk::SubpassContents::eInline);
        buffers.at(i)->bindVertexBuffers(0, *buffer.vertex, { 0 });
        buffers.at(i)->bindPipeline(vk::PipelineBindPoint::eGraphics,
                                    *pipeline.graphics);
        buffers.at(i)->draw(3, 1, 0, 0);
        buffers.at(i)->endRenderPass();
        buffers.at(i)->end();
    }
}
} // namespace vulkan
} // namespace forge