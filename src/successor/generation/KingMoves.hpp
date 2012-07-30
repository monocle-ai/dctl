#pragma once
#include <boost/config.hpp>             // BOOST_MPL_ASSERT
#include <boost/mpl/bool_fwd.hpp>       // false_, true_
#include "Generator_fwd.hpp"
#include "../Select.hpp"
#include "../../bit/Bit.hpp"
#include "../../board/Compass.hpp"
#include "../../board/Shift.hpp"
#include "../../node/Material.hpp"
#include "../../node/Stack.hpp"
#include "../../rules/Enum.hpp"
#include "../../utility/IntegerTypes.hpp"
#include "../../utility/nonconstructible.hpp"

namespace dctl {
namespace successor {
namespace detail {

// partial specialization for king moves generation
template<bool Color, typename Position>
struct generator<Color, Material::king, select::Moves, Position>
:
        // enforce static semantics
        private nonconstructible
{
private:
        // typedefs

        typedef typename Position::rules_type Rules;
        typedef typename Position::board_type Board;
        typedef angle::Compass<Color, Board> Compass;

public:
        static void run(Position const& p, Stack& moves)
        {
                if (auto const active_kings = unrestricted_kings(p, Color)) 
                        select(active_kings, not_occupied(p), moves);
        }

private:
        static void select(BitBoard active_kings, BitBoard not_occupied, Stack& moves)
        {
                serialize(active_kings, not_occupied, moves);
        }

        static void serialize(BitBoard active_kings, BitBoard not_occupied, Stack& moves)
        {
                BOOST_ASSERT(!bit::is_zero(active_kings));
                do {
                        branch(bit::get_first(active_kings), not_occupied, moves);
                        bit::clear_first(active_kings);
                } while (active_kings);
        }

        static void branch(BitIndex from_sq, BitBoard not_occupied, Stack& moves)
        {
                find<typename Compass::left_down >(from_sq, not_occupied, moves);
                find<typename Compass::right_down>(from_sq, not_occupied, moves);
                find<typename Compass::left_up   >(from_sq, not_occupied, moves);
                find<typename Compass::right_up  >(from_sq, not_occupied, moves);
        }

        template<typename Direction>
        static void find(BitIndex from_sq, BitBoard not_occupied, Stack& moves)
        {
                // tag dispatching on king range
                return find_dispatch<Direction>(from_sq, not_occupied, moves, typename Rules::king_range());
        }

        // overload for short ranged kings
        template<typename Direction>
        static void find_dispatch(BitIndex from_sq, BitBoard not_occupied, Stack& moves, rules::range::distance_1)
        {
                if (auto const dest_sq = Board::next<Direction>(from_sq) & not_occupied)
                        moves.push_back(Move::create<Color>(from_sq ^ dest_sq));
        }

        // overload for long ranged kings
        template<typename Direction>
        static void find_dispatch(BitIndex from_sq, BitBoard not_occupied, Stack& moves, rules::range::distance_N)
        {
                for (
                        auto dest_sq = Board::next<Direction>(from_sq);
                        bit::is_element(dest_sq, not_occupied);
                        Board::advance<Direction>(dest_sq)
                )
                        moves.push_back(Move::create<Color>(from_sq ^ dest_sq));
        }
};

}       // namespace detail
}       // namespace successor
}       // namespace dctl
