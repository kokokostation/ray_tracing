#ifndef PARSER
#define PARSER

#include <tuple>
#include <type_traits>
#include <iostream>
#include <string>

#include "tracer.h"
#include "geometry.h"

//parsing is performed without error handler

namespace ray_tracing
{

template<size_t I = 0, typename Functor, typename... T>
typename std::enable_if<I == sizeof...(T), void>::type
    for_index(size_t, Functor, std::tuple<T...>&)
{
}

template<size_t I = 0, typename Functor, typename... T>
typename std::enable_if<I < sizeof...(T), void>::type
    for_index(size_t index, Functor f, std::tuple<T...>& t)
{
    if(index == 0)
        f(std::get<I>(t));

    for_index<I + 1, Functor, T...>(index - 1, f, t);
}

class Reader
{
private:
    std::istream& stream;

public:
    Reader(std::istream& stream)
        : stream(stream)
    {}

    template<typename T>
    void operator()(T& t)
    {
        stream >> t;
    }
};

template<typename... T>
std::tuple<T...> parse(std::array<std::string, sizeof...(T)> names, std::istream& stream)
{
    std::tuple<T...> result;
    std::vector<char> checked(names.size());

    for(size_t i = 0; i < names.size(); ++i)
    {
        std::string input;
        stream >> input;

        size_t number;
        for(number = 0; checked[number] || names[number] != input; ++number);

        checked[number] = true;

        for_index(number, Reader(stream), result);
    }

    return result;
}

void assert_read(std::istream& stream, const std::string& str);

template<typename T, size_t N>
std::array<T, N> parse_array(std::istream& stream, const std::string& keyword)
{
    std::array<T, N> result;

    for(T& i : result)
    {
        assert_read(stream, keyword);
        stream >> i;
    }

    return result;
}

Scene parse(std::istream& stream);

}

#endif // Parser
