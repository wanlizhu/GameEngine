#pragma once

#include <stdint.h>

template<typename T>
struct function_traits : public function_traits<decltype(&T::operator())>
{};

template<typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType(ClassType::*)(Args...) const>
{
    enum { arity = sizeof...(Args) };
    typedef ReturnType result_type;

    template<uint64_t i>
    struct arg
    {
        typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
    };
};

template <typename T>
constexpr typename std::underlying_type<T>::type enum_cast(T x)
{
    return static_cast<typename std::underlying_type<T>::type>(x);
}