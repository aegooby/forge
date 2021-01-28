
#include "shader.hpp"

namespace forge
{
namespace vulkan
{
shaderc::Compiler          shader::spirv = shaderc::Compiler();
vk::ShaderModuleCreateInfo shader::create(const std::string& shader, kind kind,
                                          std::vector<std::uint32_t>& bytecode)
{
    auto result = spirv.CompileGlslToSpv(shader, kind, "");
    if (result.GetCompilationStatus() != success)
        throw std::runtime_error(result.GetErrorMessage());
    bytecode = std::vector<std::uint32_t>(result.cbegin(), result.cend());

    auto info = vk::ShaderModuleCreateInfo();
    info.setCodeSize(bytecode.size() * sizeof(std::uint32_t));
    info.setPCode(bytecode.data());
    return info;
}
} // namespace vulkan
} // namespace forge