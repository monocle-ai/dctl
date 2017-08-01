#pragma once

//          Copyright Rein Halbersma 2010-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <dctl/core/actions/detail/detect_primary_fwd.hpp>      // detect (primary template)
#include <dctl/core/actions/select/push.hpp>                    // push
#include <dctl/core/board/angle.hpp>                            // left_up, right_up, left_down, right_down
#include <dctl/core/board/bearing.hpp>                          // bearing
#include <dctl/core/board/push_targets.hpp>                     // push_targets
#include <dctl/core/state/color_piece.hpp>                      // color, color_, king_
#include <dctl/util/meta.hpp>                                   // logical_or, map_reduce
#include <dctl/util/type_traits.hpp>                            // board_t, set_t

namespace dctl::core {
namespace detail {

template<color Side, class Reverse, class State>
class detect<color_<Side>, kings_, select::push, Reverse, State>
{
        using board_type = board_t<State>;
        constexpr static auto orientation = bearing_v<board_type, color_<Side>, Reverse>;
        using king_push_directions = std::tuple<right_up<orientation>, left_up<orientation>, left_down<orientation>, right_down<orientation>>;
public:
        auto operator()(State const& s) const noexcept
        {
                if (auto const kings = s.pieces(color_c<Side>, kings_c); !kings.empty()) {
                        return meta::map_reduce<king_push_directions, meta::logical_or>{}([&](auto direction) {
                                constexpr auto Direction = decltype(direction){};
                                return !push_targets<board_type, Direction, short_ranged_tag>{}(kings, s.pieces(empty_c)).empty();
                        });
                }
                return false;
        }
};

}       // namespace detail
}       // namespace dctl::core
