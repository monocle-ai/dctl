#pragma once
#include <boost/test/unit_test.hpp>	// BOOST_CHECK_EQUAL
#include <dctl/walk/root.hpp>           // Root
#include <dctl/utility/int.hpp>        	// NodeCount

namespace dctl {
namespace walk {

template<typename Position, int N>
void test(Position const& p, NodeCount const (&leafs)[N])
{
        typedef hash_tag impl_tag;
        Data<impl_tag, Position> d;
        Enhancements<impl_tag, Position> e(&d);
        e.resize_TT(1024);

        auto depth = 0;
        for (auto const& node_count: leafs) {
                e.clear_TT();
                BOOST_CHECK_EQUAL(node_count, walk(p, ++depth, 0, e));
        }
}

}       // namespace walk
}       // namespace dctl
