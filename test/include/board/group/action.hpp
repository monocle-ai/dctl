#pragma once
#include <board/group/primitives.hpp>   // set, op, id
#include <algorithm>                    // all_of

namespace dctl {
namespace group {
namespace action {

template<class Object, class Group>
auto is_associativity(Object const& obj, Group const& g) noexcept
{
        auto const set = group::set(g);
        auto const op = group::op(g);

        return std::all_of(set.cbegin(), set.cend(), [&](auto const& a) {
                return std::all_of(set.cbegin(), set.cend(), [&](auto const& b){
                        return op(obj, op(a, b)) == op(op(obj, a), b);
                });
        });
}

template<class Object, class Group>
auto is_identity(Object const& obj, Group const& g) noexcept
{
        auto const op = group::op(g);
        auto const id = group::id(g);

        return op(obj, id) == obj;
}

template<class Object, class Group>
auto is_realized(Object const& obj, Group const& g) noexcept
{
        return
                is_associativity(obj, g) &&
                is_identity(obj, g)
        ;
}

}       // namespace action
}       // namespace group
}       // namespace dctl
