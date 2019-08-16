#pragma once

namespace Engine
{
    #define MATH_LOOP_OPERATION(iterator, number, operation)                        \
    for (int iterator = 0; iterator < number; ++iterator)                           \
    {                                                                               \
        operation;                                                                  \
    }

    #define MATH_TYPE_DEGREE_FUN(T, degree, func, ret)                              \
    if (std::is_same<T, double>::value)                                             \
        ret = static_cast<T>(std::func(degree*PI/180.0));                           \
    else                                                                            \
        ret = static_cast<T>(std::func##f(degree*PI_F/180.f));
}