#pragma once
#include <boost/mpl/arithmetic.hpp>     // divides, minus, modulus, plus, times
#include <boost/mpl/eval_if.hpp>        // eval_if
#include <boost/mpl/logical.hpp>        // not_
#include <boost/mpl/int.hpp>            // int_
#include <boost/static_assert.hpp>      // BOOST_STATIC_ASSERT
#include <dctl/board/grid_fwd.hpp>      // primary template and partial specialization declarations

namespace dctl {
namespace board {

// primary template definition
template
<
        typename Dimensions,
        typename GhostColumns
>
struct Grid
:
        Dimensions
{
public:
        BOOST_STATIC_ASSERT(GhostColumns::value > 0);

        typedef Grid<Dimensions, no_ghosts> BaseGrid;

        // diagonal directions

        typedef boost::mpl::divides<
                boost::mpl::plus< typename
                        Dimensions::width,
                        GhostColumns
                >,
                boost::mpl::int_<2>
        > left_down;

        typedef boost::mpl::plus<
                left_down,
                boost::mpl::int_<1>
        > right_down;

        // orthogonal directions

        typedef boost::mpl::minus<
                right_down,
                left_down
        > right;

        typedef boost::mpl::plus<
                right_down,
                left_down
        > down;

        // equivalent directions

        typedef right_down      left_up;
        typedef left_down       right_up;
        typedef right           left;
        typedef down            up;

        // range of row pairs

        typedef down modulo;

        // left (l) and right (r) edges of even (e) and odd (o) rows

        typedef typename BaseGrid::edge_le edge_le;
        typedef typename BaseGrid::edge_re edge_re;

        typedef boost::mpl::plus<
                left_down, typename
                Dimensions::parity
        > edge_lo;

        typedef boost::mpl::plus<
                edge_lo,
                boost::mpl::minus< typename
                        BaseGrid::edge_ro, typename
                        BaseGrid::edge_lo
                >
        > edge_ro;

        // grid size

        typedef boost::mpl::plus<
                boost::mpl::times<
                        modulo,
                        boost::mpl::divides<
                                boost::mpl::minus< typename
                                        Dimensions::height,
                                        boost::mpl::int_<1>
                                >,
                                boost::mpl::int_<2>
                        >
                >, typename
                boost::mpl::eval_if<
                        boost::mpl::modulus< typename
                                Dimensions::height,
                                boost::mpl::int_<2>
                        >,
                        edge_re,
                        edge_ro
                >::type,
                boost::mpl::int_<1>
        > size;
};

// partial specialization definition
template<typename Dimensions>
struct Grid< Dimensions, no_ghosts >
:
        public Dimensions
{
private:
        // range of even (e) and odd (o) rows

        typedef boost::mpl::divides<
                boost::mpl::plus< typename
                        Dimensions::width, typename
                        Dimensions::parity
                >,
                boost::mpl::int_<2>
        > row_e;

        typedef boost::mpl::divides<
                boost::mpl::plus< typename
                        Dimensions::width,
                        boost::mpl::not_< typename
                                Dimensions::parity
                        >
                >,
                boost::mpl::int_<2>
        > row_o;

public:
        // range of row pairs

        typedef typename Dimensions::width modulo;

        // left (l) and right (r) edges of even (e) and odd (o) rows

        typedef boost::mpl::int_<0> edge_le;

        typedef boost::mpl::plus<
                edge_le,
                boost::mpl::minus<
                        row_e,
                        boost::mpl::int_<1>
                >
        > edge_re;

        typedef boost::mpl::plus<
                edge_re,
                boost::mpl::int_<1>
        > edge_lo;

        typedef boost::mpl::plus<
                edge_lo,
                boost::mpl::minus<
                        row_o,
                        boost::mpl::int_<1>
                >
        > edge_ro;

        typedef boost::mpl::plus<
                boost::mpl::times<
                        modulo,
                        boost::mpl::divides<
                                boost::mpl::minus< typename
                                        Dimensions::height,
                                        boost::mpl::int_<1>
                                >,
                                boost::mpl::int_<2>
                        >
                >, typename
                boost::mpl::eval_if<
                        boost::mpl::modulus< typename
                                Dimensions::height,
                                boost::mpl::int_<2>
                        >,
                        edge_re,
                        edge_ro
                >::type,
                boost::mpl::int_<1>
        > size;

        // equivalent grid size
        /*
        BOOST_STATIC_ASSERT(size == (Dimensions::height::value * Dimensions::width::value) / 2 +
                (Dimensions::parity::value * Dimensions::height::value * Dimensions::width::value) % 2
        );
        */
};

}       // namespace board
}       // namespace dctl
