#pragma once
#include <type_traits>                  // is_same 
#include <boost/mpl/bool_fwd.hpp>       // false_, true_
#include <boost/mpl/identity.hpp>       // identity
#include <boost/mpl/eval_if.hpp>        // eval_if
#include <boost/mpl/identity.hpp>       // identity
#include <boost/mpl/int_fwd.hpp>        // int_
#include <boost/mpl/logical.hpp>        // and_, not_, or_

#define BOOST_PARAMETER_MAX_ARITY 13    // will be infinite with C++11 variadic templates
#include <boost/parameter.hpp>

#include "Enum.hpp"

namespace dctl {
namespace rules {

//+----------------------------------------------------------------------------+
//|      Drawing rules                                                         |
//+----------------------------------------------------------------------------+

// restricted repeated positions
template<typename> struct is_restricted_repetitions             { enum { value = true }; };

// maximum number of repeated positions
template<typename> struct max_repetitions                       { enum { value =  3 }; };       // (FMJD rule 6.1)

// restricted consecutive non-conversion moves
template<typename> struct is_restricted_reversible_moves        { enum { value = true }; };

// maximum allowed consecutive non-conversion moves
template<typename> struct max_reversible_moves                  { enum { value = 50 }; };       // (FMJD rule 6.2)

// other drawing rules
template<typename> struct max_3v1_moves                         { enum { value = 32 }; };       // (FMJD rule 6.3)
template<typename> struct max_2v1_moves                         { enum { value = 10 }; };       // (FMJD rule 6.4)

//+----------------------------------------------------------------------------+
//|      Capture ambiguity                                                     |
//+----------------------------------------------------------------------------+

template<typename> struct is_check_jump_uniqueness              { enum { value = true }; };

// intermediate capture directions
template<typename T> 
struct turn_directions
:
        boost::mpl::identity<T>
{};

template<> 
struct turn_directions<directions::orth>
:
        boost::mpl::identity<directions::diag>
{};

BOOST_PARAMETER_TEMPLATE_KEYWORD(king_range)
BOOST_PARAMETER_TEMPLATE_KEYWORD(pawn_jump_directions)
BOOST_PARAMETER_TEMPLATE_KEYWORD(jump_precedence)

BOOST_PARAMETER_TEMPLATE_KEYWORD(is_restricted_same_king_moves)
BOOST_PARAMETER_TEMPLATE_KEYWORD(max_same_king_moves)
BOOST_PARAMETER_TEMPLATE_KEYWORD(land_range)
BOOST_PARAMETER_TEMPLATE_KEYWORD(halt_range)
BOOST_PARAMETER_TEMPLATE_KEYWORD(is_pawns_jump_kings)
BOOST_PARAMETER_TEMPLATE_KEYWORD(is_jump_direction_reversal)
BOOST_PARAMETER_TEMPLATE_KEYWORD(jump_removal)
BOOST_PARAMETER_TEMPLATE_KEYWORD(pawn_promotion)
BOOST_PARAMETER_TEMPLATE_KEYWORD(is_absolute_king_precedence)
BOOST_PARAMETER_TEMPLATE_KEYWORD(is_relative_king_precedence)

typedef boost::parameter::parameters<
        // required parameters

        boost::parameter::required<tag::king_range>,
        boost::parameter::required<tag::pawn_jump_directions>,
        boost::parameter::required<tag::jump_precedence>,

        // optional parameters

        boost::parameter::optional<tag::is_restricted_same_king_moves>,
        boost::parameter::optional<tag::max_same_king_moves>,
        boost::parameter::optional<tag::land_range>,
        boost::parameter::optional<tag::halt_range>,
        boost::parameter::optional<tag::is_pawns_jump_kings>,
        boost::parameter::optional<tag::is_jump_direction_reversal>,
        boost::parameter::optional<tag::jump_removal>,
        boost::parameter::optional<tag::pawn_promotion>,
        boost::parameter::optional<tag::is_absolute_king_precedence>,
        boost::parameter::optional<tag::is_relative_king_precedence>
> Signature;

template
<
        class A0 = boost::parameter::void_,
        class A1 = boost::parameter::void_,
        class A2 = boost::parameter::void_,
        class A3 = boost::parameter::void_,
        class A4 = boost::parameter::void_,
        class A5 = boost::parameter::void_,
        class A6 = boost::parameter::void_,
        class A7 = boost::parameter::void_,
        class A8 = boost::parameter::void_,
        class A9 = boost::parameter::void_,
        class A10 = boost::parameter::void_,
        class A11 = boost::parameter::void_,
        class A12 = boost::parameter::void_
>
struct Rules
{
        // create argument pack
        typedef typename Signature::bind<
                A0, A1, A2, A3, A4, A5, A6, A7, 
                A8, A9, A10, A11, A12
        >::type args;

        // extract required parameters

        typedef typename boost::parameter::value_type<
                args, tag::king_range
        >::type king_range;

        typedef typename boost::parameter::value_type<
                args, tag::pawn_jump_directions
        >::type pawn_jump_directions;

        typedef typename boost::parameter::value_type<
                args, tag::jump_precedence
        >::type jump_precedence;

        // extract optional parameters

        typedef typename boost::parameter::value_type<
                args, tag::is_restricted_same_king_moves, boost::mpl::false_
        >::type is_restricted_same_king_moves;

        typedef typename boost::parameter::value_type<
                args, tag::max_same_king_moves, boost::mpl::int_<0>
        >::type max_same_king_moves;

        typedef typename boost::parameter::value_type<
                args, tag::land_range, king_range
        >::type land_range;

        typedef typename boost::parameter::value_type<
                args, tag::halt_range, land_range
        >::type halt_range;

        typedef typename boost::parameter::value_type<
                args, tag::is_pawns_jump_kings, boost::mpl::true_
        >::type is_pawns_jump_kings;

        typedef typename boost::parameter::value_type<
                args, tag::is_jump_direction_reversal, boost::mpl::false_
        >::type is_jump_direction_reversal;        
        
        typedef typename boost::parameter::value_type<
                args, tag::jump_removal, removal::apres_fini
        >::type jump_removal;

        typedef typename boost::parameter::value_type<
                args, tag::pawn_promotion, promotion::apres_fini
        >::type pawn_promotion;

        typedef typename boost::parameter::value_type<
                args, tag::is_absolute_king_precedence, boost::mpl::false_
        >::type is_absolute_king_precedence;

        typedef typename boost::parameter::value_type<
                args, tag::is_relative_king_precedence, boost::mpl::false_
        >::type is_relative_king_precedence;

        // compute auxiliary parameters

        typedef typename boost::mpl::eval_if<
                std::is_same<pawn_jump_directions, directions::all>,
                boost::mpl::identity<directions::all>,
                boost::mpl::identity<directions::diag>
        >::type king_jump_directions;

        typedef typename turn_directions<
                king_jump_directions
        >::type king_turn_directions;

        typedef typename turn_directions<
                pawn_jump_directions
        >::type pawn_turn_directions;

        typedef typename boost::mpl::or_<
                boost::mpl::not_<
                        std::is_same<pawn_jump_directions, directions::up> 
                >,  
                std::is_same<pawn_promotion, promotion::en_passant>
        >::type is_ambiguous_pawn_jump;

        typedef typename boost::mpl::eval_if<
                boost::mpl::or_<
                        boost::mpl::and_<
                                std::is_same<jump_removal, removal::apres_fini>,
                                std::is_same<is_jump_direction_reversal, boost::mpl::true_>
                        >,
                        std::is_same<pawn_jump_directions, directions::all>
                >,
                boost::mpl::int_<3>,
                boost::mpl::int_<4>
        >::type large_jump;

        typedef typename boost::mpl::not_<
                std::is_same<jump_precedence, precedence::none> 
        >::type is_majority_precedence;
};

}       // namespace rules
}       // namespace dctl
