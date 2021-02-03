
#pragma once

#include "../__common.hpp"
#include "../math.hpp"
#include "common.hpp"

namespace forge
{
namespace vulkan
{
namespace math
{
struct vertex
{
    vector_2 position;
    vector_3 color;

    vertex() = default;
    vertex(const vector_2& position, const vector_3& color)
        : position(position), color(color)
    { }

    static vk::VertexInputBindingDescription binding_desc()
    {
        auto result = vk::VertexInputBindingDescription();

        result.setBinding(0);
        result.setStride(sizeof(vertex));

        return result;
    }
    static std::array<vk::VertexInputAttributeDescription, 2> attr_desc()
    {
        auto result = std::array<vk::VertexInputAttributeDescription, 2>();

        result.at(0).binding  = 0;
        result.at(0).location = 0;
        result.at(0).format   = vk::Format::eR32G32Sfloat;
        result.at(0).offset   = offsetof(vertex, position);

        result.at(1).binding  = 0;
        result.at(1).location = 1;
        result.at(1).format   = vk::Format::eR32G32B32Sfloat;
        result.at(1).offset   = offsetof(vertex, color);

        return result;
    }
};
} // namespace math
} // namespace vulkan
} // namespace forge