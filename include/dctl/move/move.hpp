#pragma once
#include <dctl/color.hpp>
#include <dctl/rules.hpp>
#include <dctl/set_type.hpp>    // set_type
#include <dctl/type_traits.hpp> // board_type_t, rules_type_t
#include <cassert>              // assert
#include <tuple>                // forward_as_tuple

namespace dctl {

template<class Rules, class Board>
class BaseMove
{
public:
        using board_type = Board;
        using rules_type = Rules;
        using Set = set_type<Board>;

private:
        Set captured_pieces_{};
        Set captured_kings_{};
        std::size_t from_{};
        std::size_t dest_{};
        bool is_with_king_{};
        bool is_promotion_{};
        Color to_move_{};

        auto invariant() const
        {
                return
                        !(from() == dest() && !is_jump()) &&
                        captured_kings_.is_subset_of(captured_pieces_) &&
                        !(is_with_king_ && is_promotion_)
                ;
        }

public:
        // constructors

        // king move
        constexpr BaseMove(std::size_t src, std::size_t dst, Color c)
        :
                from_{src},
                dest_{dst},
                is_with_king_{true},
                to_move_{c}
        {
                assert(invariant());
        }

        // pawn move
        constexpr BaseMove(std::size_t src, std::size_t dst, bool prom, Color c)
        :
                from_{src},
                dest_{dst},
                is_promotion_{prom},
                to_move_{c}
        {
                assert(invariant());
        }

        // any jump
        template<class Tracker>
        explicit constexpr BaseMove(Tracker const& t)
        :
                captured_pieces_{t.captured_pieces()},
                captured_kings_{t.captured_kings()},
                from_{t.from_sq()},
                dest_{t.dest_sq()},
                is_with_king_{t.is_with_king()},
                is_promotion_{t.is_promotion()},
                to_move_{t.to_move()}
        {
                assert(invariant());
        }

        // observers

        constexpr auto captured_pieces() const noexcept
        {
                return captured_pieces_;
        }

        constexpr auto captured_kings() const noexcept
        {
                return captured_kings_;
        }

        constexpr auto from() const noexcept
        {
                return from_;
        }

        constexpr auto dest() const noexcept
        {
                return dest_;
        }

        constexpr auto is_with_king() const noexcept
        {
                return is_with_king_;
        }

        constexpr auto is_promotion() const noexcept
        {
                return is_promotion_;
        }

        constexpr auto to_move() const noexcept
        {
                return to_move_;
        }

        constexpr auto is_jump() const noexcept
        {
                return captured_pieces().any();
        }

        constexpr auto is_reversible() const noexcept
        {
                return is_with_king() && !is_jump();
        }

        constexpr auto num_pieces() const noexcept
        {
                return captured_pieces_.count();
        }

        constexpr auto num_kings() const noexcept
        {
                return captured_kings_.count();
        }

        // predicates

        friend constexpr auto
        operator==(BaseMove const& lhs, BaseMove const& rhs) noexcept
        {
                return
                        std::tie(lhs.from_, lhs.dest_, lhs.captured_pieces_) ==
                        std::tie(rhs.from_, rhs.dest_, rhs.captured_pieces_)
                ;
        }

        friend constexpr auto
        operator!=(BaseMove const& lhs, BaseMove const& rhs) noexcept
        {
                return !(lhs == rhs);
        }

        friend constexpr auto
        operator<(BaseMove const& lhs, BaseMove const& rhs) noexcept
        {
                return
                        std::tie(lhs.from_, lhs.dest_, lhs.captured_pieces_) <
                        std::tie(rhs.from_, rhs.dest_, rhs.captured_pieces_)
                ;
        }
};

template<class Rules, class Board>
class EmptyBase {};

template<class Rules, class Board>
class Move
:
        public BaseMove<Rules, Board>,
        public EmptyBase<Rules, Board>
{
public:
        using base = BaseMove<Rules, Board>;
        using base::base;
};

template<class Board>
class EmptyBase<italian::Rules, Board>
{
        set_type<Board> king_order_{};
public:
        EmptyBase() = default;
        explicit EmptyBase(set_type<Board> ko): king_order_(ko) {}

        template<class Tracker>
        explicit EmptyBase(Tracker const& t)
        :
                king_order_(t.king_order())
        {}

        set_type<Board> king_order() const { return king_order_; }
};

template<class Board>
class Move<italian::Rules, Board>
:
        public EmptyBase<italian::Rules, Board>,
        public BaseMove<italian::Rules, Board>
{
public:
        using empty = EmptyBase<italian::Rules, Board>;
        using base = BaseMove<italian::Rules, Board>;
        using base::base;
        using Set = typename base::Set;

        // king jump
        constexpr Move(Set pieces, Set kings, int src, int dst, Color c, Set ko)
        :
                empty(ko),
                base(pieces, kings, src, dst, c, ko)
        {}

        // pawn jump
        constexpr Move(Set pieces, Set kings, int src, int dst, bool prom, Color c, Set ko)
        :
                empty(ko),
                base(pieces, kings, src, dst, prom, c, ko)
        {}

        template<class Tracker>
        explicit constexpr Move(Tracker const& t)
        :
                empty(t),
                base(t)
        {}
};

template<class T>
using Move_t = Move<rules_type_t<T>, board_type_t<T>>;

}       // namespace dctl
