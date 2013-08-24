#pragma once

namespace dctl {
namespace grid {
namespace detail {

template<class T>
constexpr auto centralize(T const& t, T const& c) noexcept
{
        return 2 * t - (c - 1);
}

template<class T>
constexpr auto decentralize(T const& t, T const& c) noexcept
{
        return (t + (c - 1)) / 2;
}

}       // namespace detail

template<class Grid>
class Square
{
public:
        explicit constexpr Square(int v) noexcept
        :
                value_(v)
        {}

        constexpr auto value () const noexcept
        {
                return value_;
        }
private:
        int value_;
};

class Coordinates
{
public:
        constexpr Coordinates(int r, int c) noexcept
        :
                row_(r),
                col_(c)
        {}

        constexpr auto row() const noexcept
        {
                return row_;
        }

        constexpr auto col() const noexcept
        {
                return col_;
        }
private:
        int row_;
        int col_;
};

template<class Grid>
constexpr auto sqtocoord(Square<Grid> const& square) noexcept
{
        // number of row pairs
        auto const Q = square.value() / Grid::modulo;

        // left edge of the zeroth row
        auto const R0 = square.value() % Grid::modulo;

        // left edge of the first row
        auto const R1 = R0 - Grid::edge_lo;

        // R0 is in the zeroth or first row
        auto const P = R1 >= 0;

        // squares from the left edge
        auto const R = P? R1 : R0;

        // 2x the row pairs + the row parity
        auto const ROW = 2 * Q + P;

        // 2x the range from the left edge + the row parity XOR the opposite board coloring
        auto const COL = 2 * R + (P ^ !Grid::parity);

        return Coordinates
        {
                detail::centralize(ROW, Grid::height),
                detail::centralize(COL, Grid::width)
        };
}

template<class Grid>
constexpr auto coordtosq(Coordinates const& coord)
{
        auto const DECENTRAL_ROW = detail::decentralize(coord.row(), Grid::height);
        auto const DECENTRAL_COL = detail::decentralize(coord.col(), Grid::width);

        // row parity
        auto const P = DECENTRAL_ROW % 2;

        // number of row pairs
        auto const Q = DECENTRAL_ROW / 2;

        // the left edge
        auto const L = P? Grid::edge_lo : Grid::edge_le;

        // number of column pairs
        auto const S = DECENTRAL_COL / 2;

        // squares from the left edge
        auto const R = (L + S) % Grid::modulo;

        auto const NUM = Grid::modulo * Q + R;

        return Square<Grid>{ NUM };
}

}       // namespace grid
}       // namespace dctl
