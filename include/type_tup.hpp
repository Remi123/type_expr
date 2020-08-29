//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#ifndef TYPE_EXPR_TUP_HPP
#define TYPE_EXPR_TUP_HPP

#include <utility>

#include "type_expr.hpp"

namespace te {

// FORWARD DECLARATION

// TUP_INST
template <typename Index, typename Type>
struct tup_inst;

// TUP_IMPL
template <typename... Zip_Indexes_Types>
struct tup_impl;

// TUP
template <typename... Ts>
struct tup;

// IMPLEMENTATION

// TUP_INST
template <int I, typename T>
struct tup_inst<te::i<I>, T> {
  tup_inst(T &&t) : data{std::forward<T>(t)} {}

 protected:
  T data;
};

// TUP_IMPL
template <typename... Is, typename... Ts, template <typename...> class TypeList>
struct tup_impl<TypeList<Is, Ts>...> : tup_inst<Is, Ts>... {
  tup_impl(Ts &&... ts) : tup_inst<Is, Ts>(std::forward<Ts>(ts))... {}
  template <unsigned int I>
  auto get() -> te::eval_pipe_<ts_<Ts...>, te::at_<I>> & {
    return te::eval_pipe_<ts_<tup_inst<Is, Ts>...>, te::at_<I>>::data;
  }
  };

// TUP
template <typename... Ts>
using te_tup_metafunction =
    te::eval_pipe_<te::ts_<Ts...>, te::zip_index, te::quote_<tup_impl>>;

template <typename... Ts>
struct tup : te_tup_metafunction<Ts...> {
  tup(Ts &&... ts) : te_tup_metafunction<Ts...>(std::forward<Ts>(ts)...) {}
};

template <typename... Ts>
te::tup<Ts...> make_tup(Ts &&... ts) {
  return te::tup<Ts...>{std::forward<Ts>(ts)...};
};

template <unsigned int I, typename... Ts>
auto get(te::tup<Ts...> &t) -> decltype(t.template get<I>()) {
  return t.template get<I>();
}

template <class... Types>
tup<Types &&...> forward_as_tup(Types &&... args) noexcept {
  return te::tup<Types &&...>(std::forward<Types>(args)...);
}

template<typename ... Ts, std::size_t ... Is>
te::eval_pipe_<te::input_<Ts...>,te::fork_<te::at_<Is>...>,te::wrap_<tup>> tup_get(std::integer_sequence<std::size_t,Is...>,te::tup<Ts...>& tup)  
{
	return te::make_tup(std::move(te::get<Is>(tup))...);
}

template<typename ... Ts>
using indexes = 
te::eval_pipe_<te::input_<Ts...>, te::zip_index
		,te::group_range_<te::second,te::size> 
		,te::sort_<transform_<te::second,te::size>,te::greater_<>>
		,te::transform_<te::first>
		,te::wrap_std_integer_sequence_<std::size_t>
		>;

//TUP_SORT
// Given a tuple, sort it by it's size, bigger first.
//
template<typename ... Ts>
auto tup_sort(te::tup<Ts...>& tu) -> decltype(tup_get(indexes<Ts...>{},tu))
{
	
	return tup_get(indexes<Ts...>{},tu);

}




// TUP_CAT
// Given multiple tup, concatenate them using the same expension trick than Eric
// Niebler The trick is to get two std::integer_sequence which are the cartesian
// cartesian of each indexes of the tups with their respective index in the
// concatenation function. given tup< tup<A,B,C>, tup<D>, tup<>, tup<E,F> > //
// Tup_of_Tups index_of_tup           = std::integer_sequence<int,0,0,0,1,3,3>
// index inside each tup  = std::integer_sequence<int,0,1,2,0,0,1>
//
namespace detail {
template <int... Is, int... Ks, typename... Ts, typename Tup_of_Tups>
te::tup<Ts...> tup_cat_impl(std::integer_sequence<int, Is...>,
                            std::integer_sequence<int, Ks...>, te::ls_<Ts...>,
                            Tup_of_Tups &&tpls) {
  return te::tup<Ts...>{
      // get<0>().get<0>(), get<0>().get<1>(), ...
      std::forward<Ts>(tpls.template get<Is>().template get<Ks>())...};
}
};  // namespace detail
template <typename... Tups, typename Ret = te::eval_pipe_<
                                ts_<Tups...>, te::transform_<te::unwrap>,
                                te::flatten, quote_<te::tup>>>
Ret tup_cat(Tups &&... tups) {
  // This do the magic of getting the cartesian cartesian of each tup's types
  // with the index inside
  using zip_indexes = te::eval_pipe_<
      te::ts_<Tups...>, te::transform_<te::unwrap, te::length, te::mkseq_<>>,
      te::zip_index, transform_<te::cartesian_<te::listify>>,
      flatten, unzip>;

  using tup_index =
      te::eval_pipe_<zip_indexes, te::first, te::wrap_std_integer_sequence_<int>>;
  using types_index =
      te::eval_pipe_<zip_indexes, te::second, te::wrap_std_integer_sequence_<int>>;
  return detail::tup_cat_impl(
      tup_index{},    // int_seq
      types_index{},  // int_seq
      eval_pipe_<te::ts_<Ret>, te::unwrap,
                 te::listify>{},  // typelist of all the types
      te::forward_as_tup(std::forward<Tups>(tups)...));
};

};  // namespace te

#endif  // TYPE_EXPR_TUP_HPP
