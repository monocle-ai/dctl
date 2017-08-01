#pragma once

//          Copyright Rein Halbersma 2010-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <dctl/core/actions/detail/generate_primary_fwd.hpp>    // generate (primary template)
#include <dctl/core/actions/select/push.hpp>                    // select
#include <dctl/core/board/angle.hpp>                            // left_up, right_up
#include <dctl/core/board/bearing.hpp>                          // bearing
#include <dctl/core/board/push_targets.hpp>                     // push_targets
#include <dctl/core/board/ray.hpp>                              // make_iterator
#include <dctl/core/state/color_piece.hpp>                      // color, color_, pawn_
#include <dctl/util/meta.hpp>                                   // comma, map_reduce
#include <dctl/util/type_traits.hpp>                            // board_t, set_t, value_t
#include <iterator>                                             // prev

namespace dctl::core {
namespace detail {

template<color Side, class Reverse, class State>
class generate<color_<Side>, pawns_, select::push, Reverse, State>
{
        using board_type = board_t<State>;
        constexpr static auto orientation = bearing_v<board_type, color_<Side>, Reverse>;
        using pawn_push_directions = std::tuple<right_up<orientation>, left_up<orientation>>;
public:
        template<class SequenceContainer>
        auto operator()(State const& s, SequenceContainer& seq) const
        {
                if (auto const pawns = s.pieces(color_c<Side>, pawns_c); !pawns.empty()) {
                        meta::map_reduce<pawn_push_directions, meta::comma>{}([&](auto direction) {
                                constexpr auto Direction = decltype(direction){};
                                push_targets<board_type, Direction, short_ranged_tag>{}(pawns, s.pieces(empty_c)).consume([&](auto const dest_sq) {
                                        seq.emplace_back(
                                                *std::prev(ray::make_iterator<board_type, Direction>(dest_sq)),
                                                dest_sq,
                                                board_type::promotion(Side).contains(dest_sq)
                                        );
                                });
                        });
                }
        }
};

}       // namespace detail
}       // namespace dctl::core
