#pragma once
#include "PerftNode.h"
#include "../Containers/Adapters/DualHashMap.h"
#include "../Position/Reversible/Pieces.h"
#include "../Search/SearchStatistics.h"
#include "../Utilities/IntegerTypes.h"

template<typename> class Position;

class Perft
{
public:
        template<typename, typename B> static void root(Position<B>&, size_t);

        template<typename, typename B> static NodeCount perft(Position<B>&, size_t, size_t);
        template<typename, typename B> static NodeCount perft_bulk(Position<B>&, size_t, size_t);
        template<typename, typename B> static NodeCount perft_count(Position<B>&, size_t, size_t);
        template<typename, typename B> static NodeCount perft_hash(Position<B>&, size_t, size_t);
        template<typename, typename B> static NodeCount perft_count_hash(Position<B>&, size_t, size_t);

        template<typename B> static void announce(const Position<B>&, size_t);

        static void report(NodeCount, size_t, double, bool = false);

        static NodeCount nodes(void);
        static NodeCount sum_ply(void);
        static size_t max_ply(void);

        static void reset_statistics(void);
        static void update_statistics(void);
        static void update_statistics(size_t);

private:
        // 24-byte piece lists signature, 8-byte (59-bit leafs, 5-bit depth) entries, 2^25 entries (= 1 Gb)
        // depth-preferred replacement, incremental Zobrist hashing, 64-bit indices
        typedef DualHashMap<Pieces, PerftNode, 25> TranspositionTable;

        static TranspositionTable TT;
        static SearchStatistics d_statistics;
};

// include template definitions inside header because "export" keyword is not supported by most C++ compilers
#include "Perft.hpp"
