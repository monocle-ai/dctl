#pragma once
#include <dctl/rules/thai_fwd.hpp>      // Thai
#include <dctl/successor/value.hpp>     // Value (primary template)

namespace dctl {
namespace rules {

struct Thai
{
        // required
        static constexpr auto is_backward_pawn_jump = false;
        static constexpr auto is_jump_precedence = false;
        static constexpr auto is_long_ranged_king = true;

        // optional
        static constexpr auto initial_gap = 4;
        static constexpr auto is_directly_land_after_piece = true;
        static constexpr auto is_en_passant_jump_removal = true;
};

}       // namespace rules
}       // namespace dctl
