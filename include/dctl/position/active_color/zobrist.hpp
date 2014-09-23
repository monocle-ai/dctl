#pragma once
#include <array>                        // array
#include <cstdint>                      // uint64_t
#include <dctl/random/uniform.hpp>      // cat_zero_runif

namespace dctl {
namespace random {

template<class = void>
struct ActiveColor
{
        static std::array<uint64_t, 2> const table_;
};

template<class _>
std::array<uint64_t, 2> const
ActiveColor<_>::table_ = cat_zero_runif<1>();

}       // namespace random
}       // namespace dctl
