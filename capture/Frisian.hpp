#pragma once
#include <limits>                       // numeric_limits
#include <boost/assert.hpp>             // BOOST_ASSERT
#include <boost/operators.hpp>          // totally_ordered
#include "Value_fwd.hpp"                // Value (primary template)

namespace dctl {

namespace rules { struct Frisian; }

namespace capture {

// explicit specialization for Frisian draughts
template<>
struct Value<rules::Frisian>
:
        // Curiously Recurring Template Pattern (CRTP)
        private boost::totally_ordered< Value<rules::Frisian> >
{
public:
        // structors

        Value()
        :
                num_pieces_(0),
                num_kings_(0),
                with_king_(false)
        {
                BOOST_ASSERT(invariant());
        }

        // modifiers

        void increment(bool is_captured_king)
        {
                num_kings_ += is_captured_king;
                ++num_pieces_;
                BOOST_ASSERT(invariant());
        }

        void decrement(bool is_captured_king)
        {
                BOOST_ASSERT(!empty(is_captured_king));
                --num_pieces_;
                num_kings_ -= is_captured_king;
                BOOST_ASSERT(invariant());
        }

        void toggle_with_king()
        {
                with_king_ ^= true;
                BOOST_ASSERT(invariant());
        }

        // queries

        int count() const
        {
                return num_pieces_;
        }

        // predicates

        bool operator<(Value const& other) const
        {
                auto const delta_kings = num_kings_ - other.num_kings_;
                auto const delta_pawns = num_pieces_ - other.num_pieces_ - delta_kings;

                // Art. 11
                if (delta_kings * delta_pawns < 0) {
                        // delta_kings and delta_pawns are both non-zero and have opposite sign
                        // [2 n - 1] pawns < [n] kings < [2 n] pawns
                        return delta_pawns + 2 * delta_kings - (delta_kings > 0) <  0;
                } else {
                        // delta_kings or delta_pawns is zero or they have equal sign
                        return (
                                ( num_pieces_ < other.num_pieces_ ) || (( num_pieces_ == other.num_pieces_ ) &&
                                ( with_king_  < other.with_king_  ))
                        );
                }
        }

        bool operator==(Value const& other) const
        {
                return (
                        ( num_pieces_ == other.num_pieces_ ) &&
                        ( num_kings_  == other.num_kings_  ) &&
                        ( with_king_  == other.with_king_  )
                );
        }

private:
        bool invariant() const
        {
                return (
                        (0 <= num_kings_) && 
                        (num_kings_ <= num_pieces_) && 
                        (num_pieces_ < std::numeric_limits<int>::max())
                );
        }

        bool empty(bool is_captured_king) const
        {
                return (is_captured_king? num_kings_ : num_pieces_) == 0; 
        }

        // representation

        int num_pieces_;
        int num_kings_;
        bool with_king_;
};

}       // namespace capture
}       // namespace dctl
