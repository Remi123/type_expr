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
  if (first == last) return first;
  auto _next = std::next(first);
  ForwardIt _mid = std::partition(
      _next, last, [first](decltype(*first) em) { return !(*first == em); });
  return quick_unique(_next, _mid);
}

template <class ForwardIt, typename UnaryFunc>
ForwardIt quick_unique(ForwardIt first, ForwardIt last, UnaryFunc func) {
  if (first == last) return first;
  auto _next = std::next(first);
  auto _cmp = func(*first);
  ForwardIt _mid = std::partition(_next, last, [_cmp, func](decltype(_cmp) em) {
    return !(_cmp == func(em));
  });
  return quick_unique(_next, _mid, func);
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
  if (first == last) return first;
  auto _next = std::next(first);
  ForwardIt _mid = std::partition(
      _next, last, [first](decltype(*first) em) { return *first == em; });
  return quick_group(_mid, last);
}

template <class ForwardIt, typename UnaryFunc>
ForwardIt quick_group(ForwardIt first, ForwardIt last, UnaryFunc func) {
  if (first == last) return first;
  auto _next = std::next(first);
  auto _cmp = func(*first);
  ForwardIt _mid = std::partition(
      _next, last,
      [_cmp, func](decltype(_cmp)& em) { return _cmp == func(em); });
  return quick_group(_mid, last, func);
}

#endif
