
#include "rhi.hpp"

#include "shader.hpp"
#include "vertex.hpp"

#include <algorithm>
#include <exception>
#include <fstream>

namespace forge
{
namespace vulkan
{
context::context(class window& window) : window(window) { }
void context::start()
{
    /* INSTANCE */
    vk::ApplicationInfo app = vk::ApplicationInfo();
    app.setPNext(nullptr);
    app.setPApplicationName(window.title.c_str());
    app.setApplicationVersion(0);
    app.setPEngineName("forge");
    app.setEngineVersion(0);
    app.setApiVersion(VK_API_VERSION_1_2);

    std::uint32_t count  = 0;
    vk::Result    result = vk::Result::eSuccess;

    /** @todo Should only be enabled in debug mode. */
    result = vk::enumerateInstanceLayerProperties(&count, nullptr);
    if (result != vk::Result::eSuccess)
        throw std::runtime_error("Failed to get Vulkan layer count");
    auto __layers = std::vector<vk::LayerProperties>(count);
    result = vk::enumerateInstanceLayerProperties(&count, __layers.data());
    if (result != vk::Result::eSuccess)
        throw std::runtime_error("Failed to get Vulkan layer names");

    std::vector<const char*> layers;
    for (auto& layer : __layers)
    {
        auto& name = layer.layerName;
        auto  size = layer.layerName.size();
        if (!std::strncmp(name.data(), "VK_LAYER_KHRONOS_validation", size))
            layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    auto exts = sdl::vulkan::extensions(window.handle);

    auto info = vk::InstanceCreateInfo();
    info.setEnabledExtensionCount(exts.size());
    info.setPpEnabledExtensionNames(exts.data());
    if constexpr (__debug__)
    {
        info.setEnabledLayerCount(layers.size());
        info.setPpEnabledLayerNames(layers.data());
    }
    info.setPApplicationInfo(&app);

    instance = vk::createInstanceUnique(info);
    if (!instance) throw std::runtime_error("Failed to create Vulkan instance");

    /* SURFACE */
    auto handle = sdl::vulkan::create_surface(window.handle, *instance);
    surface     = vk::UniqueSurfaceKHR(handle, *instance);
}
bool device::queues::same()
{
    return graphics.index == present.index;
}
std::vector<std::uint32_t> device::queues::indices()
{
    auto indices = std::vector<std::uint32_t>();
    if (!same())
        indices = { graphics.index, present.index };
    else
        indices = { graphics.index };
    return indices;
}
device::device(class context& context) : context(context) { }
void device::start()
{
    /** @todo Add GPU selection protocol. */
    if (!context.instance->enumeratePhysicalDevices().size())
        throw std::runtime_error("Failed to find a compatible Vulkan device");
    physical = context.instance->enumeratePhysicalDevices().front();

    auto qf_props = physical.getQueueFamilyProperties();
    for (std::uint32_t i = 0; i < qf_props.size(); ++i)
    {
        if (qf_props.at(i).queueFlags & vk::QueueFlagBits::eGraphics)
            queues.graphics.index = i;
    }
    for (std::uint32_t i = 0; i < qf_props.size(); ++i)
    {
        if (physical.getSurfaceSupportKHR(i, context.surface.get()))
            queues.present.index = i;
    }

    auto indices = queues.indices();

    std::vector<vk::DeviceQueueCreateInfo> queue_infos;
    static float                           priority = 0.0f;
    for (auto& index : indices)
    {
        auto flags = vk::DeviceQueueCreateFlags();
        auto info  = vk::DeviceQueueCreateInfo(flags, index, 1, &priority);
        queue_infos.push_back(info);
    }
    std::vector<const char*> ext;
    for (auto& prop : physical.enumerateDeviceExtensionProperties())
    {
        auto& name = prop.extensionName;
        auto  size = name.size();
        if (!std::strncmp(name.data(), "VK_KHR_swapchain", size))
            ext.push_back("VK_KHR_swapchain");
        /** @todo Doesn't work. */
        // if (!std::strncmp(name.data(), "VK_KHR_portability_subset", size))
        //     ext.emplace_back("VK_KHR_portability_subset");
    }

    auto device_info = vk::DeviceCreateInfo();
    device_info.setFlags(vk::DeviceCreateFlags());
    device_info.setQueueCreateInfoCount(queue_infos.size());
    device_info.setPQueueCreateInfos(queue_infos.data());
    device_info.setEnabledLayerCount(0);
    device_info.setPpEnabledLayerNames(nullptr);
    device_info.setEnabledExtensionCount(ext.size());
    device_info.setPpEnabledExtensionNames(ext.data());

    logical = physical.createDeviceUnique(device_info);

    if constexpr (__debug__)
    {
        physical.getSurfaceCapabilitiesKHR(*context.surface);
        FORGE_EVAL_DISCARD(physical.getSurfaceFormatsKHR(*context.surface));
    }
}
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
pipeline::pipeline(class device& device, class render& render)
    : device(device), render(render)
{ }
void pipeline::start()
{
    /* PIPELINE */
    auto v_file   = std::fstream("assets/shaders/shader.vert");
    auto v_stream = std::stringstream();
    v_stream << v_file.rdbuf();
    std::vector<std::uint32_t> v_bytecode;
    auto v_info   = shader::create(v_stream.str(), shader::vertex, v_bytecode);
    auto v_shader = device.logical->createShaderModuleUnique(v_info);
    auto v_stage_info = vk::PipelineShaderStageCreateInfo();
    v_stage_info.setStage(vk::ShaderStageFlagBits::eVertex);
    v_stage_info.setModule(*v_shader);
    v_stage_info.setPName("main");

    auto f_file   = std::fstream("assets/shaders/shader.frag");
    auto f_stream = std::stringstream();
    f_stream << f_file.rdbuf();
    std::vector<std::uint32_t> f_bytecode;
    auto f_info = shader::create(f_stream.str(), shader::fragment, f_bytecode);
    auto f_shader     = device.logical->createShaderModuleUnique(f_info);
    auto f_stage_info = vk::PipelineShaderStageCreateInfo();
    f_stage_info.setStage(vk::ShaderStageFlagBits::eFragment);
    f_stage_info.setModule(*f_shader);
    f_stage_info.setPName("main");

    auto stage_infos = std::vector<vk::PipelineShaderStageCreateInfo>();
    stage_infos.push_back(v_stage_info);
    stage_infos.push_back(f_stage_info);

    auto binding_desc = vertex::binding_desc();
    auto attr_desc    = vertex::attr_desc();
    auto v_input_info = vk::PipelineVertexInputStateCreateInfo();
    v_input_info.setVertexBindingDescriptionCount(1);
    v_input_info.setPVertexBindingDescriptions(&binding_desc);
    v_input_info.setVertexAttributeDescriptionCount(attr_desc.size());
    v_input_info.setPVertexAttributeDescriptions(attr_desc.data());

    auto input_asm_info = vk::PipelineInputAssemblyStateCreateInfo();
    input_asm_info.setTopology(vk::PrimitiveTopology::eTriangleList);
    input_asm_info.setPrimitiveRestartEnable(false);

    auto& extent = render.extent.value;
    auto  viewport =
        vk::Viewport(0.0f, 0.0f, extent.width, extent.height, 0.0f, 1.0f);

    auto scissor = vk::Rect2D({ 0, 0 }, extent);

    auto viewport_info = vk::PipelineViewportStateCreateInfo();
    viewport_info.setViewportCount(1);
    viewport_info.setPViewports(&viewport);
    viewport_info.setScissorCount(1);
    viewport_info.setPScissors(&scissor);

    auto raster_info = vk::PipelineRasterizationStateCreateInfo();
    raster_info.setDepthClampEnable(false);
    raster_info.setRasterizerDiscardEnable(false);
    raster_info.setPolygonMode(vk::PolygonMode::eFill);
    raster_info.setFrontFace(vk::FrontFace::eCounterClockwise);
    raster_info.setLineWidth(1.0f);

    auto multisampling_info = vk::PipelineMultisampleStateCreateInfo();
    multisampling_info.setRasterizationSamples(vk::SampleCountFlagBits::e1);
    multisampling_info.setSampleShadingEnable(false);
    multisampling_info.setMinSampleShading(1.0f);

    auto color_blend_attach = vk::PipelineColorBlendAttachmentState();
    color_blend_attach.setSrcColorBlendFactor(vk::BlendFactor::eOne);
    color_blend_attach.setDstColorBlendFactor(vk::BlendFactor::eZero);
    color_blend_attach.setColorBlendOp(vk::BlendOp::eAdd);
    color_blend_attach.setSrcAlphaBlendFactor(vk::BlendFactor::eOne);
    color_blend_attach.setDstAlphaBlendFactor(vk::BlendFactor::eZero);
    auto cwm = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    color_blend_attach.setColorWriteMask(cwm);

    auto color_blend = vk::PipelineColorBlendStateCreateInfo();
    color_blend.setLogicOpEnable(false);
    color_blend.setLogicOp(vk::LogicOp::eCopy);
    color_blend.setAttachmentCount(1);
    color_blend.setPAttachments(&color_blend_attach);

    layout = device.logical->createPipelineLayoutUnique({}, nullptr);

    auto semaphore_info = vk::SemaphoreCreateInfo();
    available           = device.logical->createSemaphoreUnique(semaphore_info);
    finished            = device.logical->createSemaphoreUnique(semaphore_info);

    auto pipeline_info = vk::GraphicsPipelineCreateInfo();
    pipeline_info.setStageCount(2);
    pipeline_info.setPStages(stage_infos.data());
    pipeline_info.setPVertexInputState(&v_input_info);
    pipeline_info.setPInputAssemblyState(&input_asm_info);
    pipeline_info.setPTessellationState(nullptr);
    pipeline_info.setPViewportState(&viewport_info);
    pipeline_info.setPRasterizationState(&raster_info);
    pipeline_info.setPMultisampleState(&multisampling_info);
    pipeline_info.setPDepthStencilState(nullptr);
    pipeline_info.setPColorBlendState(&color_blend);
    pipeline_info.setPDynamicState(nullptr);
    pipeline_info.setLayout(*layout);
    pipeline_info.setRenderPass(*render.pass);

    graphics =
        device.logical->createGraphicsPipelineUnique({}, pipeline_info).value;
}
buffer::buffer(class device& device) : device(device) { }
void buffer::start()
{
    /** @todo Temp values. */
    auto          vertices = std::vector<struct vertex>(3);
    struct vertex v0 = { vector_2(0.0f, -0.5f), vector_3(1.0f, 1.0f, 1.0f) };
    struct vertex v1 = { vector_2(0.5f, 0.5f), vector_3(0.0f, 1.0f, 0.0f) };
    struct vertex v2 = { vector_2(-0.5f, 0.5f), vector_3(0.0f, 0.0f, 1.0f) };
    vertices.at(0)   = v0;
    vertices.at(1)   = v1;
    vertices.at(2)   = v2;

    auto buffer_info = vk::BufferCreateInfo();
    buffer_info.setSize(sizeof(struct vertex) * vertices.size());
    buffer_info.setUsage(vk::BufferUsageFlagBits::eVertexBuffer);
    buffer_info.setSharingMode(vk::SharingMode::eExclusive);

    this->vertex = device.logical->createBufferUnique(buffer_info);

    auto memory_reqs =
        device.logical->getBufferMemoryRequirements(this->vertex.get());
    const auto& memory_properties = device.physical.getMemoryProperties();
    auto        bits              = memory_reqs.memoryTypeBits;

    auto mask = vk::MemoryPropertyFlagBits::eHostVisible |
                vk::MemoryPropertyFlagBits::eHostCoherent;
    std::uint32_t index = ~0u;
    for (std::uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
    {
        if ((bits & 1) && ((memory_properties.memoryTypes.at(i).propertyFlags &
                            mask) == mask))
        {
            index = i;
            break;
        }
        bits >>= 1;
    }
    assert(index != ~0u);

    auto memory_info = vk::MemoryAllocateInfo();
    memory_info.setAllocationSize(memory_reqs.size);
    memory_info.setMemoryTypeIndex(index);
    memory = device.logical->allocateMemoryUnique(memory_info);

    auto __data = device.logical->mapMemory(memory.get(), 0, memory_reqs.size);
    std::uint8_t* data = static_cast<std::uint8_t*>(__data);

    std::memcpy(data, vertices.data(), sizeof(struct vertex) * vertices.size());
    device.logical->unmapMemory(memory.get());

    device.logical->bindBufferMemory(this->vertex.get(), memory.get(), 0);
}
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