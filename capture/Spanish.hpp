#pragma once
#include <boost/assert.hpp>             // BOOST_ASSERT
#include <boost/operators.hpp>          // totally_ordered
#include "Value_fwd.hpp"

namespace dctl {

namespace variant { struct Spanish; }

namespace capture {

// explicit specialization for Spanish draughts
template<>
struct Value<variant::Spanish>
:
        // Curiously Recurring Template Pattern (CRTP)
        private boost::totally_ordered< Value<variant::Spanish> >
{
public:
        // structors

        Value()
        :
                num_pieces_(0),
                num_kings_(0)
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
                --num_pieces_;
                num_kings_ -= is_captured_king;
                BOOST_ASSERT(invariant());
        }

        // queries

        int count() const
        {
                return num_pieces_;
        }

        // predicates

        bool operator<(const Value& other) const
        {
                return (
                        ( num_pieces_ < other.num_pieces_ ) || (( num_pieces_ == other.num_pieces_ ) &&
                        ( num_kings_  < other.num_kings_  ))
                );
        }

        bool operator==(const Value& other) const
        {
                return (
                        ( num_pieces_ == other.num_pieces_ ) &&
                        ( num_kings_  == other.num_kings_  )
                );
        }

private:
        bool invariant() const
        {
                return (0 <= num_kings_) && (num_kings_ <= num_pieces_);
        }

        // representation

        int num_pieces_;
        int num_kings_;
};

}       // namespace capture
}       // namespace dctl
