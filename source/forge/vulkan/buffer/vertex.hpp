
#pragma once
#include "../../__common.hpp"
#include "../common.hpp"
#include "buffer.hpp"

namespace forge
{
namespace vulkan
{
namespace buffers
{
class vertex : public buffer
{
public:
    using __base = buffer;

private:
public:
    vertex(class device&);
    void start();
};
} // namespace buffers
} // namespace vulkan
} // namespace forge