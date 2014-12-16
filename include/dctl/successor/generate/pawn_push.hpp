#pragma once
#include <dctl/angle.hpp>                               // left_up, right_up
#include <dctl/color.hpp>                               // Color
#include <dctl/piece.hpp>                               // PiecePawnType
#include <dctl/position/promotion.hpp>                  // is_promotion
#include <dctl/successor/generate/primary_fwd.hpp>      // Generate (primary template)
#include <dctl/successor/select/push.hpp>               // select

#include <dctl/board/orientation.hpp>                   // orientation_v
#include <dctl/ray.hpp>                                 // make_iterator
#include <dctl/type_traits.hpp>                         // board_type_t, set_type_t, value_type_t
#include <dctl/wave/iterator.hpp>                       // make_iterator
#include <boost/range/adaptor/transformed.hpp>          // transformed
#include <boost/range/algorithm_ext/push_back.hpp>      // push_back
#include <iterator>                                     // prev

namespace dctl {
namespace successor {

template<Color ToMove, bool IsReverse, class Position, class Sequence>
class Generate<ToMove, select::push, IsReverse, PiecePawnType, Position, Sequence>
{
        using board_type = board_type_t<Position>;
        using   set_type =   set_type_t<Position>;
        using  move_type = value_type_t<Sequence>;

        static constexpr auto orientation = orientation_v<board_type, ToMove, IsReverse>;
        set_type const& not_occupied;
        Sequence& moves;

public:
        Generate(set_type const& s, Sequence& m)
        :
                not_occupied{s},
                moves{m}
        {}

        auto operator()(set_type const& active_pawns) const
        {
                if (active_pawns.none())
                        return;

                transform_movers<left_up (orientation)>(active_pawns);
                transform_movers<right_up(orientation)>(active_pawns);
        }

private:
        template<int Direction>
        auto transform_movers(set_type const& active_pawns) const
        {
                auto const movers = active_pawns & set_type(*std::prev(along_wave<Direction>(not_occupied)));
                boost::push_back(moves, movers | boost::adaptors::transformed([](auto const& from_sq) {
                        auto const dest_sq = *++along_ray<Direction>(from_sq);
                        return move_type{from_sq, dest_sq, ToMove, is_promotion(dest_sq)};
                }));
        }

        template<int Direction>
        static auto along_wave(set_type const& s)
        {
                return wave::make_iterator<board_type, Direction>(s);
        }

        template<int Direction>
        static auto along_ray(std::size_t sq)
        {
                return ray::make_iterator<board_type, Direction>(sq);
        }

        static auto is_promotion(std::size_t sq)
        {
                return dctl::is_promotion<board_type, ToMove>(sq);
        }
};

}       // namespace successor
}       // namespace dctl
