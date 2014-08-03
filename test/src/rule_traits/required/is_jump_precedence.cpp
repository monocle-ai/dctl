#include <dctl/rule_traits/required/is_jump_precedence.hpp>     // is_jump_precedence_v
#include <dctl/rules.hpp>                                       // Checkers, Czech, Frisian, International, Italian, Pool, Russian, Spanish, Thai
#include <boost/mpl/vector.hpp>                                 // vector
#include <boost/test/test_case_template.hpp>                    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>                             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END

namespace dctl {
namespace rules {

BOOST_AUTO_TEST_SUITE(RuleTraitsIsJumpPrecedence)

using VariantsFalse = boost::mpl::vector
<
        Checkers, Pool, Russian, Thai
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(NotIsJumpPrecedenceV, T, VariantsFalse)
{
        static_assert(!is_jump_precedence_v<T>, "");
}

using VariantsTrue = boost::mpl::vector
<
        Czech, Frisian, International, Italian, Spanish
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(IsJumpPrecedenceV, T, VariantsTrue)
{
        static_assert(is_jump_precedence_v<T>, "");
}

BOOST_AUTO_TEST_SUITE_END()

}       // namespace rules
}       // namespace dctl
