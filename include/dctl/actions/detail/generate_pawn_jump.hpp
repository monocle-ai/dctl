#pragma once
#include <dctl/actions/detail/raii.hpp>                 // Launch, Capture, Visit, Toggleking_targets, Setpromotion
#include <dctl/actions/detail/builder.hpp>              // Builder
#include <dctl/actions/detail/generate_primary_fwd.hpp> // Generate (primary template)
#include <dctl/actions/detail/generate_king_jump.hpp>   // promote_en_passant
#include <dctl/actions/select/jump.hpp>                 // jumps
#include <dctl/board/angle.hpp>                         // _deg, rotate, inverse
#include <dctl/board/bearing.hpp>                       // bearing
#include <dctl/board/ray.hpp>                           // make_iterator, rotate, mirror, turn
#include <dctl/color.hpp>                               // Color
#include <dctl/mask/jump_sources.hpp>                   // jump_sources
#include <dctl/mask/promotion.hpp>                      // is_promotion
#include <dctl/piece.hpp>                               // king_type, pawn_type
#include <dctl/rule_traits.hpp>                         // is_superior_rank_jump_t, is_backward_pawn_jump, is_orthogonal_jump_t, is_promotion_en_passant_t
#include <dctl/utility/type_traits.hpp>                 // action_t, board_t, rules_t, set_t
#include <cassert>                                      // assert
#include <iterator>                                     // prev
#include <type_traits>                                  // is_same

namespace dctl {
namespace detail {

template<class Color, class Reverse, class State, class Builder>
class Generate<Color, pawn_type, select::jump, Reverse, State, Builder>
{
        using  king_jumps = Generate<Color, king_type, select::jump, Reverse, State, Builder>;
        using action_type = action_t<Builder>;
        using  board_type =  board_t<Builder>;
        using  rules_type =  rules_t<Builder>;
        using    set_type =    set_t<Builder>;

        template<int Direction>
        using jump_sources = mask::jump_sources<board_type, Direction, short_ranged_tag>;

        static constexpr auto orientation = bearing_v<board_type, Color, Reverse>.degrees;

        template<class Iterator>
        static constexpr auto direction_v = rotate(board::ray::direction_v<Iterator>, inverse(angle{orientation}));

        Builder& builder;
public:
        explicit Generate(Builder& b) noexcept
        :
                builder{b}
        {}

        auto operator()() const
        {
                if (builder.active_pawns().none()) {
                        return;
                }
                if constexpr (is_superior_rank_jump_or_v<rules_type>) {
                        raii::toggle_king_targets<Builder> guard{builder};
                        directions();
                } else {
                        directions();
                }
        }
private:
        auto directions() const
        {
                if constexpr (
                        std::is_same<pawn_jump_category_t<rules_type>, forward_pawn_jump_tag>{} &&
                        std::is_same<     jump_category_t<rules_type>,     diagonal_jump_tag>{}
                ) {
                        return directions_lfold<right_up, left_up>();
                }
                if constexpr (
                        std::is_same<pawn_jump_category_t<rules_type>, backward_pawn_jump_tag>{} &&
                        std::is_same<     jump_category_t<rules_type>,      diagonal_jump_tag>{}
                ) {
                        return directions_lfold<right_up, left_up, left_down, right_down>();
                }
                if constexpr (
                        std::is_same<pawn_jump_category_t<rules_type>, forward_pawn_jump_tag>{} &&
                        std::is_same<     jump_category_t<rules_type>,   orthogonal_jump_tag>{}
                ) {
                        return directions_lfold<right, right_up, up, left_up, left>();
                }
                if constexpr (
                        std::is_same<pawn_jump_category_t<rules_type>, backward_pawn_jump_tag>{} &&
                        std::is_same<     jump_category_t<rules_type>,    orthogonal_jump_tag>{}
                ) {
                        return directions_lfold<right, right_up, up, left_up, left, left_down, down, right_down>();
                }
        }

        template<template<int> class... Directions>
        auto directions_lfold() const
        {
                (... , sources<Directions<orientation>{}>());
        }

        template<int Direction>
        auto sources() const
        {
                jump_sources<Direction>{}(
                        builder.active_pawns(),
                        builder.current_targets(),
                        builder.not_occupied()
                ).for_each([this](auto const from_sq){
                        this->jump(this->along_ray<Direction>(from_sq));
                });
        }

        template<class Iterator>
        auto jump(Iterator const jumper) const
        {
                assert(is_onboard(jumper));
                raii::launch<Builder> guard{builder, *jumper};
                capture(std::next(jumper));
        }

        template<class Iterator>
        auto capture(Iterator const jumper) const
                -> void
        {
                assert(is_onboard(jumper));
                raii::capture<Builder> guard{builder, *jumper};
                land(std::next(jumper));
        }

        template<class Iterator>
        auto land(Iterator jumper) const
        {
                assert(is_onboard(jumper));
                try_promotion(jumper);
        }

        template<class Iterator>
        auto try_promotion(Iterator jumper) const
        {
                if constexpr (is_passing_promotion_or_v<rules_type>) {
                        if (is_promotion(*jumper))
                                return on_promotion(jumper);
                        return try_next(jumper);
                } else {
                        if (next_target(jumper))
                                return;
                        if (is_promotion(*jumper))
                                return on_promotion(jumper);
                        return add_jump(*jumper);
                }
        }

        template<class Iterator>
        auto on_promotion(Iterator jumper) const
        {
                raii::promotion<Builder> guard{builder};
                if constexpr (is_passing_promotion_or_v<rules_type>) {
                        return on_king_jump(jumper);
                } else {
                        return add_jump(*jumper);
                }
        }

        template<class Iterator>
        auto on_king_jump(Iterator jumper) const
        {
                static_assert(is_passing_promotion_or_v<rules_type>);
                if constexpr (is_superior_rank_jump_or_v<rules_type>) {
                        raii::toggle_king_targets<Builder> guard{builder};
                        king_jumps_try_next(jumper);
                } else {
                        king_jumps_try_next(jumper);
                }
        }

        template<class Iterator>
        auto king_jumps_try_next(Iterator jumper) const
        {
                static_assert(is_passing_promotion_or_v<rules_type>);
                king_jumps{builder}.try_next(jumper, passing_promotion_tag{});
        }

        template<class Iterator>
        auto try_next(Iterator jumper) const
        {
                if (!next_target(jumper))
                        add_jump(*jumper);
        }

        template<class Iterator>
        auto next_target(Iterator jumper) const
        {
                //raii::Visit<Builder> guard{builder, *jumper};
                return scan(jumper) | turn(jumper);
        }

        template<class Iterator>
        auto turn(Iterator jumper) const
        {
                if constexpr (
                        std::is_same<pawn_jump_category_t<rules_type>, forward_pawn_jump_tag>{} &&
                        std::is_same<     jump_category_t<rules_type>,     diagonal_jump_tag>{}
                ) {
                        static_assert(is_up(direction_v<Iterator>) && is_diagonal(direction_v<Iterator>));
                        return scan(board::ray::mirror<up<orientation>{}>(jumper));
                }
                if constexpr (
                        std::is_same<pawn_jump_category_t<rules_type>, backward_pawn_jump_tag>{} &&
                        std::is_same<     jump_category_t<rules_type>,      diagonal_jump_tag>{}
                ) {
                        static_assert(is_diagonal(direction_v<Iterator>));
                        return rotate_directions_lfold<-90, +90>(jumper);
                }
                if constexpr (
                        std::is_same<pawn_jump_category_t<rules_type>, forward_pawn_jump_tag>{} &&
                        std::is_same<     jump_category_t<rules_type>,   orthogonal_jump_tag>{}
                ) {
                        static_assert(!is_down(direction_v<Iterator>));

                        if constexpr (direction_v<Iterator> == right<orientation>{}) {
                                return turn_directions_lfold<right_up, up, left_up>(jumper);
                        }
                        if constexpr (direction_v<Iterator> == right_up<orientation>{}) {
                                return turn_directions_lfold<right, up, left_up, left>(jumper);
                        }
                        if constexpr (direction_v<Iterator> == up<orientation>{}) {
                                return turn_directions_lfold<right, right_up, left_up, left>(jumper);
                        }
                        if constexpr (direction_v<Iterator> == left_up<orientation>{}) {
                                return turn_directions_lfold<right, right_up, up, left>(jumper);
                        }
                        if constexpr (direction_v<Iterator> == left<orientation>{}) {
                                return turn_directions_lfold<right_up, up, left_up>(jumper);
                        }
                }
                if constexpr (
                        std::is_same<pawn_jump_category_t<rules_type>, backward_pawn_jump_tag>{} &&
                        std::is_same<     jump_category_t<rules_type>,    orthogonal_jump_tag>{}
                ) {
                        return rotate_directions_lfold<-135, -90, -45, +45, +90, +135>(jumper);
                }
        }

        template<template<int> class... Directions, class Iterator>
        auto turn_directions_lfold(Iterator jumper) const
        {
                return (... | scan(board::ray::turn<Directions<orientation>{}>(jumper)));
        }

        template<int... Directions, class Iterator>
        auto rotate_directions_lfold(Iterator jumper) const
        {
                return (... | scan(board::ray::rotate<Directions>(jumper)));
        }

        template<class Iterator>
        auto scan(Iterator jumper) const
        {
                return is_en_prise(std::next(jumper));
        }

        template<class Iterator>
        auto is_en_prise(Iterator jumper) const
        {
                if (!(is_onboard(jumper) && builder.current_targets(jumper)))
                        return false;

                assert(is_onboard(std::next(jumper)));
                capture(jumper);
                return true;
        }

        auto add_jump(std::size_t const dest_sq) const
        {
                builder.finalize(dest_sq);
        }

        template<int Direction>
        auto along_ray(std::size_t const sq) const noexcept
        {
                return board::ray::make_iterator<board_type, Direction>(sq);
        }

        auto is_promotion(std::size_t const sq) const // Throws: Nothing.
        {
                return mask::promotion_v<board_type, Color>.test(sq);
        }
};

}       // namespace detail
}       // namespace dctl
