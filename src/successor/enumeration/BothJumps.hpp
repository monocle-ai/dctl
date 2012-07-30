#pragma once
#include "Enumerator_fwd.hpp"
#include "../Select.hpp"
#include "../generation/BothJumps.hpp"
#include "../../node/Material.hpp"
#include "../../node/Stack.hpp"

namespace dctl {
namespace successor {
namespace detail {

template<bool Color, typename Position>
struct enumerator<Color, Material::both, Jumps, Position>
{
private:
        // typedefs

        typedef generator<Color, Material::both, Jumps, Position> BothJumps;

public:
        int operator()(Position const& p)
        {
                Stack moves;
                BothJumps()(p, moves);
                return static_cast<int>(moves.size());
        }
};

}       // namespace detail
}       // namespace successor
}       // namespace dctl
