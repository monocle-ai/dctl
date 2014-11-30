#pragma once
#include <dctl/board/mask.hpp>
#include <dctl/color.hpp>
#include <dctl/position/piece_placement/zobrist.hpp>
#include <dctl/set_type.hpp>
#include <dctl/zobrist/accumulate.hpp>
#include <cassert>                      // assert

namespace dctl {

template<class Rules, class Board>
class PiecePlacement
{
        using Set = set_type<Board>;

        Set pieces_[2];
        Set kings_;

        bool invariant() const
        {
                auto constexpr squares = board::Squares<Board>::mask();
                return
                         kings().is_subset_of(pieces()) &&
                         pieces().is_subset_of(squares) &&
                         disjoint(pieces(Color::black), pieces(Color::white))
                ;
        }

public:
        PiecePlacement() = default;

        PiecePlacement(Set black_pieces, Set white_pieces, Set kings)
        :
                pieces_{black_pieces, white_pieces},
                kings_{kings}
        {
                assert(invariant());
        }

        PiecePlacement(Set black_pawns, Set black_kings, Set white_pawns, Set white_kings)
        :
                pieces_{black_pawns | black_kings, white_pawns | white_kings},
                kings_{black_kings | white_kings}
        {
                assert(invariant());
        }

        // modifiers

        template<class Move, class Index>
        void make(Move const& m, Index& hash)
        {
                using Zobrist = zobrist::PiecePlacement<set_type<Board>::size()>;
                pieces_[static_cast<bool>(m.to_move())].reset(m.from());
                pieces_[static_cast<bool>(m.to_move())].set(m.dest());
                hash ^= Zobrist::pieces[static_cast<bool>(m.to_move())][m.from()];
                hash ^= Zobrist::pieces[static_cast<bool>(m.to_move())][m.dest()];
                if (m.is_with_king()) {
                        kings_.reset(m.from());
                        kings_.set(m.dest());
                        hash ^= Zobrist::kings[m.from()];
                        hash ^= Zobrist::kings[m.dest()];
                } else if (m.is_promotion()) {
                        kings_.set(m.dest());
                        hash ^= Zobrist::kings[m.dest()];
                }
                if (m.is_jump()) {
                        pieces_[static_cast<bool>(!m.to_move())] ^= m.captured_pieces();
                        kings_ ^= m.captured_kings();
                        hash ^= zobrist::hash_xor_accumulate(Zobrist::pieces[static_cast<bool>(!m.to_move())], m.captured_pieces());
                        hash ^= zobrist::hash_xor_accumulate(Zobrist::kings                    , m.captured_kings() );
                }
                assert(invariant());
        }

        // observers

        auto kings(Color c) const
        {
                return pieces_[static_cast<bool>(c)] & kings_;
        }

        auto pawns(Color c) const
        {
                return pieces_[static_cast<bool>(c)] - kings_;
        }

        auto pieces(Color c) const
        {
                return pieces_[static_cast<bool>(c)];
        }

        auto kings() const
        {
                return kings_;
        }

        auto pawns() const
        {
                return pieces() - kings_;
        }

        auto pieces() const
        {
                return pieces_[static_cast<bool>(Color::black)] | pieces_[static_cast<bool>(Color::white)];
        }

        auto not_occupied() const
        {
                auto constexpr squares = board::Squares<Board>::mask();
                return squares ^ pieces();
        }
};

template<class TabulationHash, class Rules, class Board>
auto hash_xor_accumulate(TabulationHash const& h, PiecePlacement<Rules, Board> const& p)
{
        return
                zobrist::hash_xor_accumulate(h.pieces[static_cast<bool>(Color::black)], p.pieces(Color::black)) ^
                zobrist::hash_xor_accumulate(h.pieces[static_cast<bool>(Color::white)], p.pieces(Color::white)) ^
                zobrist::hash_xor_accumulate(h.kings               , p.kings()             )
        ;
}

}       // namespace dctl
