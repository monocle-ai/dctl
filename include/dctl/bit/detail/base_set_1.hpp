#pragma once
#include <dctl/bit/detail/base_set_fwd.hpp>     // BaseSet
#include <dctl/bit/detail/intrinsic.hpp>        // popcount
#include <dctl/bit/traits.hpp>                  // none, one, all, digits, is_unsigned_integer
#include <cassert>                              // assert
#include <utility>                              // swap

namespace dctl {
namespace bit {
namespace detail {

template<class Block>
class BaseSet<Block, 1>
{
private:
        static_assert(
                is_unsigned_integer<Block>,
                "Template parameter 'T' in 'BaseSet<T, 1>' shall be of unsigned integer type."
        );

        static constexpr auto N = 1 * digits<Block>;

public:
        // constructors

        constexpr BaseSet() = default;

protected:
        // destructor

        ~BaseSet() = default;

public:
        // copying and assignment

        BaseSet(BaseSet const&) = default;
        BaseSet& operator=(BaseSet const&) = default;

        // element access

        constexpr auto* block_ptr(int /* n */)
        {
                return &elems;
        }

        constexpr auto const* block_ptr(int /* n */) const
        {
                return &elems;
        }

        // data access

        constexpr auto* do_data()
        {
                return &elems;
        }

        constexpr auto const* do_data() const
        {
                return &elems;
        }

        // comparators

        constexpr auto do_equal(BaseSet const& other) const noexcept
        {
                return elems == other.elems;
        }

        constexpr auto do_colexicographical_compare(BaseSet const& other) const noexcept
        {
                return elems < other.elems;
        }

        constexpr auto do_intersects(BaseSet const& other) const noexcept
        {
                return (elems & other.elems) != none<Block>;
        }

        constexpr auto do_is_subset_of(BaseSet const& other) const noexcept
        {
                return (elems & ~other.elems) == none<Block>;
        }

        constexpr auto do_is_proper_subset_of(BaseSet const& other) const noexcept
        {
                if (elems & ~other.elems)
                        return false;
                return (~elems & other.elems) != none<Block>;
        }

        // modifiers

        /* constexpr */ auto do_swap(BaseSet& other) noexcept
        {
                using std::swap;
                swap(elems, other.elems);
        }

        constexpr auto do_set() noexcept
        {
                elems = all<Block>;
        }

        constexpr auto do_reset() noexcept
        {
                elems = none<Block>;
        }

        constexpr auto do_flip() noexcept
        {
                elems = ~elems;
        }

        constexpr auto do_and(BaseSet const& other) noexcept
        {
                elems &= other.elems;
        }

        constexpr auto do_or(BaseSet const& other) noexcept
        {
                elems |= other.elems;
        }

        constexpr auto do_xor(BaseSet const& other) noexcept
        {
                elems ^= other.elems;
        }

        constexpr auto do_minus(BaseSet const& other) noexcept
        {
                elems &= ~other.elems;
        }

        constexpr auto do_left_shift(int n)
        {
                assert(0 <= n && n < N);
                elems <<= n;
        }

        constexpr auto do_right_shift(int n)
        {
                assert(0 <= n && n < N);
                elems >>= n;
        }

        // observers

        constexpr auto do_all() const noexcept
        {
                return elems == all<Block>;
        }

        constexpr auto do_any() const noexcept
        {
                return elems != none<Block>;
        }

        constexpr auto do_none() const noexcept
        {
                return elems == none<Block>;
        }

        constexpr auto do_count() const noexcept
        {
                return bit::intrinsic::popcount(elems);
        }

private:
        // representation

        Block elems{};
};

}       // namespace detail
}       // namespace bit
}       // namespace dctl
