#pragma once

//          Copyright Rein Halbersma 2010-2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <experimental/array>   // make_array

namespace dctl::core {
namespace meta {

template<class L>
struct transformed_array_from_type_list;

template<template<class...> class L, class... T>
struct transformed_array_from_type_list<L<T...>>
{
        template<class UnaryFunction>
        constexpr auto operator()(UnaryFunction fun) const
        {
                return std::experimental::make_array(fun(T{})...);
        }
};

}       // namespace meta
}       // namespace dctl::core
