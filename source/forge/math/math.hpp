
#pragma once
#include "../__common.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace forge
{
using matrix_33 = glm::fmat3x3;
using matrix_32 = glm::fmat3x2;

using vector_2 = glm::fvec2;
using vector_3 = glm::fvec3;

static const float sqrt_2 = std::sqrt(2.0f);
static const float sqrt_3 = std::sqrt(3.0f);
static const float sqrt_6 = std::sqrt(6.0f);

vector_2 reduce(const vector_3&);
}