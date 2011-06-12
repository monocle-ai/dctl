#include "Template.h"
#include "BothJumps.h"
#include "BothMoves.h"
#include "KingJumps.h"
#include "KingMoves.h"
#include "PawnJumps.h"
#include "PawnMoves.h"

namespace tree {
namespace generate {

template<bool Color, int Material, typename Rules, typename Board> 
void State<Color, Material, Rules, Board>::generate(const node::Position<Board>& p, move::Stack* move_stack) const 
{
        Template<Color, Material, move::JUMPS, Rules, Board>::generate(p, move_stack);
        if (move_stack->empty())
                Template<Color, Material, move::MOVES, Rules, Board>::generate(p, move_stack);
}

template<bool Color, int Material, typename Rules, typename Board> 
void State<Color, Material, Rules, Board>::generate_captures(const node::Position<Board>& p, move::Stack* move_stack) const 
{ 
        Template<Color, Material, move::JUMPS, Rules, Board>::generate(p, move_stack);
}

template<bool Color, int Material, typename Rules, typename Board> 
void State<Color, Material, Rules, Board>::generate_promotions(const node::Position<Board>& p, move::Stack* move_stack) const 
{ 
        Template<Color, Material, move::MOVES, Rules, Board>::generate_promotions(p, move_stack);
}

template<bool Color, int Material, typename Rules, typename Board>
size_t State<Color, Material, Rules, Board>::count(const node::Position<Board>& p) const
{
        size_t num_moves = Template<Color, Material, move::JUMPS, Rules, Board>::count(p);
        if (!num_moves)
                num_moves += Template<Color, Material, move::MOVES, Rules, Board>::count(p);
        return num_moves;
}

template<bool Color, int Material, typename Rules, typename Board>
size_t State<Color, Material, Rules, Board>::count_captures(const node::Position<Board>& p) const
{
        return Template<Color, Material, move::JUMPS, Rules, Board>::count(p);
}

template<bool Color, int Material, typename Rules, typename Board>
size_t State<Color, Material, Rules, Board>::count_promotions(const node::Position<Board>& p) const
{
        return Template<Color, Material, move::MOVES, Rules, Board>::count_promotions(p);
}

template<bool Color, int Material, typename Rules, typename Board>
size_t State<Color, Material, Rules, Board>::count_mobility(const node::Position<Board>& p) const
{
        return Template<Color, Material, move::MOVES, Rules, Board>::count(p);
}

template<bool Color, int Material, typename Rules, typename Board> 
bool State<Color, Material, Rules, Board>::detect(const node::Position<Board>& p) const 
{ 
        return (
                Template<Color, Material, move::MOVES, Rules, Board>::detect(p) || 
                Template<Color, Material, move::JUMPS, Rules, Board>::detect(p)
        );
}

template<bool Color, int Material, typename Rules, typename Board> 
bool State<Color, Material, Rules, Board>::detect_captures(const node::Position<Board>& p) const 
{ 
        return Template<Color, Material, move::JUMPS, Rules, Board>::detect(p);
}

template<bool Color, int Material, typename Rules, typename Board> 
bool State<Color, Material, Rules, Board>::detect_promotions(const node::Position<Board>& p) const 
{
        return Template<Color, Material, move::MOVES, Rules, Board>::detect_promotions(p);
}

}       // namespace generate
}       // namespace tree
