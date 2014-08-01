#include <dctl/rule_traits/irregular/is_absolute_king_jump_precedence.hpp>      // is_absolute_king_jump_precedence_v
#include <dctl/rules.hpp>
#include <boost/mpl/vector.hpp>                                                 // vector
#include <boost/test/test_case_template.hpp>                                    // BOOST_AUTO_TEST_CASE_TEMPLATE
#include <boost/test/unit_test.hpp>                                             // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_SUITE_END

namespace dctl {
namespace rules {

BOOST_AUTO_TEST_SUITE(RulesTraitsIsAbsoluteKingJumpPrecedence)

using VariantsFalse = boost::mpl::vector<
        Checkers, Frisian, International, Italian, Pool, Russian, Spanish, Thai
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(NotIsAbsoluteKingJumpPrecedenceV, T, VariantsFalse)
{
        static_assert(!is_absolute_king_jump_precedence_v<T>, "");
}

using VariantsTrue = boost::mpl::vector<
        Czech
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(IsAbsoluteKingJumpPrecedenceV, T, VariantsTrue)
{
        static_assert(is_absolute_king_jump_precedence_v<T>, "");
}

BOOST_AUTO_TEST_SUITE_END()

}       // namespace rules
}       // namespace dctl
