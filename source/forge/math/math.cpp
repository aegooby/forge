
#include "math.hpp"

namespace forge
{
vector_2 reduce(const vector_3& vector)
{
    return vector_2(vector.x, vector.y);
}
}