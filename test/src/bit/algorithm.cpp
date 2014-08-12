#include <dctl/bit.hpp>                         // Set, set_intersection, set_union, set_symmetric_difference, set_difference
#include <boost/mpl/vector.hpp>                 // vector
#include <boost/test/unit_test.hpp>             // BOOST_AUTO_TEST_SUITE, BOOST_CHECK, BOOST_CHECK_EQUAL, BOOST_CHECK_EQUAL_COLLECTIONS, BOOST_AUTO_TEST_SUITE_END
#include <boost/test/test_case_template.hpp>    // BOOST_AUTO_TEST_CASE_TEMPLATE

namespace dctl {
namespace bit {

BOOST_AUTO_TEST_SUITE(BitAlgorithm)

using SetTypes = boost::mpl::vector
<
        Set< 64>,
        Set<128>,
        Set<192>,
        Set<256>
>;

BOOST_AUTO_TEST_CASE_TEMPLATE(SetEmpty, T, SetTypes)
{
        constexpr auto b = T{};
        BOOST_CHECK(b.empty());
        BOOST_CHECK(!set_single(b));
        BOOST_CHECK(!set_double(b));
        BOOST_CHECK(!set_multiple(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SetSingle, T, SetTypes)
{
        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                auto const b = T{i};
                BOOST_CHECK(!b.empty());
                BOOST_CHECK( set_single(b));
                BOOST_CHECK(!set_double(b));
                BOOST_CHECK(!set_multiple(b));
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SetDouble, T, SetTypes)
{
        for (auto i = 0; i < std::numeric_limits<T>::digits; ++i) {
                for (auto j = 0; j < std::numeric_limits<T>::digits; ++j) {
                        auto const b = T{i, j};
                        if (i == j) {
                                BOOST_CHECK(b.empty());
                        } else {
                                BOOST_CHECK(!b.empty());
                                BOOST_CHECK(!set_single(b));
                                BOOST_CHECK( set_double(b));
                                BOOST_CHECK( set_multiple(b));
                        }
                }
        }
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SetMultiple, T, SetTypes)
{
        for (std::size_t i = 0; i < static_cast<std::size_t>(std::numeric_limits<T>::digits); ++i) {
                auto const b = ~((~T{} >> i) << i);
                switch (i) {
                case 0:
                        BOOST_CHECK(b.empty());
                        break;
                case 1:
                        BOOST_CHECK(set_single(b));
                        break;
                case 2:
                        BOOST_CHECK(set_double(b));
                        BOOST_CHECK(set_multiple(b));
                        break;
                default:
                        BOOST_CHECK(!b.empty());
                        BOOST_CHECK(!set_single(b));
                        BOOST_CHECK(!set_double(b));
                        BOOST_CHECK( set_multiple(b));
                        break;
                }
        }
        BOOST_CHECK(set_multiple(~T{}));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SetIntersection, T, SetTypes)
{
        constexpr auto a = T { 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        constexpr auto b = T { 0, 1, 3, T::max_size() - 4, T::max_size() - 3 };

        constexpr auto i_ab = a & b;
        constexpr auto i_ba = b & a;
        BOOST_CHECK_EQUAL_COLLECTIONS(begin(i_ab), end(i_ab), begin(i_ba), end(i_ba));

        BOOST_CHECK(i_ab.is_subset_of(a));
        BOOST_CHECK(i_ab.is_subset_of(b));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SetUnion, T, SetTypes)
{
        constexpr auto a = T { 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        constexpr auto b = T { 0, 1, 3, T::max_size() - 4, T::max_size() - 3 };

        constexpr auto u_ab = a | b;
        constexpr auto u_ba = b | a;
        BOOST_CHECK_EQUAL_COLLECTIONS(begin(u_ab), end(u_ab), begin(u_ba), end(u_ba));

        BOOST_CHECK(a.is_subset_of(u_ab));
        BOOST_CHECK(b.is_subset_of(u_ab));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SetSymmetricDifference, T, SetTypes)
{
        constexpr auto a = T { 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        constexpr auto b = T { 0, 1, 3, T::max_size() - 4, T::max_size() - 3 };

        constexpr auto s_ab = a ^ b;
        constexpr auto s_ba = b ^ a;
        BOOST_CHECK_EQUAL_COLLECTIONS(begin(s_ab), end(s_ab), begin(s_ba), end(s_ba));

        constexpr auto u = (a - b) | (b - a);
        BOOST_CHECK_EQUAL_COLLECTIONS(begin(s_ab), end(s_ab), begin(u), end(u));

        constexpr auto d = (a | b) - (a & b);
        BOOST_CHECK_EQUAL_COLLECTIONS(begin(s_ab), end(s_ab), begin(d), end(d));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SetDifference, T, SetTypes)
{
        constexpr auto a = T { 0, 1, 2, T::max_size() - 2, T::max_size() - 1 };
        constexpr auto b = T { 0, 1, 3, T::max_size() - 4, T::max_size() - 3 };

        constexpr auto d_ab = a - b;
        constexpr auto d_ba = b - a;

        BOOST_CHECK( d_ab.is_subset_of(a));
        BOOST_CHECK(!d_ba.is_subset_of(a));
        BOOST_CHECK( d_ba.is_subset_of(b));
        BOOST_CHECK(!d_ab.is_subset_of(b));
}

BOOST_AUTO_TEST_SUITE_END()

}       // namespace bit
}       // namespace dctl
