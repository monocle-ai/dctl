#pragma once
#include <dctl/successor/generate/primary_fwd.hpp>      // Generate (primary template)
#include <dctl/successor/generate/king_jump.hpp>        // promote_en_passant
#include <dctl/successor/propagate/jump.hpp>            // Propagate (jumps specialization)
#include <dctl/successor/select/jump.hpp>               // jumps
#include <dctl/pieces/king.hpp>                         // king
#include <dctl/pieces/pawn.hpp>                         // pawn

#include <dctl/angle.hpp>                               // _deg, rotate, inverse
#include <dctl/board/orientation.hpp>                   // orientation_v
#include <dctl/position/promotion.hpp>
#include <dctl/rule_traits.hpp>
#include <dctl/utility/algorithm.hpp>
#include <dctl/ray.hpp>                                 // make_iterator, rotate, mirror, turn
#include <dctl/wave/iterator.hpp>
#include <cassert>                                      // assert
#include <iterator>                                     // prev
#include <type_traits>                                  // false_type, true_type
#include <utility>                                      // pair

namespace dctl {
namespace successor {

// partial specialization for pawn jumps generation
template<bool Color, class Position, class Sequence>
class Generate<Color, pieces::pawn, select::jump, Position, Sequence>
{
public:
        // enforce reference semantics
        Generate(Generate const&) = delete;
        Generate& operator=(Generate const&) = delete;

private:
        using KingJumps = Generate<Color, pieces::king, select::jump, Position, Sequence>;
        using Rules = rules_type_t<Position>;
        using Board = board_type_t<Position>;
        using Set = set_type_t<Position>;
        using State = Propagate<select::jump, Position>;

        static constexpr auto orientation = orientation_v<Board, Color>;

        template<class Iterator>
        static constexpr auto direction_v = rotate(ray::direction_v<Iterator>, inverse(orientation));

        // representation

        State& tracker_;
        Sequence& moves_;

public:
        // constructors

        explicit Generate(State& c, Sequence& m)
        :
                tracker_{c},
                moves_{m}
        {}

        // function call operators

        void operator()(Set const& active_pawns) const
        {
                if (active_pawns.empty())
                        return;

                // tag dispatching on whether pawns can capture kings
                select_dispatch(active_pawns, is_pawn_jump_king_t<Rules>{});
        }

private:
        // overload for pawns that can capture kings
        void select_dispatch(Set const& active_pawns, std::true_type) const
        {
                branch(active_pawns);
        }

        // overload for pawns that cannot capture kings
        void select_dispatch(Set const& active_pawns, std::false_type) const
        {
                tracker_.toggle_king_targets();
                branch(active_pawns);
                tracker_.toggle_king_targets();
        }

        void branch(Set const& active_pawns) const
        {
                // tag dispatching on pawn jump directions
                branch_dispatch(active_pawns, std::pair<is_backward_pawn_jump_t<Rules>, is_orthogonal_jump_t<Rules>>{});
        }

        // overload for pawns that jump in the 2 forward diagonal directions
        void branch_dispatch(Set const& active_pawns, std::pair<std::false_type, std::false_type>) const
        {
                serialize<left_up   (orientation)>(active_pawns);
                serialize<right_up  (orientation)>(active_pawns);
        }

        // overload for pawns that jump in the 4 forward and backward diagonal directions
        void branch_dispatch(Set const& active_pawns, std::pair<std::true_type, std::false_type>) const
        {
                serialize<left_up   (orientation)>(active_pawns);
                serialize<right_up  (orientation)>(active_pawns);
                serialize<left_down (orientation)>(active_pawns);
                serialize<right_down(orientation)>(active_pawns);
        }

        // overload for pawns that jump in the 5 forward and sideways diagonal and orthogonal directions
        void branch_dispatch(Set const& active_pawns, std::pair<std::false_type, std::true_type>) const
        {
                serialize<up        (orientation)>(active_pawns);
                serialize<left_up   (orientation)>(active_pawns);
                serialize<right_up  (orientation)>(active_pawns);
                serialize<left      (orientation)>(active_pawns);
                serialize<right     (orientation)>(active_pawns);
        }

        // overload for pawns that jump in the 8 diagonal and orthogonal directions
        void branch_dispatch(Set const& active_pawns, std::pair<std::true_type, std::true_type>) const
        {
                serialize<up        (orientation)>(active_pawns);
                serialize<left_up   (orientation)>(active_pawns);
                serialize<right_up  (orientation)>(active_pawns);
                serialize<left      (orientation)>(active_pawns);
                serialize<right     (orientation)>(active_pawns);
                serialize<left_down (orientation)>(active_pawns);
                serialize<right_down(orientation)>(active_pawns);
                serialize<down      (orientation)>(active_pawns);
        }

        template<int Direction>
        void serialize(Set const& active_pawns) const
        {
                auto const jumpers = active_pawns & *std::prev(along_wave<Direction>(tracker_.template targets_with_pawn<Direction>()));
                for (auto&& from_sq : jumpers)
                        find(along_ray<Direction>(from_sq));
        }

        template<class Iterator>
        void find(Iterator jumper) const
        {
                tracker_.launch(*jumper);
                explore(std::next(jumper));     // recursively find more jumps
                tracker_.finish();
        }

        template<class Iterator>
        void explore(Iterator jumper) const
        {
                tracker_.capture(*jumper);
                add_and_continue(std::next(jumper));
                tracker_.release();
        }

        template<class Iterator>
        void add_and_continue(Iterator jumper) const
        {
                // tag dispatching on majority precedence
                tracker_.visit(*jumper);
                if (!is_find_next(jumper))
                        precedence_dispatch(jumper, is_jump_precedence_t<Rules>{});
                tracker_.leave();
        }

        // overload for no majority precedence
        template<class Iterator>
        void precedence_dispatch(Iterator jumper, std::false_type) const
        {
                add(jumper);
        }

        // overload for majority precedence
        template<class Iterator>
        void precedence_dispatch(Iterator jumper, std::true_type) const
        {
                if (tracker_.handle_precedence(moves_))
                        add(jumper);
        }

        template<class Iterator>
        bool is_find_next(Iterator jumper) const
        {
                // tag dispatching on promotion condition
                return promotion_dispatch(jumper, is_en_passant_promotion_t<Rules>{});
        }

        // overload for pawns that promote apres-fini
        template<class Iterator>
        bool promotion_dispatch(Iterator jumper, std::false_type) const
        {
                return land(jumper);
        }

        // overload for pawns that promote en-passant
        template<class Iterator>
        bool promotion_dispatch(Iterator jumper, std::true_type) const
        {
                return is_promotion(*jumper) ? KingJumps{tracker_, moves_}.promote_en_passant(jumper) : land(jumper);
        }

        template<class Iterator>
        bool land(Iterator jumper) const
        {
                // CORRECTNESS: bitwise instead of logical OR to disable short-circuiting
                return scan(jumper) | turn(jumper);
        }

        template<class Iterator>
        bool turn(Iterator jumper) const
        {
                // tag dispatching on pawn turn directions
                return turn_dispatch(jumper, std::pair<is_backward_pawn_jump_t<Rules>, is_orthogonal_jump_t<Rules>>{});
        }

        // overload for pawns that jump in the 2 forward diagonal directions
        template<class Iterator>
        bool turn_dispatch(Iterator jumper, std::pair<std::false_type, std::false_type>) const
        {
                static_assert(is_up(direction_v<Iterator>) && is_diagonal(direction_v<Iterator>), "");
                return scan(ray::mirror<up(orientation)>(jumper));
        }

        // overload for pawns that jump in the 4 forward and backward diagonal directions
        template<class Iterator>
        bool turn_dispatch(Iterator jumper, std::pair<std::true_type, std::false_type>) const
        {
                static_assert(is_diagonal(direction_v<Iterator>), "");

                // CORRECTNESS: bitwise instead of logical OR to disable short-circuiting
                return
                        scan(ray::rotate<+90_deg>(jumper)) |
                        scan(ray::rotate<-90_deg>(jumper))
                ;
        }

        // overload for pawns that jump in the 5 forward and sideways diagonal and orthogonal directions
        template<class Iterator>
        bool turn_dispatch(Iterator jumper, std::pair<std::false_type, std::true_type>) const
        {
                static_assert(!is_down(direction_v<Iterator>) && (is_diagonal(direction_v<Iterator>) || is_orthogonal(direction_v<Iterator>)), "");

                // tag dispatching on the current jump direction
                return turn_dispatch(jumper, angle_t<direction_v<Iterator>>{});
        }

        // overload for the upward direction
        template<class Iterator>
        bool turn_dispatch(Iterator jumper, angle_t<up(orientation)>)
        {
                // CORRECTNESS: bitwise instead of logical OR to disable short-circuiting
                return
                        scan(ray::turn<left_up (orientation)>(jumper)) |
                        scan(ray::turn<right_up(orientation)>(jumper)) |
                        scan(ray::turn<left    (orientation)>(jumper)) |
                        scan(ray::turn<right   (orientation)>(jumper))
                ;
        }

        // overload for the left upward direction
        template<class Iterator>
        bool turn_dispatch(Iterator jumper, angle_t<left_up(orientation)>)
        {
                // CORRECTNESS: bitwise instead of logical OR to disable short-circuiting
                return
                        scan(ray::turn<up      (orientation)>(jumper)) |
                        scan(ray::turn<right_up(orientation)>(jumper)) |
                        scan(ray::turn<left    (orientation)>(jumper)) |
                        scan(ray::turn<right   (orientation)>(jumper))
                ;
        }

        // overload for the right upward direction
        template<class Iterator>
        bool turn_dispatch(Iterator jumper, angle_t<right_up(orientation)>)
        {
                // CORRECTNESS: bitwise instead of logical OR to disable short-circuiting
                return
                        scan(ray::turn<up      (orientation)>(jumper)) |
                        scan(ray::turn<left_up (orientation)>(jumper)) |
                        scan(ray::turn<left    (orientation)>(jumper)) |
                        scan(ray::turn<right   (orientation)>(jumper))
                ;
        }

        // overload for the left direction
        template<class Iterator>
        bool turn_dispatch(Iterator jumper, angle_t<left(orientation)>)
        {
                // CORRECTNESS: bitwise instead of logical OR to disable short-circuiting
                return
                        scan(ray::turn<up      (orientation)>(jumper)) |
                        scan(ray::turn<left_up (orientation)>(jumper)) |
                        scan(ray::turn<right_up(orientation)>(jumper))
                ;
        }

        // overload for the right direction
        template<class Iterator>
        bool turn_dispatch(Iterator jumper, angle_t<right(orientation)>)
        {
                // CORRECTNESS: bitwise instead of logical OR to disable short-circuiting
                return
                        scan(ray::turn<up      (orientation)>(jumper)) |
                        scan(ray::turn<left_up (orientation)>(jumper)) |
                        scan(ray::turn<right_up(orientation)>(jumper))
                ;
        }

        // overload for pawns that jump in the 8 diagonal and orthogonal directions
        template<class Iterator>
        bool turn_dispatch(Iterator jumper, std::pair<std::true_type, std::true_type>) const
        {
                static_assert(is_diagonal(direction_v<Iterator>) || is_orthogonal(direction_v<Iterator>), "");

                // CORRECTNESS: bitwise instead of logical OR to disable short-circuiting
                return
                        scan(ray::rotate< +45_deg>(jumper)) |
                        scan(ray::rotate< -45_deg>(jumper)) |
                        scan(ray::rotate< +90_deg>(jumper)) |
                        scan(ray::rotate< -90_deg>(jumper)) |
                        scan(ray::rotate<+135_deg>(jumper)) |
                        scan(ray::rotate<-135_deg>(jumper))
                ;
        }

        template<class Iterator>
        bool scan(Iterator jumper) const
        {
                return is_en_prise(std::next(jumper));
        }

        template<class Iterator>
        bool is_en_prise(Iterator jumper) const
        {
                if (!tracker_.targets_with_pawn(jumper))
                        return false;

                explore(jumper);        // recursively find more jumps
                return true;
        }

        template<class Iterator>
        void add(Iterator dest) const
        {
                // tag dispatching on ambiguity of pawn jumps
                ambiguity_dispatch(dest, is_unambiguous_pawn_jump_t<Rules>{});
        }

        // overload for pawn jumps that are always unambiguous
        template<class Iterator>
        void ambiguity_dispatch(Iterator dest, std::true_type) const
        {
                tracker_.template add_pawn_jump<Color, with::pawn>(*dest, moves_);
        }

        // overload for pawn jumps that are potentially ambiguous
        template<class Iterator>
        void ambiguity_dispatch(Iterator dest, std::false_type) const
        {
                auto const check_duplicate = is_remove_duplicates_v<Rules> && tracker_.is_potential_duplicate(moves_);
                tracker_.template add_pawn_jump<Color, with::pawn>(*dest, moves_);
                if (check_duplicate && util::is_duplicate_back(moves_))
                        moves_.pop_back();
        }

        template<int Direction>
        static wave::Iterator<Board, Direction> along_wave(Set const& s)
        {
                return wave::make_iterator<Board, Direction>(s);
        }

        template<int Direction>
        static ray::Iterator<Board, Direction> along_ray(int sq)
        {
                return ray::make_iterator<Board, Direction>(sq);
        }

        static bool is_promotion(int sq)
        {
                return dctl::is_promotion<Color, Board>(sq);
        }
};

}       // namespace successor
}       // namespace dctl
