#pragma once
#include <dctl/bit/detail/base_set.hpp>         // BaseSet
#include <dctl/bit/iterator/iterator.hpp>       // ConstIterator
#include <dctl/bit/iterator/reference.hpp>      // ConstReference
#include <boost/range/concepts.hpp>             // BOOST_CONCEPT_ASSERKey, SinglePassRangeConcept
#include <cassert>                              // assert
#include <cstddef>                              // ptrdiff_t, size_t
#include <functional>                           // less
#include <initializer_list>                     // initializer_list
#include <iterator>                             // iterator_traits
#include <limits>                               // digits
#include <type_traits>                          // is_convertible

namespace dctl {
namespace bit {

template<class Block>
constexpr auto num_blocks(std::size_t N)
{
        return (N - 1) / std::numeric_limits<Block>::digits + 1;
}

template<std::size_t N>
class Set
:
        private detail::BaseSet<std::size_t, num_blocks<std::size_t>(N)>
{
public:
        using block_type = std::size_t;
        static constexpr auto Nb = num_blocks<block_type>(N);
        using Base = detail::BaseSet<block_type, Nb>;

        using key_type = int;
        using value_type = int;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = ConstReference<block_type, Nb>;
        using const_reference = reference;
        using iterator = ConstIterator<block_type, Nb>;
        using const_iterator = iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // constructors

        constexpr Set() = default;

        template<class InputIt>
        constexpr Set(InputIt first, InputIt last)
        {
                insert(first, last);
        }

        constexpr Set(std::initializer_list<value_type> ilist)
        {
                insert(ilist.begin(), ilist.end());
        }

        template<class Range>
        explicit Set(Range&& rng)
        {
                BOOST_CONCEPT_ASSERT(( boost::SinglePassRangeConcept<Range> ));
                insert(boost::begin(std::forward<Range>(rng)), boost::end(std::forward<Range>(rng)));
        }

        constexpr auto& operator=(std::initializer_list<value_type> ilist)
        {
                clear();
                insert(ilist);
                return *this;
        }

        constexpr auto* data()
        {
                return Base::data();
        }

        constexpr auto const* data() const
        {
                return Base::data();
        }

        // iterators

        constexpr auto begin() noexcept
        {
                return iterator{this->block_ptr(0)};
        }

        constexpr auto begin() const noexcept
        {
                return const_iterator{this->block_ptr(0)};
        }

        constexpr iterator end() noexcept
        {
                return { this->block_ptr(N), N };
        }

        constexpr const_iterator end() const noexcept
        {
                return { this->block_ptr(N), N };
        }

        /* constexpr */ auto rbegin() noexcept
        {
                return reverse_iterator{end()};
        }

        /* constexpr */ auto rbegin() const noexcept
        {
                return const_reverse_iterator{end()};
        }

        /* constexpr */ auto rend() noexcept
        {
                return reverse_iterator{begin()};
        }

        /* constexpr */ auto rend() const noexcept
        {
                return const_reverse_iterator{begin()};
        }

        constexpr auto cbegin() const noexcept
        {
                return begin();
        }

        constexpr auto cend() const noexcept
        {
                return end();
        }

        /* constexpr */ auto crbegin() const noexcept
        {
                return rbegin();
        }

        /* constexpr */ auto crend() const noexcept
        {
                return rend();
        }

        // capacity

        constexpr auto empty() const noexcept
        {
                return this->do_none();
        }

        constexpr auto size() const noexcept
        {
                return this->do_count();
        }

        static constexpr auto max_size() noexcept
        {
                return N;
        }

        constexpr auto count() const noexcept
        {
                return this->do_count();
        }

        // modifiers

        constexpr std::pair<iterator, bool> insert(value_type const& value)
        {
                auto const not_set = !is_mask(value);
                if (not_set)
                        set(value);
                return { { this->block_ptr(value), value }, not_set };
        }

        constexpr iterator insert(const_iterator /*hint*/, value_type const& value)
        {
                set(value);
                return { this->block_ptr(value), value };
        }

        template<class InputIt>
        constexpr void insert(InputIt first, InputIt last)
        {
                static_assert(std::is_convertible<decltype(*first), key_type>::value, "");
                for (auto it = first; it != last; ++it)
                        set(static_cast<key_type>(*it));
        }

        constexpr void insert(std::initializer_list<value_type> ilist)
        {
                insert(ilist.begin(), ilist.end());
        }

        template<class... Args>
        constexpr auto emplace(Args&&... args)
        {
                return insert(std::forward<Args>(args)...);
        }

        template <class... Args>
        constexpr auto emplace_hint(const_iterator position, Args&&... args)
        {
                return insert(position, std::forward<Args>(args)...);
        }

        constexpr auto erase(key_type const& key)
        {
                auto const n = count(key);
                reset(key);
                return n;
        }

        constexpr auto erase(const_iterator pos)
        {
                reset(*pos++);
                return pos;
        }

        constexpr auto erase(const_iterator first, const_iterator last)
        {
                auto it = first;
                while (it != last)
                        reset(*it++);
                return it;
        }

        auto swap(Set& other) noexcept
        {
                this->do_swap(other);
        }

        constexpr auto clear() noexcept
        {
                this->do_reset();
        }

        constexpr auto count(key_type const& key) const
        {
                return static_cast<size_type>(is_mask(key));
        }

        constexpr auto find(key_type const& key)
        {
                auto const found = is_mask(key);
                return found ? iterator{this->block_ptr(key), key} : end();
        }

        constexpr auto find(key_type const& key) const
        {
                auto const found = is_mask(key);
                return found ? const_iterator{this->block_ptr(key), key} : cend();
        }

        // element access

        constexpr reference operator[](key_type n)
        {
                return { block_ref(n), n };
        }

        constexpr auto operator[](key_type n) const
        {
                return is_mask(n);
        }

        constexpr auto test(key_type i) const noexcept
        {
                auto const n = static_cast<std::size_t>(i);
                return 0 <= n && n < N && is_mask(n);
        }

        constexpr auto& set(key_type i, bool value = true)
        {
                auto const n = static_cast<std::size_t>(i);
                if (value)
                        block_ref(n) |= mask(n);
                else
                        block_ref(n) &= ~mask(n);
                return *this;
        }

        constexpr auto& reset(key_type i)
        {
                auto const n = static_cast<std::size_t>(i);
                block_ref(n) &= ~mask(n);
                return *this;
        }

        constexpr auto& flip(key_type i)
        {
                auto const n = static_cast<std::size_t>(i);
                block_ref(n) ^= mask(n);
                return *this;
        }

        // comparators

        friend constexpr auto operator==(Set const& lhs, Set const& rhs) noexcept
        {
                return lhs.do_equal(rhs);
        }

        friend constexpr auto operator!=(Set const& lhs, Set const& rhs) noexcept
        {
                return !(lhs == rhs);
        }

        friend constexpr auto operator< (Set const& lhs, Set const& rhs) noexcept
        {
                return lhs.do_colexicographical_compare(rhs);
        }

        friend constexpr auto operator>=(Set const& lhs, Set const& rhs) noexcept
        {
                return !(lhs < rhs);
        }

        friend constexpr auto operator> (Set const& lhs, Set const& rhs) noexcept
        {
                return rhs < lhs;
        }

        friend constexpr auto operator<=(Set const& lhs, Set const& rhs) noexcept
        {
                return !(rhs < lhs);
        }

        constexpr auto is_proper_subset_of(Set const& other) const noexcept
        {
                return this->do_is_proper_subset_of(other);
        }

        constexpr auto is_proper_superset_of(Set const& other) const noexcept
        {
                return other.is_proper_subset_of(*this);
        }

        constexpr auto is_subset_of(Set const& other) const noexcept
        {
                return this->do_is_subset_of(other);
        }

        constexpr auto is_superset_of(Set const& other) const noexcept
        {
                return other.is_subset_of(*this);
        }

        friend constexpr auto intersect(Set const& lhs, Set const& rhs) noexcept
        {
                return lhs.do_intersects(rhs);
        }

        friend constexpr auto disjoint(Set const& lhs, Set const& rhs) noexcept
        {
                return !intersect(lhs, rhs);
        }

        // modifiers

        constexpr auto& set() noexcept
        {
                this->do_set();
                return *this;
        }

        constexpr auto& reset() noexcept
        {
                this->do_reset();
                return *this;
        }

        constexpr auto& flip() noexcept
        {
                this->do_flip();
                return *this;
        }

        constexpr auto& operator&=(Set const& rhs) noexcept
        {
                this->do_and(rhs);
                return *this;
        }

        constexpr auto& operator|=(Set const& rhs) noexcept
        {
                this->do_or(rhs);
                return *this;
        }

        constexpr auto& operator^=(Set const& rhs) noexcept
        {
                this->do_xor(rhs);
                return *this;
        }

        constexpr auto& operator-=(Set const& rhs) noexcept
        {
                this->do_minus(rhs);
                return *this;
        }

        constexpr auto& operator<<=(size_type n)
        {
                assert(n < N);
                this->do_left_shift(n);
                return *this;
        }

        constexpr auto& operator>>=(size_type n)
        {
                assert(n < N);
                this->do_right_shift(n);
                return *this;
        }

        friend constexpr auto operator~(Set const& lhs) noexcept
        {
                auto nrv(lhs);
                nrv.flip();
                return nrv;
        }

        friend constexpr auto operator&(Set const& lhs, Set const& rhs) noexcept
        {
                auto nrv(lhs);
                nrv &= rhs;
                return nrv;
        }

        friend constexpr auto operator|(Set const& lhs, Set const& rhs) noexcept
        {
                auto nrv(lhs);
                nrv |= rhs;
                return nrv;
        }

        friend constexpr auto operator^(Set const& lhs, Set const& rhs) noexcept
        {
                auto nrv(lhs);
                nrv ^= rhs;
                return nrv;
        }

        friend constexpr auto operator-(Set const& lhs, Set const& rhs) noexcept
        {
                auto nrv(lhs);
                nrv -= rhs;
                return nrv;
        }

        friend constexpr auto operator<<(Set const& lhs, size_type n)
        {
                assert(n < N);
                auto nrv(lhs);
                nrv <<= n;
                return nrv;
        }

        friend constexpr auto operator>>(Set const& lhs, size_type n)
        {
                assert(n < N);
                auto nrv(lhs);
                nrv >>= n;
                return nrv;
        }

        // observers

        constexpr auto all() const noexcept
        {
                return this->do_all();
        }

        constexpr auto any() const noexcept
        {
                return this->do_any();
        }

        constexpr auto none() const noexcept
        {
                return this->do_none();
        }

private:
        constexpr auto& block_ref(std::size_t n)
        {
                return *(this->block_ptr(n));
        }

        constexpr auto const& block_ref(std::size_t n) const
        {
                return *(this->block_ptr(n));
        }

        static constexpr auto mask(std::size_t n)
        {
                return static_cast<block_type>(1) << (n % std::numeric_limits<block_type>::digits);
        }

        constexpr auto is_mask(std::size_t n) const
        {
                return (block_ref(n) & mask(n)) != block_type{0};
        }
};

template<std::size_t N>
auto swap(Set<N>& lhs, Set<N>& rhs) noexcept
{
        lhs.swap(rhs);
}

template<std::size_t N>
constexpr auto begin(Set<N> const& s) noexcept
{
        return s.begin();
}

template<std::size_t N>
constexpr auto end(Set<N> const& s) noexcept
{
        return s.end();
}

template<std::size_t N>
constexpr auto cbegin(Set<N> const& s) noexcept
{
        return begin(s);
}

template<std::size_t N>
constexpr auto cend(Set<N> const& s) noexcept
{
        return end(s);
}

template<std::size_t N>
/* constexpr */ auto rbegin(Set<N> const& s) noexcept
{
        return s.rbegin();
}

template<std::size_t N>
/* constexpr */ auto rend(Set<N> const& s) noexcept
{
        return s.rend();
}

template<std::size_t N>
/* constexpr */ auto crbegin(Set<N> const& s) noexcept
{
        return rbegin(s);
}

template<std::size_t N>
/* constexpr */ auto crend(Set<N> const& s) noexcept
{
        return rend(s);
}

}       // namespace bit
}       // namespace dctl
