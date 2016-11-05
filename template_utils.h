#ifndef TEMPLATE_UTILS_H
#define TEMPLATE_UTILS_H

#include <tuple>
#include <type_traits>

namespace ray_tracing
{

template<typename R, typename F, typename T, int... N>
typename std::enable_if<sizeof...(N) == std::tuple_size<T>::value, R>::type
    call(F f, T&& t)
{
    return f(std::get<N>(std::forward<T>(t))...);
}

template<typename R, typename F, typename T, int... N>
typename std::enable_if<sizeof...(N) < std::tuple_size<T>::value, R>::type
    call(F f, T&& t)
{
    return call<R, F, T, N..., sizeof...(N)>(f, std::forward<T>(t));
}

}

#endif // TEMPLATE_UTILS_H
