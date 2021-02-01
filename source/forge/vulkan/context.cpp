
#include "context.hpp"

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
    app.setApplicationVersion(VK_MAKE_VERSION(1, 0, 0));
    app.setPEngineName(__name__);
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
} // namespace vulkan
} // namespace forge