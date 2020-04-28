//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#ifndef TYPE_EXPR_QUICK_MANIPULATION_HPP
#define TYPE_EXPR_QUICK_MANIPULATION_HPP

#include <algorithm>
#include <forward_list>
#include <iterator>

// QUICK_UNIQUE
// Reorders the elements in the range [first, last) in such a way that
// all elements for which all the first instance of the value precede
// the duplicated elements .
// Relative order of the elements is not preserved.
// RETURN : Iterator to the first element of the second group
// IF given a unary function, the comparaison is made with the return
// value of each element
template <class ForwardIt>
ForwardIt quick_unique(ForwardIt first, ForwardIt last) {
  while (first != last) {
    auto _cmp = *first;
    first = std::next(first);
    last = std::partition(
        first, last, [&_cmp](decltype(_cmp)& em) { return !(_cmp == em); });
  }
  return last;
}

template <class ForwardIt, typename UnaryFunc>
ForwardIt quick_unique(ForwardIt first, ForwardIt last, UnaryFunc func) {
  while (first != last) {
    auto _cmp = func(*first);
    first = std::next(first);
    last = std::partition(first, last, [&_cmp, func](decltype(_cmp)& em) {
      return !(_cmp == func(em));
    });
  }
  return last;
}

// QUICK_GROUP
// Reorders the elements in the range [first, last) in such a way that
// all elements for which are equal to each other are grouped.
// Relative order of the elements is not preserved.
// RETURN : Iterator to the last element.
// IF given a unary function, the comparaison is made with the return
// value of each element
template <class ForwardIt>
ForwardIt quick_group(ForwardIt first, ForwardIt last) {
  while (first != last) {
    auto _cmp = *first;
    auto _next = std::next(first);

    first = std::partition(_next, last,
                           [&_cmp](decltype(_cmp)& em) { return _cmp == em; });
  }
  return first;
}

template <class ForwardIt, typename UnaryFunc>
ForwardIt quick_group(ForwardIt first, ForwardIt last, UnaryFunc func) {
  while (first != last) {
    auto _cmp = func(*first);
    auto _next = std::next(first);

    first = std::partition(_next, last, [&_cmp, func](decltype(_cmp)& em) {
      return _cmp == func(em);
    });
  }
  return first;
}

#endif
