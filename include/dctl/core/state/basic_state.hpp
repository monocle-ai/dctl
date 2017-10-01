#pragma once

//          Copyright Rein Halbersma 2010-2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <dctl/core/rules/type_traits.hpp>
#include <dctl/core/state/color_piece.hpp>
#include <dctl/core/state/position.hpp>
#include <dctl/util/conditional_base.hpp>       // conditional_base
#include <dctl/util/type_traits.hpp>            // set_t
#include <hash_append/hash_append.h>            // hash_append
#include <cassert>                              // assert
#include <cstdint>                              // uint64_t
#include <type_traits>                          // is_pod, is_same
#include <tuple>                                // tie
#include <utility>                              // forward

namespace dctl::core {
namespace detail {
namespace block_adl {

template<class Board>
struct base_position
{
        using position_type = bwk::position<Board>;
        position_type m_position;
};

struct base_color
{
        color m_color;
};

template<class Board>
struct most_recently_pushed_kings {};

template<class Rules, class Board>
using conditional_base_mrpk = util::conditional_base<
        is_restricted_king_move_v<Rules>,
        most_recently_pushed_kings<Board>
>;

}       // namespace block_adl

using block_adl::base_position;
using block_adl::base_color;
using block_adl::conditional_base_mrpk;

}       // namespace detail

template<class Rules, class Board>
class basic_state
:
        detail::base_position<Board>,
        detail::conditional_base_mrpk<Rules, Board>,
        detail::base_color
{
        using base_position = detail::base_position<Board>;
        using conditional_base_mrpk = detail::conditional_base_mrpk<Rules, Board>;
        using base_color = detail::base_color;
public:
        using    rules_type = Rules;
        using    board_type = Board;
        using      set_type = set_t<Board>;
        using position_type = position_t<base_position>;

private:
        constexpr auto assert_invariants() const noexcept
        {
                assert(true);
        }

public:
        basic_state(position_type const& position, color const to_move)
        :
                base_position{position},
                conditional_base_mrpk{},
                base_color{to_move}
        {
                assert_invariants();
        }

        constexpr static auto initial()
                -> basic_state
        {
                constexpr auto separation = initial_position_gap_v<Rules> + Board::height % 2;
                PP_CONSTEXPR_CONST auto black_pawns = board_type::initial(black_c, separation);
                PP_CONSTEXPR_CONST auto white_pawns = board_type::initial(white_c, separation);
                return {{black_pawns, white_pawns}, color::white};
        }

        constexpr static auto initial(int const separation)
                -> basic_state
        {
                auto const black_pawns = board_type::initial(black_c, separation);
                auto const white_pawns = board_type::initial(white_c, separation);
                return {{black_pawns, white_pawns}, color::white};
        }

        template<class Action>
        auto make(Action const& a)
        {
                static_assert(std::is_same_v<rules_type, rules_t<Action>>);
                static_assert(std::is_same_v<board_type, board_t<Action>>);
                this->m_position.make(to_move(), a);
                pass_turn();
                assert_invariants();
        }

        auto make(nullmove_t)
        {
                pass_turn();
                assert_invariants();
        }

        template<class Action>
        auto undo(Action const& a)
        {
                static_assert(std::is_same_v<rules_type, rules_t<Action>>);
                static_assert(std::is_same_v<board_type, board_t<Action>>);
                pass_turn();
                this->m_position.undo(to_move(), a);
                assert_invariants();
        }

        auto undo(nullmove_t)
        {
                pass_turn();
                assert_invariants();
        }

        template<class... Args>
        auto pieces(Args&&... args) const noexcept
        {
                static_assert(sizeof...(Args) <= 2);
                return this->m_position.pieces(std::forward<Args>(args)...);
        }

        template<color Side, piece Type>
        auto targets(color_<Side>, piece_<Type>) const noexcept
        {
                if constexpr (Type == piece::pawns && is_superior_rank_jump_v<rules_type>) {
                        return pieces(color_c<!Side>, pawns_c);
                } else {
                        return pieces(!color_c<Side>);
                }
        }

        template<class... Args>
        auto num_pieces(Args&&... args) const noexcept
        {
                static_assert(sizeof...(Args) <= 2);
                return this->m_position.num_pieces(std::forward<Args>(args)...);
        }

        auto to_move() const noexcept
        {
                return m_color;
        }

        auto tied() const
        {
                return std::tie(this->m_position, m_color);
        }

        template<class HashAlgorithm>
        friend auto hash_append(HashAlgorithm& h, basic_state const& s)
        {
                using xstd::hash_append;
                hash_append(h, s.m_position, s.m_color);
        }

private:
        constexpr auto pass_turn() noexcept
        {
                m_color = !m_color;
        }
};

template<class Rules, class Board>
constexpr auto operator==(basic_state<Rules, Board> const& lhs, basic_state<Rules, Board> const& rhs) noexcept
{
        return lhs.tied() == rhs.tied();
}

template<class Rules, class Board>
constexpr auto operator< (basic_state<Rules, Board> const& lhs, basic_state<Rules, Board> const& rhs) noexcept
{
        return lhs.tied() < rhs.tied();
}

template<class Rules, class Board>
constexpr auto operator!=(basic_state<Rules, Board> const& lhs, basic_state<Rules, Board> const& rhs) noexcept
{
        return !(lhs == rhs);
}

template<class Rules, class Board>
constexpr auto operator> (basic_state<Rules, Board> const& lhs, basic_state<Rules, Board> const& rhs) noexcept
{
        return rhs < lhs;
}

template<class Rules, class Board>
constexpr auto operator>=(basic_state<Rules, Board> const& lhs, basic_state<Rules, Board> const& rhs) noexcept
{
        return !(lhs < rhs);
}

template<class Rules, class Board>
constexpr auto operator<=(basic_state<Rules, Board> const& lhs, basic_state<Rules, Board> const& rhs) noexcept
{
        return !(rhs < lhs);
}

}       // namespace dctl::core