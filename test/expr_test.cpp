//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#include "type_expr.hpp"

#include <assert.h>

#include <type_traits>

#include "type_tup.hpp"

namespace te = type_expr;

int main() {
  using namespace type_expr;
  namespace te = type_expr;

  static_assert(std::is_same<i<(4 + 3 * 2 / 1)>, i<10>>::value, "");

  auto input_test = te::eval_pipe_<input_<i<1>>, input_<i<2>>>{};
  static_assert(std::is_same<decltype(input_test), i<2>>::value, "");

  auto sort_test = te::eval_pipe_<
      input_<i<3>, i<1>, i<3>, i<4>, i<3>, i<1>, i<2>, i<3>, i<5>>,
      sort_<less_<>>>{};
  sort_test = input_<i<1>, i<1>, i<2>, i<3>, i<3>, i<3>, i<3>, i<4>, i<5>>{};

  auto pipe_test = te::eval_pipe_<input_<i<3>>, plus_<i<1>>, plus_<i<2>>>{};
  static_assert(std::is_same<decltype(pipe_test), i<6>>::value, "");

  auto fold_test =
      te::eval_pipe_<input_<i<1>, i<2>, i<3>>, fold_left_<plus_<>>>{};
  static_assert(std::is_same<decltype(fold_test), i<6>>::value, "");

  static_assert(std::is_same<decltype(pipe_test), decltype(fold_test)>::value,
                "");

  auto pipe_pipe_test =
      te::eval_pipe_<pipe_<input_<i<3>>, plus_<i<1>>>, plus_<i<2>>>{};
  static_assert(std::is_same<decltype(pipe_pipe_test), i<6>>::value, "");
  // -------------------------------------------------------------------
  // -------------------------------------------------------------------
  // -------------------------------------------------------------------
  // UNWRAP TESTING
  // There was a problem with unwrap...
  // those test stay there because I don't want to debug this again.
  te::eval_pipe_<input_<std::array<int, 5>>, unwrap>{} = input_<int, i<5>>{};
  te::eval_pipe_<input_<std::integer_sequence<int, 5, 6>>, unwrap>{} =
      input_<i<5>, i<6>>{};
  te::eval_pipe_<input_<ls_<int, i<0>>>, unwrap, second> t11 = i<0>{};
  te::eval_pipe_<input_<ls_<int, i<0>>>, unwrap, pipe_<identity, second>>{} =
      i<0>{};
  te::eval_pipe_<input_<ls_<int, i<0>>>, unwrap, pipe_<second>>{} = i<0>{};
  te::eval_pipe_<input_<ls_<int, i<0>>>, pipe_<unwrap, pipe_<second>>>{} =
      i<0>{};

  te::eval_pipe_<input_<ls_<int, i<0>>>, pipe_<unwrap, first>,
                 same_as_<int>>{} = b<true>{};

  constexpr te::eval_pipe_<input_<int>, unwrap> unwrap_error =
      type_expr::error_<te::unspecialized, int>{};
  // Once an error_<Ts...> is discover, the pipe continue but doesn't evaluate.
  // a catch_ metafunction will never be implemented.

  constexpr te::eval_pipe_<input_<ls_<int>>, unwrap> pipe_unwrap = 0;
  constexpr te::eval_pipe_<input_<ls_<int>>, pipe_<input_<ls_<int>>, unwrap>>
      pipe_unwrap2 = 0;
  constexpr te::eval_pipe_<input_<ls_<int>>, pipe_<unwrap>> pipe_unwrap3 = 0;
  constexpr te::eval_pipe_<input_<ls_<int, float>>, pipe_<unwrap>,
                           same_as_<int, float>>
      pipe_unwrap4 = b<true>{};
  constexpr te::eval_pipe_<input_<ls_<int, float>>, pipe_<unwrap>, get_<0>>
      pipe_unwrap5 = 0;
  //
  // -------------------------------------------------------------------
  // -------------------------------------------------------------------
  // -------------------------------------------------------------------

  // Calculation tests
  // -------------------------------------------------------------------
  static_assert(
      te::eval_pipe_<input_<double>, plus_<int>, same_as_<double>>::value, "");
  static_assert(
      te::eval_pipe_<input_<i<21>, i<49>>, gcd, same_as_<i<7>>>::value, "");
  static_assert(
      te::eval_pipe_<input_<i<21>, i<49>>, lcm, same_as_<i<147>>>::value, "");
  // -------------------------------------------------------------------

  auto pipe_less_ = te::eval_pipe_<input_<i<1>, i<2>>, less_<>>{};
  auto pipe_less__2 = te::eval_pipe_<input_<i<1>>, less_<i<2>>>{};

  static_assert(pipe_less_.value, "");
  static_assert(pipe_less__2.value, "");

  auto pipe_is_test = te::eval_pipe_<input_<i<1>>, same_as_<i<1>>>{};
  static_assert(pipe_is_test.value, "");

  auto pipe_is_test_multi =
      te::eval_pipe_<input_<i<1>, i<2>>, same_as_<i<1>, i<2>>>{};
  static_assert(pipe_is_test_multi.value, "");
  // This is such a good feature; instead of always listify then compare with
  // ls_<...>

  static_assert(
      eval_pipe_<input_<input_<int, float>>, same_as_<int, float>>::value,
      "input_ is recursive");

  auto pipe_isnt_ = te::eval_pipe_<input_<i<1>, i<2>>, not_same_as_<int>>{};
  static_assert(pipe_isnt_.value, "");
  // Isnt_ is the opposite of is_

  auto pipe_add = te::eval_pipe_<input_<int>, pipe_<push_back_<float>>,
                                 same_as_<int, float>>{};
  static_assert(pipe_add.value, "");

  auto pipe_add2 = te::eval_pipe_<input_<int, short>, pipe_<push_back_<float>>,
                                  same_as_<int, short, float>>{};
  static_assert(pipe_add2.value, "");

  static_assert(te::eval_pipe_<input_<int, float>, push_back_<ls_<>>,
                               same_as_<int, float, ls_<>>>::value,
                "");
  static_assert(te::eval_pipe_<input_<int, float>, push_front_<ls_<>>,
                               same_as_<ls_<>, int, float>>::value,
                "");
  static_assert(te::eval_pipe_<push_back_<int>, same_as_<int>>::value, "");
  // pushing types has never been this easy

  static_assert(
      te::eval_pipe_<input_<int, int, int>, length, same_as_<i<3>>>::value, "");
  // length continue with the number of elements in the inputs inside a i<>

  static_assert(te::eval_pipe_<input_<char>, size, same_as_<i<1>>>::value, "");
  static_assert(te::eval_pipe_<input_<char, char>, transform_<size>,
                               same_as_<i<1>, i<1>>>::value,
                "");
  // size continue with the size of the type in the inputs inside a i<>

  static_assert(
      te::eval_pipe_<input_<int, float, ls_<int>>, first, same_as_<int>>::value,
      "");
  static_assert(te::eval_pipe_<input_<int, float, ls_<int>>, last,
                               same_as_<ls_<int>>>::value,
                "");
  // Getting the first and the last.

  static_assert(te::eval_pipe_<input_<int, input_<int>, ls_<int, int>>, flatten,
                               same_as_<int, int, ls_<int, int>>>::value,
                "");
  // Flatten and join_list might be reworked

  static_assert(te::eval_pipe_<input_<int, int, int>,
                               cond_<same_as_<int, int>, input_<b<false>>,
                                     same_as_<int, int, int>>>::value,
                "");
  // Conditional have a predicate and continue with the second or the third
  // function

  static_assert(te::eval_pipe_<input_<int, float, short>, get_<1>,
                               same_as_<float>>::value,
                "");
  static_assert(te::eval_pipe_<input_<i<1>, i<2>>, transform_<plus_<i<3>>>,
                               same_as_<i<4>, i<5>>>::value,
                "");
  // Transform apply this metafunctor to the types one by one
  // Can also be called a map_ but I disgress transform_ is a better name

  static_assert(te::eval_pipe_<input_<i<1>>, fork_<plus_<i<1>>, plus_<i<2>>>,
                               same_as_<i<2>, i<3>>>::value,
                "");
  // pipes and forks in a single expression ? Cool Stuff

  static_assert(
      te::eval_pipe_<input_<int, float, short>, partition_<same_as_<int>>,
                     same_as_<ls_<int>, ls_<float, short>>>::value,
      "");
  // Partition , like the std counterpart, result in two
  // containers : first one with predicate == true,
  //              second one with predicate == false

  static_assert(
      te::eval_pipe_<te::input_<int, te::input_<float, float>, char>,
                     te::each_<te::same_as_<int>, te::same_as_<float, float>,
                               te::same_as_<char>>,
                     te::all_of_<te::same_as_<std::true_type>>>::value,
      "");
  // Each isn't my favorite fonction, but is there nonetheless.
  //

  static_assert(te::eval_pipe_<input_<int, float, int, short>,
                               replace_if_<same_as_<int>, input_<float>>,
                               same_as_<float, float, float, short>>::value,
                "");
  // replace_if transform the type into another if the predicate is true

  static_assert(te::eval_pipe_<
                    input_<input_<int, short>, input_<float, char>>, cartesian,
                    same_as_<input_<int, float>, input_<int, char>,
                             input_<short, float>, input_<short, char>>>::value,
                "");
  // cartesian is a little bit special : given two lists, it return each
  // permutation possible while respecting the order

  static_assert(
      te::eval_pipe_<input_<i<1>, i<2>, i<3>, i<4>, i<5>>,
                     any_of_<same_as_<i<5>>>, same_as_<std::true_type>>::value,
      "");
  static_assert(
      te::eval_pipe_<input_<i<1>, i<0>, i<0>>, count_if_<same_as_<i<0>>>,
                     same_as_<i<2>>>::value,
      "");
  // Count_if does exactly what is name suggest.

  static_assert(
      te::eval_pipe_<input_<int, int, int>, all_of_<same_as_<int>>>::value, "");
  static_assert(
      te::eval_pipe_<input_<int, float, int>, any_of_<same_as_<float>>>::value,
      "");
  static_assert(
      te::eval_pipe_<input_<int, float, int>, none_of_<same_as_<char>>>::value,
      "");
  // Does exactly the same as their std:: counterpart

  static_assert(te::eval_pipe_<input_<int, int, int>, pipe_<get_<0>>,
                               same_as_<int>>::value,
                "");
  constexpr te::eval_pipe_<input_<i<1>, i<2>, i<3>>, get_<-1>, same_as_<>>
      t_err = type_expr::error_<type_expr::get_<-1>::index_out_of_range>{};
  constexpr te::eval_pipe_<input_<int, int, int>, pipe_<get_<3>>, same_as_<>>
      t_err2 = type_expr::error_<type_expr::get_<3>::index_out_of_range>{};

  static_assert(
      te::eval_pipe_<input_<i<2>>, mkseq, same_as_<i<0>, i<1>>>::value, "");
  static_assert(te::eval_pipe_<input_<i<1>>, mkseq, same_as_<i<0>>>::value, "");
  static_assert(te::eval_pipe_<input_<i<0>>, mkseq, same_as_<>>::value, "");

  static_assert(
      te::eval_pipe_<input_<int, i<0>>,
                     cond_<pipe_<first, same_as_<int>>, input_<std::true_type>,
                           input_<std::false_type>>>::value,
      "");
  static_assert(
      te::eval_pipe_<input_<float, int, float, int>, find_if_<same_as_<int>>,
                     same_as_<i<1>, int>>::value,
      "");
  static_assert(
      te::eval_pipe_<input_<int, float>, zip_index,
                     same_as_<input_<i<0>, int>, input_<i<1>, float>>>::value,
      "");
  static_assert(te::eval_pipe_<input_<i<1>>, plus_<i<1>>, mkseq,
                               same_as_<i<0>, i<1>>>::value,
                "");

  // We will use te::ls_<...> instead of tuple but for the purpose of this test
  // the same result is achieve
  /*static_assert(*/
  // te::eval_pipe_<
  // te::input_<te::ls_<int, float, char>, te::ls_<>,
  // te::ls_<int *, char *>, te::ls_<int>>,
  // te::transform_<te::unwrap, te::length, te::mkseq>, te::zip_index,
  // transform_<te::cartesian>, te::flatten, te::unzip,
  // transform_<quote_std_integer_sequence>,
  // same_as_<std::integer_sequence<int, 0, 0, 0, 2, 2, 3>,
  // std::integer_sequence<int, 0, 1, 2, 0, 1, 0>>>::value,
  /*"Eric Niebler Challenge");*/
  static_assert(
      te::eval_pipe_<
          te::input_<te::ls_<int, float, char>, te::ls_<>,
                     te::ls_<int *, char *>, te::ls_<int>>,
          te::transform_<te::unwrap, te::length, te::mkseq>, te::zip_index,
          transform_<te::cartesian, transform_<listify>>,
          te::flatten
          //,transform_<te::unwrap>
          ,
          te::unzip, transform_<quote_std_integer_sequence>,
          same_as_<std::integer_sequence<int, 0, 0, 0, 2, 2, 3>,
                   std::integer_sequence<int, 0, 1, 2, 0, 1, 0>>>::value,
      "Eric Niebler Challenge");
  // On this challenge, the goal was to unwrap, remove empty class, sort them by
  // size and rewrap them. on_args_<Es...> deals with the unwrap rewrap if the
  // signature of the type accept only types.
  struct Z {};  // EMPTY

  using ArthurODwyer_metafunction =
      te::on_args_<te::remove_if_<te::lift_<std::is_empty>>,
                   te::sort_<te::transform_<te::size>, te::greater_<>>>;

  static_assert(
      te::eval_pipe_<
          te::input_<te::ls_<Z, int[4], Z, int[1], Z, int[2], int[3]>>,
          ArthurODwyer_metafunction,
          same_as_<te::ls_<int[4], int[3], int[2], int[1]>>>::value,
      "Arthur O'Dwyer");

  static_assert(te::eval_pipe_<
                    te::input_<te::ls_<Z, int[4], Z, int[1], Z, int[2], int[3]>,
                               te::ls_<int[2], Z, int[1], Z, int[3]>>,
                    te::transform_<ArthurODwyer_metafunction>,
                    same_as_<te::ls_<int[4], int[3], int[2], int[1]>,
                             te::ls_<int[3], int[2], int[1]>>>::value,
                "Arthur O'Dwyer but with multiple types");

  static_assert(
      te::eval_pipe_<
          te::input_<te::i<5>, te::i<3>, te::i<2>, te::i<1>, te::i<4>>,
          te::fork_<te::sort_<te::less_<>>,
                    te::sort_<te::greater_<>, te::not_<>>>,
          te::lift_<std::is_same>>::value,
      "");

  static_assert(eval_pipe_<input_<int>, if_<same_as_<int>>,
                           then_<input_<int *>>, endif, same_as_<int *>>::value,
                "");
  static_assert(
      eval_pipe_<input_<float>, if_<same_as_<int>>,
                 then_<lift_<std::add_pointer>>, else_if_<same_as_<float>>,
                 then_<lift_<std::add_const>>, endif,
                 same_as_<const float>>::value,
      "");
  static_assert(
      eval_pipe_<input_<char, int>, if_<same_as_<char, float>>,
                 or_if_<same_as_<char, int>>, then_<listify>,
                 else_<input_<int>>, endif, same_as_<ls_<char, int>>>::value,
      "");
  static_assert(eval_pipe_<input_<char, int>, if_<same_as_<char, float>>,
                           and_if_<same_as_<char, int>>, then_<listify>,
                           else_<input_<int>>, endif, same_as_<int>>::value,
                "");

  return 0;
}

template <char C>
using c_t = std::integral_constant<char, C>;

template <char... C>
using Row = te::input_<c_t<C>...>;

template <typename Top, typename Bot>
using Domino = te::input_<Top, Bot>;

using d0 = Domino<Row<'b', 'b', 'a'>, Row<'b', 'b'>>;
using d1 = Domino<Row<'a', 'b'>, Row<'a', 'a'>>;
using d2 = Domino<Row<'a'>, Row<'b', 'a'>>;
using dnull = Domino<Row<>, Row<>>;  // Adding a Domino without value to have
                                     // fun

static_assert(
    te::eval_pipe_<te::input_<d0, d1, dnull, d2>  // Input types
                   ,
                   te::unzip  // Unzip into input_<LeftRows...> and
                              // input_<RightRows...>
                   ,
                   te::transform_<te::flatten>  // Each of the two input_<...>
                                                // concatenate their char_type
                   ,
                   te::lift_<std::is_same>  // Compare if they are the same to
                                            // std::true_type or std::false_type
                   >::value,
    "Comment each line from bottom to top to see how it work");
