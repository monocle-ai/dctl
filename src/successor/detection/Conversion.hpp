#pragma once
#include <functional>                   // function
#include "Detector_fwd.hpp"
#include "Primary.hpp"
#include "BothJumps.hpp"
#include "BothPromotions.hpp"
#include "KingJumps.hpp"
// there are no king promotions
#include "PawnJumps.hpp"
#include "PawnPromotions.hpp"
#include "../Select.hpp"

namespace dctl {
namespace successor {
namespace detail {

// partial specialization for legal successors detection
template<bool Color, int Material, typename Position, typename Range>
struct detector<Color, Material, Conversion, Position, Range>
:
        public std::function<bool(Position const&)>
{
private:
        // typedefs

        // only pawns can promote and they always have rules::range::distance_1
        typedef detector<Color, Material, Promotions, Position, rules::range::distance_1> DoPromotions;

        typedef detector<Color, Material, Jumps, Position, Range> DoJumps;

public:
        bool operator()(Position const& p) const
        {
                // speculate #promotions > #jumps, so that the || is likely to short-circuit
                return (
                        DoPromotions()(p) ||
                        DoJumps()(p)
                );
        }
};

}       // namespace detail
}       // namespace successor
}       // namespace dctl
