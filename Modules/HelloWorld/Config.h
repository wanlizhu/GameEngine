#pragma once

#ifndef __METAL_VERSION__
#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <cerrno>
#include <cassert>
#include <clocale>
#include <climits>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <list>
#include <set>
#include <stack>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <optional>
#include <any>
#include <variant>
#include <typeindex>
#include <memory>
#include <fstream>
#include <sstream>
#include <filesystem>
#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/noise.hpp"
#include "glm/gtc/epsilon.hpp"
#include "glm/gtc/color_space.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/round.hpp"
#include "glm/gtc/ulp.hpp"
#include "glm/gtx/closest_point.hpp"
#include "glm/gtx/color_encoding.hpp"
#include "glm/gtx/color_space.hpp"
#include "glm/gtx/common.hpp"
#include "glm/gtx/component_wise.hpp"
#include "glm/gtx/dual_quaternion.hpp"
#include "glm/gtx/easing.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/extend.hpp"
#include "glm/gtx/exterior_product.hpp"
#include "glm/gtx/fast_exponential.hpp"
#include "glm/gtx/fast_square_root.hpp"
#include "glm/gtx/fast_trigonometry.hpp"
#include "glm/gtx/functions.hpp"
#include "glm/gtx/gradient_paint.hpp"
#include "glm/gtx/handed_coordinate_space.hpp"
#include "glm/gtx/hash.hpp"
#include "glm/gtx/intersect.hpp"
#include "glm/gtx/io.hpp"
#include "glm/gtx/matrix_cross_product.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/matrix_factorisation.hpp"
#include "glm/gtx/matrix_interpolation.hpp"
#include "glm/gtx/matrix_major_storage.hpp"
#include "glm/gtx/matrix_operation.hpp"
#include "glm/gtx/matrix_query.hpp"
#include "glm/gtx/matrix_transform_2d.hpp"
#include "glm/gtx/mixed_product.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/normal.hpp"
#include "glm/gtx/normalize_dot.hpp"
#include "glm/gtx/number_precision.hpp"
#include "glm/gtx/optimum_pow.hpp"
#include "glm/gtx/orthonormalize.hpp"
#include "glm/gtx/perpendicular.hpp"
#include "glm/gtx/polar_coordinates.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/range.hpp"
#include "glm/gtx/raw_data.hpp"
#include "glm/gtx/rotate_normalized_axis.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/scalar_multiplication.hpp"
#include "glm/gtx/scalar_relational.hpp"
#include "glm/gtx/spline.hpp"
#include "glm/gtx/std_based_type.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/texture.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/transform2.hpp"
#include "glm/gtx/vec_swizzle.hpp"
#include "glm/gtx/vector_angle.hpp"
#include "glm/gtx/vector_query.hpp"
#include "glm/gtx/wrap.hpp"

#ifdef __OBJC__
#include <TargetConditionals.h>
#include <MetalKit/MetalKit.h>
#include <ModelIO/ModelIO.h>
#if TARGET_OS_IOS
#   include <UIKit/UIKit.h>
#else
#   include <Cocoa/Cocoa.h>
#endif
#endif

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef min
#undef max
#endif

namespace fs = std::filesystem;
using Int2 = glm::ivec2;
using Int3 = glm::ivec3;
using Int4 = glm::ivec4;
using Float2 = glm::vec2;
using Float3 = glm::vec3;
using Float4 = glm::vec4;
using Float3x3 = glm::mat3;
using Float4x4 = glm::mat4;

#endif // #ifndef __METAL_VERSION__

enum VertexBufferIndex
{
    VBO_Position = 0,
    VBO_Normal = 1,
    VBO_Tangent = 2,
    VBO_Texcoord = 3,
    UBO_Camera = 4,
    UBO_Model = 5,
    UBO_MaterialPBR = 6,
};
