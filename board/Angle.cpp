#include <boost/test/unit_test.hpp> 
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>                   // list
#include <boost/mpl/placeholders.hpp>           // _1
using namespace boost::mpl::placeholders;
#include "Transform.hpp"                        // rotate, inverse, mirror_up, mirror_down
#include "../../src/board/Group.hpp"
#include <typeinfo>

namespace dctl {

BOOST_AUTO_TEST_SUITE(TestAngle)

BOOST_AUTO_TEST_CASE(Axioms)
{
        is_axioms<group::C1>()();
        is_axioms<group::C2>()();
        is_axioms<group::C4>()();
        is_axioms<group::C8>()();
}

BOOST_AUTO_TEST_CASE_TEMPLATE(RightAction, T, group::set<group::C8>::type)
{
        is_right_action<T, group::C1>()();
        is_right_action<T, group::C2>()();
        is_right_action<T, group::C4>()();
        is_right_action<T, group::C8>()();
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IdemPotentInverse, T, group::set<group::C8>::type)
{
        BOOST_CHECK((is_idempotent< inverse<_1> , T>::value));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IdemPotentMirrorUp, T, group::set<group::C8>::type)
{
        BOOST_CHECK((is_idempotent< mirror_up<_1>, T>::value));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(IdemPotentMirrorDown, T, group::set<group::C8>::type)
{
        BOOST_CHECK((is_idempotent< mirror_down<_1>, T>::value));
}

BOOST_AUTO_TEST_SUITE_END()

}       // namespace dctl
