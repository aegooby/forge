
#include "vertex.hpp"

#include "../device.hpp"
#include "../vertex.hpp"

namespace forge
{
namespace vulkan
{
namespace buffers
{
vertex::vertex(class device& device) : __base(device) { }
void vertex::start()
{
    /** @todo Temp values. */
    auto vertices = std::vector<math::vertex>();
    vertices.emplace_back(vector_2(0.0f, -0.5f), vector_3(1.0f, 1.0f, 1.0f));
    vertices.emplace_back(vector_2(0.5f, 0.5f), vector_3(0.0f, 1.0f, 0.0f));
    vertices.emplace_back(vector_2(-0.5f, 0.5f), vector_3(0.0f, 0.0f, 1.0f));

    __base::size     = sizeof(math::vertex) * vertices.size();
    __base::usage    = vk::BufferUsageFlagBits::eVertexBuffer;
    __base::property = vk::MemoryPropertyFlagBits::eHostVisible |
                       vk::MemoryPropertyFlagBits::eHostCoherent;

    __base::start();

    auto ptr = device.logical->mapMemory(memory.get(), 0, memory_reqs.size);
    std::uint8_t* __ptr = static_cast<std::uint8_t*>(ptr);

    std::memcpy(__ptr, vertices.data(), sizeof(math::vertex) * vertices.size());
    device.logical->unmapMemory(memory.get());
}
} // namespace buffers
} // namespace vulkan
} // namespace forge