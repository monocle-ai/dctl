#pragma once
#include <array>                        // array
#include <cassert>                      // assert
#include <cstddef>                      // ptrdiff_t, size_t
#include <dctl/utility/make_array.hpp>  // make_array

namespace dctl {
namespace egdb {

template
<
        int BoardSize,
        int MaxPieces
>
class Binomial
{
private:
        static constexpr auto MaxN = 384;
        static constexpr auto N = (BoardSize < MaxN) ? BoardSize : MaxN;

        static constexpr auto MaxK = (N + 1) / 2;
        static constexpr auto K = (MaxPieces < MaxK) ? MaxPieces : MaxK;

        using value_type = std::ptrdiff_t;
        using row_type = std::array<value_type, 1 + K>;
        using table_type = std::array<row_type, 1 + N>;

        static constexpr value_type choose(int n, int k) noexcept
        {
                if (n - k < k)  k = n - k;      // choose the smallest
                if (k < 0)      return 0;       // by symmetry, covers n < k
                if (k == 0)     return 1;       // by symmetry, covers n == k

                // We need choose(n, k) with k in [0, (n + 1) / 2].
                // The 12x10 board has 60 squares, and the central coefficient
                // choose(60, 30) just fits into 64-bit signed integers.
                // For even larger boards, we need to limit the range of k:
                if (
                        // the 12x12 board has 72 squares
                        // choose(72, 20) just fits
                        (61 <= n && n < 73 && 20 < k) ||

                        // the 10x19 board has 95 squares
                        // choose(95, 16) just fits
                        (73 <= n && n < 96 && 16 < k) ||

                        // the 14x14 board has 98 squares
                        // choose(98, 15) just fits
                        (96 <= n && n < 99 && 15 < k) ||

                        // choose(128, 13) just fits
                        (99 <= n && n < 129 && 13 < k) ||

                        // choose(256, 10) just fits
                        (129 <= n && n < 257 && 10 < k) ||

                        // choose(384, 9) just fits
                        (257 <= n && n <= MaxN && 9 < k) ||

                        // beyond this, we reach compiler limits for constexpr
                        // and overriding those would blow up compilation time
                        (MaxN < n)
                )
                        return -1;

                // O(K) complexity, with O(N*K) for Pascal's Triangle method
                // slight drawback: we loose a factor of K for 64-bit overflow
                return (n * choose(n - 1, k - 1)) / k;
        }

        // simulate a constexpr lambda (not allowed in C++14)
        struct lambda
        {
                int n;

                constexpr auto operator()(int k) noexcept
                {
                        return choose(n, k);
                }
        };

        static constexpr auto init(int n)
        {
                return make_array<1 + K>(lambda{n});
        }

        static constexpr table_type table = make_array<1 + N>(init);

public:
        static value_type coefficient(int n, int k)
        {
                assert(0 <= n && 0 <= k);

                if (n - k < k)          k = n - k;
                if (k < 0)              return 0;
                if (N < n || K < k)     return -1;

                return table[static_cast<std::size_t>(n)][static_cast<std::size_t>(k)];
        }
};

template<int N, int K>
constexpr typename Binomial<N, K>::table_type
Binomial<N, K>::table;

}       // namespace egdb
}       // namespace dctl
