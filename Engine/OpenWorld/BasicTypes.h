#pragma once

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

#define VEC2(ptr) glm::vec2(ptr[0], ptr[1])
#define VEC3(ptr) glm::vec3(ptr[0], ptr[1], ptr[2])
#define VEC4(ptr) glm::vec4(ptr[0], ptr[1], ptr[2], ptr[3])

#define FLOAT_EQ(a, b) (std::abs(a - b) < std::numeric_limits<float>::epsilon())

enum DataType
{
    InvalidDataType = 0,
    DATA_Uchar,
    DATA_Ushort,
    DATA_Uint,
    DATA_Float,
    DATA_Float2,
    DATA_Float3,
    DATA_Float4,
    DATA_Float4x4,
};

enum PrimitiveType
{
    InvalidPrimitiveType = 0,
    PRIM_Points,
    PRIM_Lines,
    PRIM_Triangles,
};

enum PixelFormat
{
    InvalidPixelFormat = 0,
    R32Uint = 53,          // MTLPixelFormatR32Uint
    R32Float = 55,         // MTLPixelFormatR32Float
    
    RG32Uint = 103,        // MTLPixelFormatRG32Uint
    RG32Float = 105,       // MTLPixelFormatRG32Float
    
    RGBA32Uint = 123,      // MTLPixelFormatRGBA32Uint
    RGBA32Float = 125,     // MTLPixelFormatRGBA32Float
    RGBA8Unorm = 70,       // MTLPixelFormatRGBA8Unorm
    BGRA8Unorm = 80,       // MTLPixelFormatBGRA8Unorm
    
    Depth32Float = 252,    // MTLPixelFormatDepth32Float
    Stencil8Uint = 253,    // MTLPixelFormatStencil8
    Depth32Stencil8 = 260, // MTLPixelFormatDepth32Float_Stencil8
};

#ifdef __APPLE__
#define PixelFormat_SystemRTV BGRA8Unorm
#else
#define PixelFormat_SystemRTV RGBA8Unorm
#endif

#define VAO_POSITION "position"
#define VAO_NORMAL   "normal"
#define VAO_TEXCOORD "texcoord"
#define MODEL_MATRIX "modelMatrix"
#define VIEW_MATRIX  "viewMatrix"
#define PROJECTION_MATRIX  "projectionMatrix"

namespace fs = std::filesystem;

class BoundingBox
{
public:
    BoundingBox() : _min(0.0), _max(0.0) {}
    BoundingBox(glm::vec3 min, glm::vec3 max) : _min(min), _max(max) {}
    
    glm::vec3 const& min() const { return _min; }
    glm::vec3 const& max() const { return _max; }
    glm::vec3 center() const { return (_min + _max) * 0.5; }
    glm::vec3 extent() const { return (_max - _min); }
    float radius() const { return glm::length(_max - _min) * 0.5; }
    bool isValid() const { return !glm::all(glm::equal(_min, _max)); }
    void expand(const BoundingBox& box) {
        expand(box._min);
        expand(box._max);
    }
    void expand(const glm::vec3& point) {
        _min = glm::min(_min, point);
        _max = glm::max(_max, point);
    }
    
private:
    glm::vec3 _min;
    glm::vec3 _max;
};
