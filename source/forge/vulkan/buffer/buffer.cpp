
#include "buffer.hpp"

#include "../../math.hpp"
#include "../device.hpp"

namespace forge
{
namespace vulkan
{
buffer::buffer(class device& device) : device(device) { }
void buffer::start()
{
    auto buffer_info = vk::BufferCreateInfo();
    buffer_info.setSize(size);
    buffer_info.setUsage(usage);
    buffer_info.setSharingMode(vk::SharingMode::eExclusive);

    data = device.logical->createBufferUnique(buffer_info);

    memory_reqs = device.logical->getBufferMemoryRequirements(data.get());
    const auto& memory_properties = device.physical.getMemoryProperties();
    auto        bits              = memory_reqs.memoryTypeBits;

    std::uint32_t index = ~0u;
    for (std::uint32_t i = 0; i < memory_properties.memoryTypeCount; ++i)
    {
        if ((bits & 1) && ((memory_properties.memoryTypes.at(i).propertyFlags &
                            property) == property))
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

    device.logical->bindBufferMemory(data.get(), memory.get(), 0);
}
} // namespace vulkan
} // namespace forge