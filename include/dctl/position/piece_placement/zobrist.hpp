#pragma once
#include <dctl/random/uniform.hpp>      // runif
#include <array>                        // array
#include <cstddef>                      // size_t
#include <cstdint>                      // uint64_t

namespace dctl {
namespace zobrist {

template<std::size_t N>
struct PiecePlacement
{
        static std::array<uint64_t, N> const by_color[2];
        static std::array<uint64_t, N> const by_piece[2];
};

template<std::size_t N>
std::array<uint64_t, N> const
PiecePlacement<N>::by_color[2] =
{
        random::runif<N>(),
        random::runif<N>()
};

template<std::size_t N>
std::array<uint64_t, N> const
PiecePlacement<N>::by_piece[2] =
{
        random::runif<N>(),
        random::runif<N>()
};

}       // namespace zobrist
}       // namespace dctl
