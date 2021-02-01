
#include "device.hpp"

#include "context.hpp"

namespace forge
{
namespace vulkan
{
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
        // if (!std::strncmp(name.data(), "VK_KHR_portability_subset",
        // size))
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
} // namespace vulkan
} // namespace forge