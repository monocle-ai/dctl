#pragma once
#include <dctl/actions/detail/raii.hpp>                 // ToggleKingTargets
#include <dctl/actions/detail/tracker.hpp>              // Tracker
#include <dctl/actions/detect/detail/primary_fwd.hpp>   // Detect (primary template)
#include <dctl/actions/select/jump.hpp>                 // jump
#include <dctl/board/angle.hpp>                         // up, left_up, right_up, left, right, left_down, right_down, down
#include <dctl/board/orientation.hpp>                   // orientation_v
#include <dctl/board/wave/patterns.hpp>                 // Sandwich
#include <dctl/color.hpp>                               // Color
#include <dctl/piece.hpp>                               // pawn
#include <dctl/rule_traits.hpp>                         // is_backward_pawn_jump, is_orthogonal_jump, is_pawn_jump_king
#include <dctl/utility/type_traits.hpp>                 // board_t, rules_t, set_t

namespace dctl {
namespace actions {
namespace detail {

template<Color ToMove, class Reverse, class Tracker>
class Detect<ToMove, Piece::pawn, select::jump, Reverse, Tracker>
{
        using   board_type = board_t<Tracker>;
        using   rules_type = rules_t<Tracker>;
        using     set_type =   set_t<Tracker>;

        static constexpr auto orientation = orientation_v<board_type, ToMove, Reverse::value>;
        Tracker& tracker;

public:
        explicit Detect(Tracker& t)
        :
                tracker{t}
        {}

        auto operator()(set_type const& active_pawns) const
        {
                return active_pawns.any() ? king_targets_dispatch(active_pawns, is_pawn_jump_king_t<rules_type>{}) : false;
        }

private:
        // pawns that can capture kings
        auto king_targets_dispatch(set_type const& active_pawns, std::true_type) const
        {
                return branch(active_pawns);
        }

        // pawns that cannot capture kings
        auto king_targets_dispatch(set_type const& active_pawns, std::false_type) const
        {
                raii::ToggleKingTargets<Tracker> guard{tracker};
                return branch(active_pawns);
        }

        auto branch(set_type const& active_pawns) const
        {
                return branch_dispatch(active_pawns, is_backward_pawn_jump_t<rules_type>{}, is_orthogonal_jump_t<rules_type>{});
        }

        // pawns that jump in the 2 forward diagonal directions
        auto branch_dispatch(set_type const& active_pawns, std::false_type, std::false_type) const
        {
                return
                        parallelize<left_up   (orientation)>(active_pawns) ||
                        parallelize<right_up  (orientation)>(active_pawns)
                ;
        }

        // pawns that jump in the 4 forward and backward diagonal directions
        auto branch_dispatch(set_type const& active_pawns, std::true_type, std::false_type) const
        {
                return
                        parallelize<left_up   (orientation)>(active_pawns) ||
                        parallelize<right_up  (orientation)>(active_pawns) ||
                        parallelize<left_down (orientation)>(active_pawns) ||
                        parallelize<right_down(orientation)>(active_pawns)
                ;
        }

        // pawns that jump in the 5 forward and sideways diagonal and orthogonal directions
        auto branch_dispatch(set_type const& active_pawns, std::false_type, std::true_type) const
        {
                return
                        parallelize<up        (orientation)>(active_pawns) ||
                        parallelize<left_up   (orientation)>(active_pawns) ||
                        parallelize<right_up  (orientation)>(active_pawns) ||
                        parallelize<left      (orientation)>(active_pawns) ||
                        parallelize<right     (orientation)>(active_pawns)
                ;
        }

        // pawns that jump in the 8 diagonal and orthogonal directions
        auto branch_dispatch(set_type const& active_pawns, std::true_type, std::true_type) const
        {
                return
                        parallelize<up        (orientation)>(active_pawns) ||
                        parallelize<left_up   (orientation)>(active_pawns) ||
                        parallelize<right_up  (orientation)>(active_pawns) ||
                        parallelize<left      (orientation)>(active_pawns) ||
                        parallelize<right     (orientation)>(active_pawns) ||
                        parallelize<left_down (orientation)>(active_pawns) ||
                        parallelize<right_down(orientation)>(active_pawns) ||
                        parallelize<down      (orientation)>(active_pawns)
                ;
        }

        template<int Direction>
        auto parallelize(set_type const& active_pawns) const
        {
                return Sandwich<board_type, Direction, short_ranged_tag>{}(
                        active_pawns, tracker.template targets<Direction>(), tracker.path()
                ).any();
        }
};

}       // namespace detail
}       // namespace actions
}       // namespace dctl
