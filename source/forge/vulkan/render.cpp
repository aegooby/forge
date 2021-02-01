
#include "render.hpp"

#include "../sdl.hpp"
#include "../window.hpp"
#include "context.hpp"
#include "device.hpp"

namespace forge
{
namespace vulkan
{
void render::extent::update(class window& window)
{
    auto pair = sdl::vulkan::extent(window.handle);
    value     = vk::Extent2D(pair.first, pair.second);
}
render::render(class context& context, class device& device)
    : context(context), device(device)
{ }
void render::start()
{
    /* SWAPCHAIN */
    auto info = vk::SwapchainCreateInfoKHR();
    info.setSurface(context.surface.get());
    info.setMinImageCount(buffer_count);
    info.setImageFormat(format);
    info.setImageColorSpace(vk::ColorSpaceKHR::eSrgbNonlinear);
    info.setImageExtent(extent.value);
    info.setImageArrayLayers(1);
    info.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment);
    auto queue_indices = device.queues.indices();
    if (!device.queues.same())
    {
        info.setImageSharingMode(vk::SharingMode::eConcurrent);
        info.setQueueFamilyIndexCount(queue_indices.size());
        info.setPQueueFamilyIndices(queue_indices.data());
    }
    else
    {
        info.setImageSharingMode(vk::SharingMode::eExclusive);
        info.setQueueFamilyIndexCount(0);
        info.setPQueueFamilyIndices(nullptr);
    }
    info.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
    info.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
    info.setPresentMode(vk::PresentModeKHR::eFifo);
    info.setClipped(true);
    info.setOldSwapchain(nullptr);

    swapchain = device.logical->createSwapchainKHRUnique(info);

    auto images = device.logical->getSwapchainImagesKHR(swapchain.get());
    image_views.clear();
    for (auto& image : images)
    {
        using sw        = vk::ComponentSwizzle;
        using iafb      = vk::ImageAspectFlagBits;
        auto components = vk::ComponentMapping(sw::eIdentity, sw::eIdentity,
                                               sw::eIdentity, sw::eIdentity);
        auto isr        = vk::ImageSubresourceRange(iafb::eColor, 0, 1, 0, 1);

        auto info = vk::ImageViewCreateInfo();
        info.setFlags(vk::ImageViewCreateFlags());
        info.setImage(image);
        info.setViewType(vk::ImageViewType::e2D);
        info.setFormat(format);
        info.setComponents(components);
        info.setSubresourceRange(isr);

        image_views.push_back(device.logical->createImageViewUnique(info));
    }

    /* RENDER PASS */
    auto color_attachment = vk::AttachmentDescription();
    color_attachment.setFormat(format);
    color_attachment.setSamples(vk::SampleCountFlagBits::e1);
    color_attachment.setLoadOp(vk::AttachmentLoadOp::eClear);
    color_attachment.setStoreOp(vk::AttachmentStoreOp::eStore);
    color_attachment.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    auto color_attachment_ref = vk::AttachmentReference();
    color_attachment_ref.setAttachment(0);
    color_attachment_ref.setLayout(vk::ImageLayout::eColorAttachmentOptimal);

    auto subpass = vk::SubpassDescription();
    subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics);
    subpass.setInputAttachmentCount(0);
    subpass.setPInputAttachments(nullptr);
    subpass.setColorAttachmentCount(1);
    subpass.setPColorAttachments(&color_attachment_ref);

    auto subpass_dependency = vk::SubpassDependency();
    subpass_dependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
    subpass_dependency.setDstSubpass(0);
    auto stage_mask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    subpass_dependency.setSrcStageMask(stage_mask);
    subpass_dependency.setDstStageMask(stage_mask);
    auto access_mask = vk::AccessFlagBits::eColorAttachmentRead |
                       vk::AccessFlagBits::eColorAttachmentWrite;
    subpass_dependency.setDstAccessMask(access_mask);

    auto pass_info = vk::RenderPassCreateInfo();
    pass_info.setAttachmentCount(1);
    pass_info.setPAttachments(&color_attachment);
    pass_info.setSubpassCount(1);
    pass_info.setPSubpasses(&subpass);
    pass_info.setDependencyCount(1);
    pass_info.setPDependencies(&subpass_dependency);

    pass = device.logical->createRenderPassUnique(pass_info);

    /* FRAMEBUFFERS */
    framebuffers.clear();
    for (std::size_t i = 0; i < image_views.size(); ++i)
    {
        auto info = vk::FramebufferCreateInfo();
        info.setRenderPass(*pass);
        info.setAttachmentCount(1);
        info.setPAttachments(&image_views.at(i).get());
        info.setWidth(extent.value.width);
        info.setHeight(extent.value.height);
        info.setLayers(1);
        framebuffers.push_back(device.logical->createFramebufferUnique(info));
    }
}
} // namespace vulkan
} // namespace forge