#pragma once
#include "Rules.hpp"
#include "Enum.hpp"

namespace dctl {
namespace rules {

// http://www.mindsports.nl/index.php/arena/draughts/382-killer-draughts-rules
struct Killer
: 
        Rules< 
                king_range<range::distance_N>,
                pawn_jump_directions<directions::diag>,
                jump_precedence<precedence::quantity>,
                halt_range<range::distance_1K>
        > 
{};

}       // namespace rules
}       // namespace dctl

#include "../capture/Killer.hpp"
