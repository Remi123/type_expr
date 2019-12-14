#include "rage.hpp"
#include <assert.h>
#include <type_traits>
int main() {
  // ls_<float,int,void,short,double> t2 =
  // sort<float,int,int,void,void,float,short,double>::type{};
  using namespace rage;

  static_assert(std::is_same<i<(4 + 3 * 2 / 1)>, i<10>>::value, "");

  auto input_test = pipe_<input<i<1>>, input<i<2>>>::type{};
  static_assert(std::is_same<decltype(input_test), i<2>>::value, "");

  


  auto sort_test =
      pipe_<input<i<3>, i<1>, i<3>, i<4>, i<3>, i<1>, i<2>, i<3>, i<5>>,
            sort>::type{};
  sort_test = input<i<1>, i<1>, i<2>, i<3>, i<3>, i<3>, i<3>, i<4>, i<5>>{};

  auto pipe_test = pipe_<input<i<3>>, plus<i<1>>, plus<i<2>>>::type{};
  static_assert(std::is_same<decltype(pipe_test), i<6>>::value, "");

  auto fold_test = pipe_<input<i<1>, i<2>, i<3>>, fold_left_<add<>>>::type{};
  static_assert(std::is_same<decltype(fold_test), i<6>>::value, "");

  static_assert(std::is_same<decltype(pipe_test), decltype(fold_test)>::value,
                "");

  auto pipe_fold_test =
      pipe_<input<i<1>, i<4>>, fold_left_<lift_<Adding>>>::type{};
  static_assert(std::is_same<decltype(pipe_fold_test), i<5>>::value, "");

  auto pipe_pipe_test =
      pipe_<pipe_<input<i<3>>, plus<i<1>>>, plus<i<2>>>::type{};
  static_assert(std::is_same<decltype(pipe_pipe_test), i<6>>::value, "");
// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------
// UNWRAP TESTING 
// There was a problem with unwrap... 
// those test stay there because I don't want to debug this again.

  pipe_t<input<ls_<int,i<0>>>,unwrap,second> t11 = i<0>{} ;
  pipe_t<input<ls_<int,i<0>>>,unwrap,pipe_<identity,second> > t12 = i<0>{} ;
  pipe_t<input<ls_<int,i<0>>>,unwrap,pipe_<second> > t13 = i<0>{} ;
  pipe_t<input<ls_<int,i<0>>>,pipe_<unwrap,pipe_<second>>> t14 = i<0>{} ;

  pipe_v<input<ls_<int,i<0>>>,pipe_<unwrap,first>,is_<int>  >;

  constexpr pipe_<input<int>, unwrap ,is_<nothing> >::type t = b<true>{};
  constexpr  pipe_<input<ls_<int>>,unwrap>::type pipe_unwrap = 0;
  constexpr  pipe_<input<ls_<int>>,pipe_<input<ls_<int>>,unwrap>>::type pipe_unwrap2 = 0;
  constexpr  pipe_<input<ls_<int>>,pipe_<unwrap>>::type pipe_unwrap3 = 0;
  constexpr  pipe_<input<ls_<int,float>>,pipe_<unwrap>, is_<int,float>>::type pipe_unwrap4 = b<true>{};
  constexpr  pipe_<input<ls_<int,float>>,pipe_<unwrap>, get_<0>>::type pipe_unwrap5 = 0;
  // 
// -------------------------------------------------------------------
// -------------------------------------------------------------------
// -------------------------------------------------------------------
  
  auto pipe_less = pipe_<input<i<1>, i<2>>, lift_<less>>::type{};

  static_assert(pipe_less.value, "");

  auto pipe_is_test = pipe_<input<i<1>>, is_<i<1>>>::type{};
  static_assert(pipe_is_test.value, "");

  auto pipe_is_test_multi = pipe_<input<i<1>, i<2>>, is_<i<1>, i<2>>>::type{};
  static_assert(pipe_is_test_multi.value, "");
  // This is such a good feature; instead of always listify then compare with
  // ls_<...>

  auto pipe_isnt_ = pipe_<input<i<1>, i<2>>, isnt_<int>>::type{};
  static_assert(pipe_isnt_.value, "");
  // Isnt_ is the opposite of is_

  auto pipe_add = pipe_<input<int>,pipe_<push_back_<float>>, is_<int,float> >::type{}; 
  static_assert(pipe_add.value,"");

  auto pipe_add2 = pipe_<input<int,short>,pipe_<push_back_<float>>, is_<int,short,float> >::type{}; 
  static_assert(pipe_add2.value,"");

  

  static_assert(pipe_<input<int, float>, push_back_<ls_<>>,
                      is_<int, float, ls_<>>>::type::value,
                "");
  static_assert(pipe_<input<int, float>, push_front_<ls_<>>,
                      is_<ls_<>, int, float>>::type::value,
                "");
  static_assert(pipe_<push_back_<int>, is_<int>>::type::value, "");
  // pushing types has never been this easy

  static_assert(pipe_<input<int, int, int>, length, is_<i<3>>>::type::value,
                "");
  // length continue with the number of elements in the inputs inside a i<>

  static_assert(pipe_<input<char>, size, is_<i<1>>>::type::value, "");
  static_assert(
      pipe_<input<char, char>, transform_<size>, is_<i<1>, i<1>>>::type::value,
      "");
  // size continue with the size of the type in the inputs inside a i<>

  static_assert(
      pipe_<input<int, float, ls_<int>>, first, is_<int>>::type::value, "");
  static_assert(
      pipe_<input<int, float, ls_<int>>, last, is_<ls_<int>>>::type::value, "");
  // Getting the first and the last.

  static_assert(pipe_<input<int, input<int>, ls_<int, int>>, flatten,
                      is_<int, int, ls_<int, int>>>::type::value);
  // Flatten and join_list might be reworked

  static_assert(
      pipe_<input<int, int, int>, cond_<is_<int, int>, input<b<false>>,
                                        is_<int, int, int>>>::type::value,
      "");
  // Conditional have a predicate and continue with the second or the third
  // function

  static_assert(
      pipe_<input<int, float, short>, get_<1>, is_<float>>::type::value, "");
  static_assert(pipe_<input<i<1>, i<2>>, transform_<plus<i<3>>>,
                      is_<i<4>, i<5>>>::type::value,
                "");
  // Transform apply this metafunctor to the types one by one
  // Can also be called a map_ but I disgress transform_ is a better name

  static_assert(pipe_<input<i<1>>, fork_<plus<i<1>>, plus<i<2>>>,
                      is_<i<2>, i<3>>>::type::value,
                "");
  // pipes and forks in a single expression ? Cool Stuff

  static_assert(pipe_<input<int, float, short>, partition_<is_<int>>,
                      is_<ls_<int>, ls_<float, short>>>::type::value,
                "");
  // Partition , like the std counterpart, result in two
  // containers : first one with predicate == true,
  //              second one with predicate == false

  static_assert(
      pipe_<input<int, float, int, short>, replace_if_<is_<int>, float>,
            is_<float, float, float, short>>::type::value,
      "");
  // replace_if transform the type into another if the predicate is true

  static_assert(pipe_<input<ls_<int, short>, ls_<float, char>>, product,
                      is_<ls_<int, float>, ls_<int, char>, ls_<short, float>,
                          ls_<short, char>>>::type::value,
                "");
  // product is a little bit special : given two lists, it return each
  // permutation possible while respecting the order

static_assert( pipe_<input<i<1>,i<2>,i<3>,i<4>,i<5>>, or_<is_<i<5>>> , is_<true_type>>::type::value,"");
static_assert(pipe_<input<i<1>,i<0>,i<0>>, count_if_<is_<i<0>>>, is_<i<2>>>::type::value,"");

  return 0;
}
