#pragma once
#include <functional>                   // function
#include <boost/assert.hpp>             // BOOST_ASSERT
#include <dctl/bit/bit_fwd.hpp>         // forward declarations
#include <dctl/bit/intrinsic.hpp>       // count, find
#include <dctl/bit/lookup.hpp>          // count, find
#include <dctl/bit/loop.hpp>            // count, find
#include <dctl/utility/int.hpp>         // num_bits

namespace dctl {
namespace bit {

template<typename T>
T zero()
{
        return { 0 };
}

template<typename T>
T universe()
{
        return ~zero<T>();
}

template<typename T>
T singlet(int i)
{
        return T(1) << i;
}

template<typename T>
T reverse_singlet(int i)
{
        return singlet<T>(num_bits<T>::value - 1 - i);
}

namespace first {
namespace detail {

template<typename T>
struct equal
{
        T operator()(T b) const
        {
                return b & (T(0) - b);
        }
};

template<typename T>
struct not_equal_to
{
        T operator()(T b) const
        {
                return b & (b - T(1));
        }
};

template<typename T>
struct less
{
        T operator()(T b) const
        {
                return ~b & (b - T(1));
        }
};

template<typename T>
struct less_equal
{
        T operator()(T b) const
        {
                return b ^ (b - T(1));
        }
};

template<typename T>
struct clear
{
        void operator()(T& b) const
        {
                b &= b - T(1);
        }
};

// TODO: partial specializations for bit arrays

}       // namespace detail

// least significant 1-bit
template<typename T>
T equal(T b)
{
        return detail::equal<T>()(b);
}

// most significant 1-bits
template<typename T>
T not_equal_to(T b)
{
        return detail::not_equal_to<T>()(b);
}

// clear the least significant 1-bit
template<typename T>
void clear(T& b)
{
        detail::clear<T>()(b);
}

}       // namespace first

namespace range {

// [i, i+1)
template<typename T>
T equal(int i)
{
        return singlet<T>(i);
}

// [0, ..., i-1, i+1, ... N)
template<typename T>
T not_equal_to(int i)
{
        return ~equal<T>(i);
}

// [i+1, N)
template<typename T>
T greater(int i)
{
        return ~less_equal<T>(i);
}

// [0, i)
template<typename T>
T less(int i)
{
        return equal<T>(i) - 1;
}

// [i, N)
template<typename T>
T greater_equal(int i)
{
        return ~less<T>(i);
}

// [0, i] == [0, i+1)
template<typename T>
T less_equal(int i)
{
        return less<T>(i + 1);
}

}       // namespace range

// number of set 1-bits
template<typename T>
int count(T b)
{
        return intrinsic::count(b);
}

// index of the least significant 1-bit
template<typename T>
int find(T b)
{
        return intrinsic::find(b);
}

// 0 bits set to 1
template<typename T>
bool is_zero(T b)
{
        return b == zero<T>();
}

// 1 bit set to 1
template<typename T>
bool is_single(T b)
{
        return !(is_zero(b) || is_multiple(b));
}

// 2 bits set to 1
template<typename T>
bool is_double(T b)
{
        return is_single(first::not_equal_to(b));
}

// 2 or more bits set to 1
template<typename T>
bool is_multiple(T b)
{
        return !is_zero(first::not_equal_to(b));
}

template<typename Iterator, typename Board>
bool is_element(Iterator it, Board b)
{
        return !is_zero(it & b);
}

}       // namespace bit
}       // namespace dctl
