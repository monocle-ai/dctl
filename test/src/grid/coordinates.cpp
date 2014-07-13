#include <dctl/angle.hpp>                       // _deg, inverse, rotate
#include <dctl/board/types.hpp>                 // Micro, Mini, Checkers, International, Roman, Frisian, Spantsireti, Ktar11, Ktar12
#include <dctl/grid/coordinates.hpp>            // Square, ulo_from_sq, sco_from_ulo
#include <group.hpp>                            // action::is_realized, make
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/iterator/counting_iterator.hpp> // counting_iterator
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END, BOOST_CHECK
#include <algorithm>                            // all_of
#include <iterator>                             // begin, end
#include <type_traits>                          // common_type_t

namespace dctl {
namespace board {

BOOST_AUTO_TEST_SUITE(GridCoordinates)

using BoardSequence = boost::mpl::vector
<
        Micro,
        Mini,
        Checkers,
        International,
        Canadian,
        Dumm,
        Roman,
        Frisian,
        SriLankan,
        Spantsireti,
        Ktar<10, 11>,
        Ktar<10, 12>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(GroupActionIsRealizedForAllCyclicGroupsOnAllSquareCoordinates, T, BoardSequence)
{
        auto const op = [](auto i, auto j){ return rotate(i, j); };
        auto const inv = [](auto i){ return inverse(i); };

        auto const C1 = make_group(
                { 0_deg },
                op, inv
        );

        auto const C2 = make_group(
                { 0_deg, 180_deg },
                op, inv
        );

        auto const C4 = make_group(
                { 0_deg,  90_deg, 180_deg, 270_deg },
                op, inv
        );

        using CyclicGroup = std::common_type_t<decltype(C1), decltype(C2), decltype(C4)>;

        CyclicGroup const C_N[] =
        {
                C1, C2, C4
        };

        BOOST_CHECK(
                std::all_of(begin(C_N), end(C_N), [=](auto const& g) {
                        return std::all_of(T::begin(), T::end(), [&](auto i) {
                                auto const coord = sco_from_ulo(ulo_from_sq(grid::ulo::Square<typename T::external_grid>{i}));
                                return group::action::is_realized(coord, g);
                        });
                })
        );
}

BOOST_AUTO_TEST_SUITE_END()

}       // namespace board
}       // namespace dctl
