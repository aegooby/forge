
#include "rhi.hpp"

#include "shader.hpp"
#include "vertex.hpp"

namespace forge
{
rhi::rhi(class window& window) : window(window) { }
void rhi::start()
{
    context.start();
    device.start();
    render.extent.update(window);
    render.start();
    pipeline.start();
    buffer.start();
    command.start();
}
void rhi::draw()
{
    auto timeout = std::numeric_limits<uint64_t>::max();

    auto& swapchain = render.swapchain.get();
    auto& available = pipeline.available.get();
    auto& finished  = pipeline.finished.get();
    auto  index =
        device.logical->acquireNextImageKHR(swapchain, timeout, available, {});

    switch (index.result)
    {
        case vk::Result::eErrorOutOfDateKHR:
        {
            /** @todo Swapchain recreation. */
            break;
        }
        case vk::Result::eSuccess:
            break;
        case vk::Result::eSuboptimalKHR:
            break;
        default:
            throw std::runtime_error("Failed to acquire Vulkan image");
            break;
    }

    vk::PipelineStageFlags mask =
        vk::PipelineStageFlagBits::eColorAttachmentOutput;

    auto submit_info = vk::SubmitInfo();
    submit_info.setWaitSemaphoreCount(1);
    submit_info.setPWaitSemaphores(&available);
    submit_info.setPWaitDstStageMask(&mask);
    submit_info.setCommandBufferCount(1);
    submit_info.setPCommandBuffers(&command.buffers.at(index.value).get());
    submit_info.setSignalSemaphoreCount(1);
    submit_info.setPSignalSemaphores(&finished);

    device.queues.graphics.queue.submit(submit_info, {});

    auto present_info = vk::PresentInfoKHR();
    present_info.setWaitSemaphoreCount(1);
    present_info.setPWaitSemaphores(&finished);
    present_info.setSwapchainCount(1);
    present_info.setPSwapchains(&swapchain);
    present_info.setPImageIndices(&index.value);
    auto result = device.queues.present.queue.presentKHR(present_info);
    if (result != vk::Result::eSuccess)
        throw std::runtime_error("Failed to present Vulkan");

    device.logical->waitIdle();
}
} // namespace forge