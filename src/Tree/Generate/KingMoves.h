#pragma once
#include "Driver.h"
#include "../node/Stack.h"
#include "../../utils/IntegerTypes.h"
#include "../../utils/TemplateTricks.h"
#include "../../rules/Rules.h"

namespace tree {

template<typename> class Position;

namespace generate {

template<bool Color, typename Rules, typename Board> 
class Driver<Color, Pieces::KING, MOVES, Rules, Board>
{
public:
        static void generate(const Position<Board>&, Stack&);
        static void generate_reverse(const Position<Board>&, Stack&);
        static void generate_promotions(const Position<Board>&, Stack&);

        static size_t count(const Position<Board>&);
        static size_t count_reverse(const Position<Board>&);
        static size_t count_promotions(const Position<Board>&);
        
        static bool detect(const Position<Board>&);
        static bool detect_reverse(const Position<Board>&);
        static bool detect_promotions(const Position<Board>&);

private:
        // implementation
        
        // tag dispatching based on restrictions on consecutive moves with the same king
        static void generate_serial(BitBoard, BitBoard, Stack&);
        static void generate_serial(BitBoard, BitBoard, Stack&, Int2Type<false>);
        static void generate_serial(BitBoard, BitBoard, Stack&, Int2Type<true >);
        
        static void generate_dirs(BitBoard, BitBoard, Stack&);
        
        // tag dispatching based on king range
        template<size_t> static void generate_dir(BitBoard, BitBoard, Stack&);
        template<size_t> static void generate_dir(BitBoard, BitBoard, Stack&, Int2Type<rules::RANGE_1>);
        template<size_t> static void generate_dir(BitBoard, BitBoard, Stack&, Int2Type<rules::RANGE_N>);

        static size_t count_dirs(BitBoard, BitBoard);
        
        // tag dispatching based on king range
        template<size_t> static size_t count_dir(BitBoard, BitBoard);
        template<size_t> static size_t count_dir(BitBoard, BitBoard, Int2Type<rules::RANGE_1>);
        template<size_t> static size_t count_dir(BitBoard, BitBoard, Int2Type<rules::RANGE_N>);

        static bool detect_dirs(BitBoard, BitBoard);
        template<size_t> static bool detect_dir(BitBoard, BitBoard);
};

}       // namespace generate
}       // namespace tree

// include template definitions inside header because "export" keyword is not supported by most C++ compilers
#include "KingMoves.hpp"
