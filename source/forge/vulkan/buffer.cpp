
#include "buffer.hpp"

#include "../math.hpp"
#include "device.hpp"
#include "vertex.hpp"

namespace forge
{
namespace vulkan
{
buffer::buffer(class device& device) : device(device) { }
void buffer::start()
{
    /** @todo Temp values. */
    auto vertices = std::vector<struct vertex>();
    vertices.emplace_back(vector_2(0.0f, -0.5f), vector_3(1.0f, 1.0f, 1.0f));
    vertices.emplace_back(vector_2(0.5f, 0.5f), vector_3(0.0f, 1.0f, 0.0f));
    vertices.emplace_back(vector_2(-0.5f, 0.5f), vector_3(0.0f, 0.0f, 1.0f));

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
} // namespace vulkan
} // namespace forge