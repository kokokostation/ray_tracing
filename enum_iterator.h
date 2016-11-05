#ifndef ENUM_ITERATOR
#define ENUM_ITERATOR

#include <iterator>

namespace ray_tracing
{

template<typename E>
class range
{
private:
    typedef typename std::underlying_type<E>::type under;

public:
    class enum_iterator
    {
    private:
        E value;
        typedef typename std::underlying_type<E>::type under;

    public:
        constexpr explicit enum_iterator(E value) : value(value) {}
        enum_iterator& operator++()
        {
            value = E(under(value) + 1);
            return *this;
        }

        E& operator*() {return value;}
        bool operator==(const enum_iterator& enum_iterator) {return value == enum_iterator.value;}
        bool operator!=(const enum_iterator& enum_iterator) {return !(*this == enum_iterator);}
    };

    constexpr size_t size() {return under(E::Last) - under(E::First);}
    constexpr enum_iterator begin() {return enum_iterator(E::First);}
    constexpr enum_iterator end() {return enum_iterator(E::Last);}
};

}

#endif // ENUM_ITERATOR

