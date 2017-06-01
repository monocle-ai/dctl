#pragma once
#include <dctl/core/board/mask/detail/iterator.hpp>  // make_iterator
#include <dctl/core/rule_traits.hpp>                 // short_ranged_tag
#include <iterator>                             // prev

namespace dctl {
namespace board {
namespace mask {

template<class Board, int Direction, class KingRangeCategory>
class jump_sources;

template<class Board, int Direction>
class jump_sources<Board, Direction, short_ranged_tag>
{
        template<class Set>
        auto prev_set(Set const s) const
        {
                return Set(*std::prev(detail::make_iterator<Board, Direction>(s)));
        }

public:
        template<class Set>
        auto operator()(Set const active_pieces, Set const passive_pieces, Set const not_occupied) const
        {
                return active_pieces & prev_set(passive_pieces & prev_set(not_occupied));
        }
};

}       // namespace mask
}       // namespace board
}       // namespace dctl