#pragma once
#include <xstd/pp/tti.hpp>      // XSTD_PP_TTI_CONSTANT, XSTD_PP_TTI_TYPENAME
#include <type_traits>          // integral_constant

namespace dctl {
namespace precedence {
namespace trivial {

struct equal_to
{
        template<class Move>
        constexpr auto operator()(Move const&, Move const&) const noexcept
        {
                return true;
        }
};

struct less
{
        template<class Move>
        constexpr auto operator()(Move const&, Move const&) const noexcept
        {
                return false;
        }
};

}       // namespace trivial

XSTD_PP_TTI_TYPENAME(equal_to, trivial::equal_to)
XSTD_PP_TTI_TYPENAME(less    , trivial::less    )

template<class Rules>
struct not_equal_to_t
{
        template<class Move>
        constexpr auto operator()(Move const& lhs, Move const& rhs) const noexcept
        {
                return !equal_to_t<Rules>{}(lhs, rhs);
        }
};

template<class Rules>
struct greater_t
{
        template<class Move>
        constexpr auto operator()(Move const& lhs, Move const& rhs) const noexcept
        {
                return less_t<Rules>{}(rhs, lhs);
        }
};

template<class Rules>
struct greater_equal_t
{
        template<class Move>
        constexpr auto operator()(Move const& lhs, Move const& rhs) const noexcept
        {
                return !less_t<Rules>{}(lhs, rhs);
        }
};

template<class Rules>
struct less_equal_t
{
        template<class Move>
        constexpr auto operator()(Move const& lhs, Move const& rhs) const noexcept
        {
                return !less_t<Rules>{}(rhs, lhs);
        }
};

template<class Rules>
constexpr auto is_trivial_v = !is_less_v<Rules>;

template<class Rules>
using is_trivial = std::integral_constant<bool, is_trivial_v<Rules>>;

XSTD_PP_TTI_CONSTANT(is_king_order, false)
XSTD_PP_TTI_CONSTANT(is_absolute_with_king, false)

}       // namespace precedence
}       // namespace dctl