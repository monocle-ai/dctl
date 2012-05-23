#pragma once
#include <boost/mpl/bool_fwd.hpp>       // false_, true_
#include <boost/mpl/identity.hpp>       // identity
#include "Driver_fwd.hpp"
#include "KingJumps.hpp"
#include "PawnJumps.hpp"
#include "Selection.hpp"
#include "../capture/State.hpp"
#include "../node/Material.hpp"
#include "../node/Stack.hpp"
#include "../utility/nonconstructible.hpp"

namespace dctl {
namespace successor {

template<bool Color, typename Rules, typename Board>
struct Driver<Color, Material::both, select::Jumps, Rules, Board>
:
        // enforce static semantics
        private nonconstructible
{
private:
        // typedefs

        typedef Driver<Color, Material::king, select::Jumps, Rules, Board> KingJumps;
        typedef Driver<Color, Material::pawn, select::Jumps, Rules, Board> PawnJumps;
        typedef capture::State<Rules, Board> State;

public:
        template<template<typename, typename> class Position>
        static void generate(Position<Rules, Board> const& p, Stack& moves)
        {
                State capture(p, moves);
                generate(p, capture);
        }

        template<template<typename, typename> class Position>
        static int count(Position<Rules, Board> const& p)
        {
                Stack moves;
                generate(p, moves);
                return static_cast<int>(moves.size());
        }

        template<template<typename, typename> class Position>
        static bool detect(Position<Rules, Board> const& p)
        {
                // speculate #pawns > #kings so that the || is likely to short-circuit
                return (
                        PawnJumps::detect(p) ||
                        KingJumps::detect(p)
                );
        }

private:
        template<template<typename, typename> class Position>
        static void generate(Position<Rules, Board> const& p, State& capture)
        {
                // tag dispatching on absolute king capture precedence
                generate_dispatch(
                        p, capture,
                        boost::mpl::identity<typename Rules::is_absolute_king_precedence>()
                );
        }

        // partial specialization for no absolute king capture precedence
        template<template<typename, typename> class Position>
        static void generate_dispatch(
                Position<Rules, Board> const& p, State& capture, 
                boost::mpl::identity<boost::mpl::false_>
        )
        {
                KingJumps::generate(p, capture);
                PawnJumps::generate(p, capture);
        }

        // partial specialization for absolute king capture precedence
        template<template<typename, typename> class Position>
        static void generate_dispatch(
                Position<Rules, Board> const& p, State& capture, 
                boost::mpl::identity<boost::mpl::true_>
        )
        {
                KingJumps::generate(p, capture);
                if (capture.empty())
                        PawnJumps::generate(p, capture);
        }
};

}       // namespace successor
}       // namespace dctl
