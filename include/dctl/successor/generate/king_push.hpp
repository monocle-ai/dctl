#pragma once
#include <dctl/successor/generate/primary_fwd.hpp>      // Generate (primary template)
#include <dctl/successor/select/push.hpp>               // push
#include <dctl/pieces/king.hpp>                         // king

#include <dctl/angle.hpp>                               // left_up, right_up, left_down, right_down
#include <dctl/board/orientation.hpp>                   // orientation_v
#include <dctl/position/unary_projections.hpp>
#include <dctl/ray.hpp>                                 // make_iterator
#include <dctl/rule_traits.hpp>
#include <boost/range/adaptor/transformed.hpp>          // transformed
#include <boost/range/algorithm_ext/push_back.hpp>      // push_back
#include <cstddef>                                      // size_t
#include <iterator>                                     // prev

namespace dctl {
namespace successor {

// partial specialization for king moves generation
template<Color ToMove, class Position, class Sequence>
class Generate<ToMove, pieces::king, select::push, Position, Sequence>
{
        using Rules = rules_type_t<Position>;
        using Board = board_type_t<Position>;
        using Set = set_type_t<Position>;
        using Move = value_type_t<Sequence>;

        static constexpr auto orientation = orientation_v<Board, ToMove>;
        Set const& propagate;
        Sequence& moves;

public:
        Generate(Set const& p, Sequence& m)
        :
                propagate{p},
                moves{m}
        {}

        // enforce reference semantics
        Generate(Generate const&) = delete;
        Generate& operator=(Generate const&) = delete;

        // function call operators

        auto operator()(Set const& active_kings) const
        {
                // tag dispatching on king range
                find_dispatch(active_kings, is_long_ranged_king_t<Rules>{});
        }

private:
        // short ranged kings
        auto find_dispatch(Set const& active_kings, std::false_type) const
        {
                if (active_kings.none())
                        return;

                transform_movers<left_up   (orientation)>(active_kings);
                transform_movers<right_up  (orientation)>(active_kings);
                transform_movers<left_down (orientation)>(active_kings);
                transform_movers<right_down(orientation)>(active_kings);
        }

        // long ranged kings
        auto find_dispatch(Set const& active_kings, std::true_type) const
        {
                for (auto&& from_sq : active_kings) {
                        transform_targets(along_ray<left_up   (orientation)>(from_sq));
                        transform_targets(along_ray<right_up  (orientation)>(from_sq));
                        transform_targets(along_ray<left_down (orientation)>(from_sq));
                        transform_targets(along_ray<right_down(orientation)>(from_sq));
                }
        }

        template<int Direction>
        auto transform_movers(Set const& active_kings) const
        {
                auto const movers = active_kings & Set(*std::prev(along_wave<Direction>(propagate)));
                boost::push_back(moves, movers | boost::adaptors::transformed([](auto const& from_sq) {
                        return Move{from_sq, *++along_ray<Direction>(from_sq), ToMove};
                }));
        }

        template<class Iterator>
        auto transform_targets(Iterator from) const
        {
                auto const targets = ray::classical(from, propagate);
                boost::push_back(moves, targets | boost::adaptors::transformed([=](auto const& dest_sq) {
                        return Move{*from, dest_sq, ToMove};
                }));
        }

        template<int Direction>
        static auto along_wave(Set const& s)
        {
                return wave::make_iterator<Board, Direction>(s);
        }

        template<int Direction>
        static auto along_ray(std::size_t sq)
        {
                return ray::make_iterator<Board, Direction>(sq);
        }
};

}       // namespace successor
}       // namespace dctl
