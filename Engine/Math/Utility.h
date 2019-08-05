#pragma once

namespace Engine
{
    #define MATH_LOOP_OPERATION(iterator, number, operation)                        \
    for (int iterator = 0; iterator < number; ++iterator)                           \
    {                                                                               \
        operation;                                                                  \
    }
}