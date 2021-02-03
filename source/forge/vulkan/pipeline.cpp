
#include "pipeline.hpp"

#include "device.hpp"
#include "render.hpp"
#include "shader.hpp"

#include <fstream>

namespace forge
{
namespace vulkan
{
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

    auto binding_desc = math::vertex::binding_desc();
    auto attr_desc    = math::vertex::attr_desc();
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
} // namespace vulkan
} // namespace forge