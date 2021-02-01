
#pragma once
#include "../__common.hpp"
#include "common.hpp"
#include "rhi.hpp"

namespace forge
{
namespace vulkan
{
class shader
{
public:
    using kind                     = shaderc_shader_kind;
    static constexpr auto infer    = shaderc_glsl_infer_from_source;
    static constexpr auto vertex   = shaderc_glsl_vertex_shader;
    static constexpr auto fragment = shaderc_glsl_fragment_shader;
    static constexpr auto success  = shaderc_compilation_status_success;

private:
    static shaderc::Compiler spirv;

public:
    [[nodiscard]] static vk::ShaderModuleCreateInfo
    create(const std::string&, kind, std::vector<std::uint32_t>&);
};
} // namespace vulkan
} // namespace forge