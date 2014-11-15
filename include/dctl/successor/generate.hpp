#pragma once
#include <dctl/successor/select/legal.hpp>              // DefaultSelection
#include <dctl/successor/generate/specializations.hpp>  // Generate
#include <dctl/successor/invariant.hpp>                 // invariant
#include <dctl/pieces/pieces.hpp>                       // pawn, king, piece
#include <dctl/move/move.hpp>                           // Move
#include <dctl/position/color.hpp>                      // black, white
#include <dctl/type_traits.hpp>                         // value_type_t
#include <dctl/utility/stack_vector.hpp>                // DCTL_PP_STACK_RESERVE
#include <memory>                                       // allocator, allocator_traits
#include <vector>                                       // vector

namespace dctl {
namespace successor {

template<class Pieces, class Select, class Position, class Sequence>
void generate(Position const& p, Sequence& moves)
{
        if (p.active_color() == Color::black)
                Generate<Color::black, Pieces, Select>{}(p, moves);
        else
                Generate<Color::white, Pieces, Select>{}(p, moves);
        assert((invariant<Pieces, Select>(p, moves.size())));
}

template
<
        class Pieces, class Select, class Position,
        class Allocator = std::allocator<Move_t<Position>>
>
auto generate(Position const& p, Allocator const& alloc = Allocator())
{
        using Move = value_type_t<std::allocator_traits<Allocator>>;
        std::vector<Move, Allocator> moves(alloc);
        moves.reserve(DCTL_PP_STACK_RESERVE);
        generate<Pieces, Select>(p, moves);
        return moves;
}

template<class Position, class Allocator = std::allocator<Move_t<Position>>>
auto generate(Position const& p, Allocator const& alloc = Allocator())
{
        return generate<pieces::all, select::legal>(p, alloc);
}

template<class Position, class Move>
auto make_copy(Position const& p, Move const& m)
{
        auto q = p;
        q.attach(p);
        q.make(m);
        return q;
}

}       // namespace successor
}       // namespace dctl
