#include <dctl/rules/russian.hpp>       // Russian
#include <dctl/rule_traits.hpp>         // is_backward_pawn, is_jump_precedence, is_long_ranged_king
#include <boost/test/unit_test.hpp>     // BOOST_AUTO_TEST_SUITE, BOOST_AUTO_TEST_CASE, BOOST_AUTO_TEST_SUITE_END

namespace dctl {
namespace russian {

BOOST_AUTO_TEST_SUITE(RulesVariantsRussian)

using T = Rules;

BOOST_AUTO_TEST_CASE(Traits)
{
        // required
        static_assert(is_backward_pawn_jump_v<T>, "");
        static_assert(!is_jump_precedence_v<T>, "");
        static_assert(is_long_ranged_king_v<T>, "");

        // optional
        static_assert(std::is_same<promotion_category_t<T>, passing_promotion_tag>::value, "");
}

BOOST_AUTO_TEST_SUITE_END()

}       // namespace russian
}       // namespace dctl
