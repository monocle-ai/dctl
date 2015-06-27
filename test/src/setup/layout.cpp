#include <iostream>
#include <typeinfo>
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END
#include <dctl/board/types.hpp>                 // International
#include <dctl/setup/diagram.hpp>               // diagram, bits, squares
#include <dctl/board/algebraic.hpp>
#include <iterator>
#include <dctl/state/state.hpp>
#include <dctl/setup/diagram.hpp>
#include <dctl/rules.hpp>
#include <dctl/setup/string.hpp>

namespace dctl {
namespace setup {

BOOST_AUTO_TEST_SUITE(SetupLayout)

using BSequence = boost::mpl::vector
<
        board::Checkers,
        board::Board< 8, 10>,
        board::Board<10,  8>,
        board::International,
        board::Board<10, 12>,
        board::Board<12, 10>,
        board::Compact1210,
        board::Canadian,
        board::Checkers,
        board::Ktar< 8, 10>,
        board::Ktar<10,  8>,
        board::International,
        board::Ktar<10, 11>,
        board::Ktar<10, 12>,
        board::Compact1012,
        board::Ktar<12, 10>,
        board::Canadian
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(Initial, T, BSequence)
{
        auto const p = State<rules::International, T>::initial();
        std::cout << "W = " << T::width() << ", H = " << T::height() << ", bits = " << T::bits() << "\n\n";
        std::cout << setup::diagram<pdn::protocol>()(p);
}

BOOST_AUTO_TEST_SUITE_END()

}       // namespace setup
}       // namespace dctl
