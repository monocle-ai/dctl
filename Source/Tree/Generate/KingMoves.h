#pragma once
#include "Template.h"
#include "../Move/Types.h"
#include "../../Utilities/IntegerTypes.h"
#include "../../Utilities/TemplateTricks.h"
#include "../../Variants/Rules.h"

namespace Tree {

namespace Node { template<typename> class Position; }
namespace Move { class Stack; }

namespace Generate {

template<bool Color, typename Rules, typename Board> 
class Template<Color, Node::Pieces::KING, Move::MOVES, Rules, Board>
{
public:
        static void generate(const Node::Position<Board>&, Move::Stack&);
        static void generate_promotions(const Node::Position<Board>&, Move::Stack&);

        static size_t count(const Node::Position<Board>&);
        static size_t count_promotions(const Node::Position<Board>&);
        
        static bool detect(const Node::Position<Board>&);
        static bool detect_promotions(const Node::Position<Board>&);

private:
        // implementation
        
        // tag dispatching for restrictions on consecutive moves with the same king
        static void generate_serial(BitBoard, BitBoard, Move::Stack&);
        static void generate_serial(BitBoard, BitBoard, Move::Stack&, Int2Type<true>);
        static void generate_serial(BitBoard, BitBoard, Move::Stack&, Int2Type<false>);
        
        static void generate_dirs(BitBoard, BitBoard, Move::Stack&);
        
        // tag dispatching based on king range
        template<size_t> static void generate_dir(BitBoard, BitBoard, Move::Stack&);
        template<size_t> static void generate_dir(BitBoard, BitBoard, Move::Stack&, Int2Type<Variants::RANGE_1>);
        template<size_t> static void generate_dir(BitBoard, BitBoard, Move::Stack&, Int2Type<Variants::RANGE_N>);

        static size_t count_dirs(BitBoard, BitBoard);
        
        // tag dispatching based on king range
        template<size_t> static size_t count_dir(BitBoard, BitBoard);
        template<size_t> static size_t count_dir(BitBoard, BitBoard, Int2Type<Variants::RANGE_1>);
        template<size_t> static size_t count_dir(BitBoard, BitBoard, Int2Type<Variants::RANGE_N>);

        static bool detect_dirs(BitBoard, BitBoard);
        template<size_t> static bool detect_dir(BitBoard, BitBoard);
};

}       // namespace Generate
}       // namespace Tree

// include template definitions inside header because "export" keyword is not supported by most C++ compilers
#include "KingMoves.hpp"
