#pragma once
#include <algorithm>                    // min_element, rotate, upper_bound
#include <iterator>                     // advance, iter_swap

namespace dctl {

template<class OutputIterator, class Size, class Assignable>
void iota_n (OutputIterator first, Size n, Assignable value)
{
        std::for_each(first, std::advance(first, n), [=]() {
                *first++ = value++;
        });
}

template<class ForwardIterator>
void insertion_sort(ForwardIterator first, ForwardIterator last)
{    
        std::for_each(first, last, [=](ForwardIterator it) {                    
                std::rotate(std::upper_bound(first, it, *it), it, it + 1);
        });
}

template<class ForwardIterator>
void selection_sort(ForwardIterator first, ForwardIterator last)
{    
        std::for_each(first, last, [=](ForwardIterator it) {                    
                std::iter_swap(std::min_element(it, last), it);
        });
}

}       // namespace dctl
