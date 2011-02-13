#include "Node.h"
#include "Value.h"
#include <cassert>
#include <sstream>

Tree::Search::Node::Node(void)
:
        value_(0),
        rest_(0)
{
}

Tree::Search::Node::Node(int v, Type t, size_t d, size_t m)
:
        value_(static_cast<int16_t>(v)),
        rest_(0)
{
        rest_ ^= (t & TYPE_MASK) << TYPE_SHIFT;
        rest_ ^= (d & DEPTH_MASK) << DEPTH_SHIFT;
        rest_ ^= (m & MOVE_MASK) << MOVE_SHIFT;
}

int Tree::Search::Node::value(void) const
{
        return value_;
}

int Tree::Search::Node::refined_value(int score) const
{
        return is_cutoff(score)? value() : score;
}

Tree::Search::Node::Type Tree::Search::Node::type(void) const
{
        return static_cast<Type>((rest_ & (TYPE_MASK << TYPE_SHIFT)) >> TYPE_SHIFT);
}

size_t Tree::Search::Node::depth(void) const
{
        return (rest_ & (DEPTH_MASK << DEPTH_SHIFT)) >> DEPTH_SHIFT;
}

size_t Tree::Search::Node::move(void) const
{
        return (rest_ & (MOVE_MASK << MOVE_SHIFT)) >> MOVE_SHIFT;
}

bool Tree::Search::Node::is_sufficient(size_t d) const
{
        return depth() >= d; 
}

bool Tree::Search::Node::is_cutoff(int alpha, int beta) const
{
        return is_fail_low(alpha) || is_fail_high(beta);
}

bool Tree::Search::Node::is_cutoff(int beta) const
{
        return is_cutoff(beta - 1, beta);
}

bool Tree::Search::Node::is_fail_low(int alpha) const
{
        return has_upper() && value() <= alpha;
}

bool Tree::Search::Node::is_fail_high(int beta) const
{
        return has_lower() && value() >= beta;
}

bool Tree::Search::Node::is_finite(void) const
{
        return Value::is_finite(value());
}

bool Tree::Search::Node::is_heuristic(void) const
{
        return Value::is_heuristic(value());
}
bool Tree::Search::Node::has_move(void) const
{
        return move() != no_move();
}

bool Tree::Search::Node::has_lower(void) const
{
        return (type() & lower()) != 0;
}

bool Tree::Search::Node::has_upper(void) const
{
        return (type() & upper()) != 0;
}

bool Tree::Search::Node::is_lower(void) const
{
        return type() == lower();
}

bool Tree::Search::Node::is_upper(void) const
{
        return type() == upper();
}

bool Tree::Search::Node::is_exact(void) const
{
        return type() == exact();
}

bool Tree::Search::Node::invariant(void) const
{
        return is_finite();             
}

std::string Tree::Search::Node::print_value(void) const
{
        std::stringstream sstr(Value::print(value()));
        return sstr.str();
}

std::string Tree::Search::Node::print_type(void) const
{
        std::stringstream sstr;
        switch(type()) {
        case LOWER:
                sstr << ">=";
                break;
        case UPPER:
                sstr << "<=";
                break;
        case EXACT:
                sstr << "==";
                break;
        default:
                assert(false);
        }
        return sstr.str();
}

std::string Tree::Search::Node::print_depth(void) const
{
        std::stringstream sstr;
        sstr << depth();
        return sstr.str();
}

std::string Tree::Search::Node::print_move(void) const
{
        std::stringstream sstr;
        if (has_move())
                sstr << move();
        else
                sstr << "...";
        return sstr.str();
}

void Tree::Search::Node::set_move(size_t m)
{
        rest_ ^= (move() & MOVE_MASK) << MOVE_SHIFT;
        rest_ ^= (m      & MOVE_MASK) << MOVE_SHIFT;
}

Tree::Search::Node::Type Tree::Search::Node::lower()
{
        return LOWER;
}

Tree::Search::Node::Type Tree::Search::Node::upper()
{
        return UPPER;
}

Tree::Search::Node::Type Tree::Search::Node::exact()
{
        return EXACT;
}

size_t Tree::Search::Node::no_move()
{
        return MOVE_MASK;
}
