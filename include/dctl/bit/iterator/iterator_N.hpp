#pragma once
#include <dctl/bit/detail/intrinsic.hpp>        // bsfnz, bsrnz, clznz, ctznz
#include <dctl/bit/iterator/iterator_fwd.hpp>   // ConstIterator
#include <dctl/bit/iterator/reference_fwd.hpp>  // ConstReference
#include <boost/iterator/iterator_facade.hpp>   // iterator_facade
#include <cassert>                              // assert
#include <cstddef>                              // ptrdiff_t, size_t
#include <iterator>                             // bidirectional_iterator_tag
#include <limits>                               // digits
#include <tuple>                                // tie

namespace dctl {
namespace bit {

template<class UnsignedInteger, std::size_t Nb>
class ConstIterator
:
        public boost::iterator_facade
        <
                ConstIterator<UnsignedInteger, Nb>,
                std::size_t const,
                std::bidirectional_iterator_tag,
                ConstReference<UnsignedInteger, Nb>,
                std::ptrdiff_t
        >
{
private:
        static constexpr auto digits = static_cast<std::size_t>(std::numeric_limits<UnsignedInteger>::digits);
        static constexpr auto N = Nb * digits;

public:
        // constructors

        constexpr ConstIterator() = default;

        explicit constexpr ConstIterator(UnsignedInteger const* b)
        :
                block_{b},
                index_{find_first()}
        {}

        constexpr ConstIterator(UnsignedInteger const* b, std::size_t n)
        :
                block_{b},
                index_{n}
        {
                assert(b != nullptr);
                assert(n == N);
        }

private:
        // gateway for boost::iterator_facade to access private implementation
        friend class boost::iterator_core_access;

        constexpr auto find_first()
        {
                assert(block_ != nullptr);
                for (std::size_t i = 0; i < Nb; ++i) {
                        if (auto const mask = *block_)
                                return i * digits + static_cast<std::size_t>(bit::intrinsic::bsfnz(mask));
                        ++block_;
                }
                return N;
        }

        // operator++() and operator++(int) provided by boost::iterator_facade
        constexpr auto increment()
        {
                assert(block_ != nullptr);
                assert(0 <= index_ && index_ < N);
                if (N == ++index_) {
                        ++block_;
                        return;
                }

                auto const idx = index_ % digits;
                if (idx == 0)
                        ++block_;
                if (auto const mask = *block_ >> idx) {
                        index_ += static_cast<std::size_t>(bit::intrinsic::ctznz(mask));
                        return;
                }
                ++block_;

                for (auto i = index_ / digits + 1; i < Nb; ++i) {
                        if (auto const mask = *block_) {
                                index_ = i * digits + static_cast<std::size_t>(bit::intrinsic::bsfnz(mask));
                                return;
                        }
                        ++block_;
                }
                index_ = N;
                assert(0 < index_ && index_ <= N);
        }

        // operator--() and operator--(int) provided by boost::iterator_facade
        constexpr auto decrement()
        {
                assert(block_ != nullptr);
                assert(0 < index_ && index_ <= N);
                if (--index_ == 0)
                        return;

                auto const idx = index_ % digits;
                if (idx == digits - 1)
                        --block_;
                if (auto const mask = *block_ << (digits - 1 - idx)) {
                        index_ -= static_cast<std::size_t>(bit::intrinsic::clznz(mask));
                        return;
                }
                --block_;

                for (auto i = index_ / digits - 1; i >= 0; --i) {
                        if (auto const mask = *block_) {
                                index_ = i * digits + static_cast<std::size_t>(bit::intrinsic::bsrnz(mask));
                                return;
                        }
                        --block_;
                }
                index_ = 0;
                assert(index_ < N);
        }

        // operator* provided by boost::iterator_facade
        constexpr ConstReference<UnsignedInteger, Nb> dereference() const
        {
                assert(block_ != nullptr);
                return { *block_, index_ };
        }

        // operator== and operator!= provided by boost::iterator_facade
        constexpr auto equal(ConstIterator const& other) const noexcept
        {
                return
                        std::tie(      block_,       index_) ==
                        std::tie(other.block_, other.index_)
                ;
        }

private:
        // representation

        UnsignedInteger const* block_{};
        std::size_t index_{};
};

}       // namespace bit
}       // namespace dctl
