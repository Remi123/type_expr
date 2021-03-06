﻿//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#ifndef TYPE_EXPR_HPP
#define TYPE_EXPR_HPP

#include <ratio>
#include <type_traits>
#include <utility>

// STD NAMESPACE FORWARD DECLARATION
// Here is all the types of std namespace that we will provide utilities
namespace std {
#if __cplusplus < 201403L
template <class T, T... Ints>
class integer_sequence;
#endif
template <typename T, std::size_t N>
class array;
};  // namespace std
namespace kvasir {
namespace mpl {
template <typename... Ts>
struct list;
};
};  // namespace kvasir
namespace metal {
template <typename... Ts>
struct list;
};
namespace boost {
namespace mp11 {
template <class... T>
struct mp_list;
};
namespace fusion {
template <class... Ts>
struct list;
}
};  // namespace boost

namespace te {

// -------------------------------------------------------
// FIRST CLASS CITIZEN OF TYPE_EXPR
// -------------------------------------------------------

// INPUT : Universal type container of types (aka typelist).
template <typename... Ts>
struct ts_ {
  typedef ts_ type;
  template <typename...>
  struct f {
    typedef ts_<Ts...> type;
  };
};
template <typename T>
struct ts_<T> {
  typedef T type;
  template <typename... Ts>
  struct f {
    typedef T type;
  };
};
// Recursive ts_. Very powerful feature but make some algo not very intuitive.
//template <typename... Ts>
//struct ts_<ts_<Ts...>> : ts_<Ts...> {};

// NOTHING : Universal representation of the concept of nothing
using nothing = ts_<>;

// ERROR : Template used for debugging
template <typename... MSG>
struct error_ {
  template <typename...>
  struct f {
    typedef error_<MSG...> type;
  };
};

// LS_ : user-declared container
template <typename...>
struct ls_ {};

// IDENTITY : Continue with whatever the ts_ were.
struct identity {
  template <typename... Ts>
  struct f : ts_<Ts...> {};
};

// SPECIALIZATION
// Some errors could be avoided by letting the user specialize a function for a
// given type While this is not a silver bullet and is not implemented
// everywhere, we will favor this method
struct unspecialized {};
template <typename MetaFunction, typename... Ts>
struct specialization {
  typedef error_<unspecialized, Ts...> type;
};

// I : Universal integer type.
template <int V>
using i = std::integral_constant<int, V>;

// ZERO : User-specialized to retrieve the zero of their type.
template <typename T>
struct zero;

// First specialization of zero. The zero of any i<Num> is i<0>
template <typename T, T value>
struct zero<std::integral_constant<T, value>> {
  typedef std::integral_constant<T, 0> type;
};

// Second specilisation of zero. The zero of  any ratio<N,D> is ratio<0>
template <std::size_t num, std::size_t den>
struct zero<std::ratio<num, den>> {
  typedef std::ratio<0, den> type;
};

// Third specialization of zero. The zero of any std::integer_sequence is with
// the same amount of zero
template <typename T, T... value>
struct zero<std::integer_sequence<T, value...>> {
  typedef std::integer_sequence<T, (value * 0)...> type;
};

// B : Universal boolean type.
template <bool B>
using b = std::integral_constant<bool, B>;

// -------------------------------------------------------
// METAEXPRESSION
// -------------------------------------------------------

// INPUT : Dedicated meta-expression to send types to the next functions.
template <typename... Ts>
struct input_ : ts_<Ts...> {};

template <typename... Ts>
struct input_<ts_<Ts...>> : ts_<Ts...> {
  // Specialization that allow one recursion.
  // We can now use input as a replacement for ts
};

template<typename ... aTs>
struct input_append_
{
	template<typename ... Ts>
	struct f
	{using type = ts_<Ts...,aTs...>;};
};

template<typename ... pTs>
struct input_prepend_
{
	template<typename ... Ts>
	struct f
	{using type = ts_<pTs...,Ts...>;};
};

template<typename ... A>
struct ts_append_
{
	template<typename ... Ts>
	struct f /*: ts_<Ts...,ts_<A...>>*/{
		using type = ts_<Ts...,ts_<A...>>;
	};
};
template<typename ... P>
struct ts_prepend_
{
	template<typename ... Ts>
	struct f : ts_<ts_<P...>,Ts...>{};
};

/*template <typename... Ts>*/
//struct input_<kvasir::mpl::list<Ts...>> : ts_<Ts...> {};

//template <typename... Ts>
//struct input_<metal::list<Ts...>> : ts_<Ts...> {};

//template <typename... Ts>
//struct input_<boost::mp11::mp_list<Ts...>> : ts_<Ts...> {};

//template <typename... Ts>
/*struct input_<boost::fusion::list<Ts...>> : ts_<Ts...> {};*/

// TRAIT_ : Universal customization point using template template. Get the ::type
// The Farming field of our library
template <template <typename...> class F>
struct trait_ {
  template <typename... Ts>
  struct f : F<Ts...>{
    //typedef typename F<Ts...>::type type;
  };
};

template<template<typename...> class F>
using wraptype_ = trait_<F>;

// WRAP && QUOTE_: Universal wrapper. Doesn't get the ::type. Use with template alias
// The Other Farming field of our library
template <template <typename...> class F>
struct wrap_ {
  template <typename... Ts>
  struct f {
    typedef F<Ts...> type;
  };
};

template<template <typename ... > class F>
using quote_ = wrap_<F>;

// WRAP_STD_INTEGER_SEQUENCE
// Specialization for std::integer_sequence
template <typename T>
struct wrap_std_integer_sequence_ {
  template <typename... Is>
  struct f {
    typedef std::integer_sequence<T, (T)Is::value...> type;
  };
};

// QUOTE_STD_ARRAY
// Specialization for std::array
struct wrap_std_array {
  template <typename T, typename N>
  struct f {
    typedef std::array<T, N::value> type;
  };
};

// GET_TYPE
// Given a type, get the inner ::type
struct get_type {
  template <typename T>
  struct f {
    typedef typename T::type type;
  };
};

// Type_traits 
// some type_traits
using add_pointer = wraptype_<std::add_pointer>;
using add_const = wraptype_<std::add_const>;

// FOLD_LEFT_ : Fold expression
// The Farmer of the library
template <typename binaryF>
struct fold_left_ {
  template <typename... Ts>
  struct f {
    typedef error_<fold_left_<binaryF>, Ts...> type;
  };
  template <typename A>
  struct f<A> {
    typedef A type;
  };

  /*template <typename... A>*/
  //struct f<ts_<A...>> {
    //typedef typename ts_<A...>::type type;
  /*};*/
  // helper alias to use with the monster below
  template <typename A, typename B>
  using f_impl = typename binaryF::template f<A, B>::type;

  template <typename T0, typename T1,typename ... Ts>
  struct f<T0, T1,Ts...> : f<f_impl<T0, T1>,Ts...> {};
  	template <typename T0, typename T1, typename T2, typename T3, typename T4,
			typename T5, typename T6, typename T7, typename T8, typename... Ts>
		struct f<T0, T1, T2, T3, T4, T5, T6, T7,T8, Ts...>
	  : f<f_impl<f_impl<
			  f_impl<f_impl<f_impl<f_impl<f_impl<f_impl<T0, T1>, T2>, T3>, T4>,
							T5>,
					 T6>,
			  T7>,T8>,
		  Ts...> {
		  };
};

// PIPE_EXPR : Internal-only. Take a type and send it as input to the next
// metafunction.
// The Flour and Yeast of the library.
template <typename...>
struct pipe_context {
  typedef nothing type;
};
template <typename T, typename G>
struct pipe_context<T, G> {
  typedef typename G::template f<T>::type type;
};
template <typename... Ts, typename G>
struct pipe_context<ts_<Ts...>, G> {
  typedef typename G::template f<Ts...>::type type;
};
template <typename ErrorT, typename G>
struct pipe_context<error_<ErrorT>, G> {
  typedef error_<ErrorT> type;
};

// PIPE_ : Universal container of metafunction.
// The Bread and Butter of the library
template <typename... Es>
struct pipe_;

// EVAL_PIPE_ : Helper alias
template <typename... Fs>
using eval_pipe_ = typename pipe_<Fs...>::template f<>::type;
#if __cplusplus > 201403L
template <typename... Fs>
constexpr eval_pipe_<Fs...> eval_pipe_v = eval_pipe_<Fs...>::value;
#endif

template <typename... Es>
struct pipe_ {
  template <typename... Ts>
  struct f {
    typedef
        typename fold_left_<wraptype_<pipe_context>>::template f<ts_<Ts...>,
                                                             Es...>::type type;
  };
  // No ::type a la type_traits. The problem since it's always instanciated even if not
  // asked. required to have an alias eval_pipe_ = typename
  // pipe_<Fs...>::template f<>::type; to instanciate to the result type;
  // As a result, pipe_::type is itself
  using type = pipe_;

  template <typename... Us>
  constexpr pipe_<Es..., Us...> const operator|(const pipe_<Us...> &) {
    return {};
  };
};

template <typename... Fs, typename... Args>
constexpr eval_pipe_<Fs...> eval(const pipe_<Fs...> &, Args &&... args) {
  return eval_pipe_<Fs...>{std::forward<Args>(args)...};
}

// same_as_ : Comparaison metafunction.
template <typename... Ts>
struct same_as_ {
  template <typename... Us>
  struct f {
    typedef typename std::is_same<ts_<Ts...>, ts_<Us...>>::type type;
  };
};
// TRANSFORM_ :
// Similar to haskell's map. Also similar to std::transform
template <typename... Es>
struct transform_ {
  template <typename... Ts>
  struct f {
    typedef ts_<eval_pipe_<input_<Ts>, pipe_<Es...>>...> type;
  };
};

// FORK_ : Inputs are copied to each metafunctions
// The Peanut Butter of the library
template <typename... Es>
struct fork_ {
  template <typename... Ts>
  struct f {
    typedef ts_<eval_pipe_<input_<Ts...>, Es>...> type;
  };
};

// EACH : Badly named mix between fork and transform. Requiere the same number
// of expressions as arguments than inputs.
template <typename... Es>
struct each_ {
  template <typename... Ts>
  struct f {
    typedef ts_<eval_pipe_<input_<Ts>, Es>...> type;
  };
};

// COMPOSE : Construct a function using the inputs, then evaluate using the
// inputs. Without a doubt the most powerful function of my library
template <typename... PipableExpr>
struct compose_ {
  template <typename... Ts>
  struct f {
    using metaexpr = eval_pipe_<input_<Ts...>, PipableExpr..., quote_<pipe_>>;
    using type = typename metaexpr::template f<Ts...>::type;
  };
};

template<typename ... PipableExpr>
struct compose_debug_
{
	template <typename... Ts>
  struct f {
    using type = eval_pipe_<input_<Ts...>, PipableExpr..., quote_<pipe_>>;
  };
};

// COMPOSE_NULL : Construct a function using the inputs, then evaluate from nothing
// A variation where we don't use the inputs
template <typename... PipableExpr>
struct compose_null_ {
  template <typename... Ts>
  struct f {
    using metaexpr = eval_pipe_<input_<Ts...>, PipableExpr..., quote_<pipe_>>;
    using type = typename metaexpr::template f<>::type;
  };
};


// UNWRAP : Universal unwrapper.
struct unwrap {
  template <typename T>
  struct f {
    typedef typename specialization<unwrap, T>::type type;
  };
  template <template <typename T, T...> class F, typename U, U... values>
  struct f<F<U, values...>> : ts_<std::integral_constant<U, values>...> {};

  template <template <typename... Ts> class F, typename... Ts>
  struct f<F<Ts...>> : ts_<Ts...> {};
  template <template <typename... Ts> class F, typename... Ts>
  struct f<const F<Ts...>> : ts_<Ts...> {};
  template <template <typename... Ts> class F, typename... Ts>
  struct f<F<Ts...> &> : ts_<Ts...> {};
  template <template <typename... Ts> class F, typename... Ts>
  struct f<const F<Ts...> &> : ts_<Ts...> {};
};
// specialization unwrap for std::array
// Caution about std::size_t to int conversion
template <typename T, std::size_t value>
struct specialization<unwrap, std::array<T, value>> : ts_<T, i<value>> {};

// NOT_SAME_AS_
template <typename... Ts>
struct not_same_as_ {
  template <typename... Us>
  struct f {
    typedef std::integral_constant<bool,
                                   !std::is_same<ts_<Ts...>, ts_<Us...>>::value>
        type;
  };
};

// CONTAINER : Continue with a type that can contain a template template
// OR THE TYPE ITSELF IF NOT A TEMPLATE TEMPLATE;
struct container {
  struct sorry_not_implemented {};
  template <typename T>
  struct f : ts_<T> {};
  template <template <typename...> class F, typename... Ts>
  struct f<F<Ts...>> : ts_<quote_<F>> {};
};

template <template <typename...> class F>
struct container_is_ : pipe_<container, same_as_<quote_<F>>> {};

// LISTIFY_ : wrap ts_ into ls_
// Could have been implemented by struct listify : quote_<ls_>{}
struct listify {
  template <typename... Ts>
  struct f {
    typedef ls_<Ts...> type;
  };
};

// REVERSE : Reverse the order of the types
struct reverse {
	template<typename ... Ts>
	struct f {using type = eval_pipe_<input_prepend_<Ts>...>; };
};

template <typename... Predicate>
struct conditional {
 private:
  struct Type_not_a_predicate {};

 public:
  template <typename...>
  struct f : ts_<error_<Type_not_a_predicate, Predicate...>> {};
};
template <>
struct conditional<std::true_type> {
  template <typename T, typename F>
  struct f {
    typedef T type;
  };
};
template <>
struct conditional<std::false_type> {
  template <typename T, typename F>
  struct f {
    typedef F type;
  };
};

// COND_ : Similar to std::conditional but only accept metafunctions
template <typename UnaryPredicate, typename True_Expression,
          typename False_Expression>
struct cond_ {
  template <typename... Ts>
  struct f {
    typedef
        typename conditional<typename UnaryPredicate::template f<Ts...>::type>::
            template f<True_Expression,
                       False_Expression>::type::template f<Ts...>::type type;
  };
};

// MKSEQ_ : Continue with i<0>, i<1>, ... , i<N-1>
template <std::size_t... i>
struct index_sequence {
  typedef std::size_t value_type;
  typedef index_sequence<i...> type;
  typedef ts_<std::integral_constant<int, i>...> to_types;
};

// this structure doubles index_sequence elements.
template <std::size_t s, typename IS>
struct doubled_index_sequence;

template <std::size_t s, std::size_t... i>
struct doubled_index_sequence<s, index_sequence<i...>> {
  typedef index_sequence<i..., (s + i)...> type;
};

// this structure incremented by one index_sequence, iff NEED-is true,
// otherwise returns IS
template <bool NEED, typename IS>
struct inc_index_sequence;

template <typename IS>
struct inc_index_sequence<false, IS> {
  typedef IS type;
};

template <std::size_t... i>
struct inc_index_sequence<true, index_sequence<i...>> {
  typedef index_sequence<i..., sizeof...(i)> type;
};

// helper structure for make_index_sequence.
template <std::size_t N>
struct make_index_sequence_impl
    : inc_index_sequence<
          (N % 2 != 0),
          typename doubled_index_sequence<
              N / 2, typename make_index_sequence_impl<N / 2>::type>::type> {};

// helper structure needs specialization only with 0 element.
template <>
struct make_index_sequence_impl<0> {
  typedef index_sequence<> type;
};

// OUR make_index_sequence,  gcc-4.4.7 doesn't support `using`,
// so we use struct instead of it.
template <std::size_t N>
struct make_index_sequence : make_index_sequence_impl<N>::type {};

// index_sequence_for  any variadic templates
template <typename... T>
struct index_sequence_for : make_index_sequence<sizeof...(T)> {};

// MKSEQ actual implementation.
template <typename... TypeValue>
struct mkseq_ {
  struct not_integral_constant_int {};
  template <typename... Ts>
  struct f {
    typedef error_<not_integral_constant_int> type;
  };
  template <typename T>
  struct f<T> {
    typedef typename make_index_sequence<T::value>::to_types type;
  };
};
template <typename Tval>
struct mkseq_<Tval> {
  template <typename...>
  struct f {
    using type = typename make_index_sequence<Tval::value>::to_types;
  };
};
template<std::size_t N> using mkseq_c = mkseq_<i<N>>;


// ZIP : Join together two list of type in multiple inputs
struct zip {
  template <typename...>
  struct f {
    typedef error_<zip> type;
  };
  template <//template <typename...> class F, template <typename...> class G,
            typename... Ts, typename... Us>
  struct f<ts_<Ts...>, ts_<Us...>> {
    typedef ts_<ts_<Ts, Us>...> type;
  };
  template <//template <typename...> class F, template <typename...> class G,
            //template <typename...> class H,
            typename... fs, typename... gs,
            typename... hs>
  struct f<ts_<fs...>, ts_<gs...>, ts_<hs...>> {
    typedef ts_<ts_<fs, gs, hs>...> type;
  };
};
// ZIP_INDEX
struct zip_index {
  template <typename... Ts>
  struct f_impl {
    typedef nothing type;
  };
  template <typename... Is, typename... Ts>
  struct f_impl<ts_<Is...>, ts_<Ts...>> {
    typedef ts_<ts_<Is, Ts>...> type;
  };

  template <typename... Ts>
  struct f {
    typedef
        typename f_impl<typename mkseq_<>::template f<i<sizeof...(Ts)>>::type,
                        ts_<Ts...>>::type type;
  };
};

// UNZIP_INDEX
struct unzip_index {
  struct unzip_index_not_recognized {};
  template <typename... Ts>
  struct f {
    typedef error_<unzip_index_not_recognized> type;
  };
  template <typename... Is, typename... Ts>
  struct f<ts_<Is, Ts>...> {
    typedef ts_<Ts...> type;
  };
};

// UNZIP
struct unzip {
  template <typename...>
  struct f;
  template <//template <typename...> class F,
  		   typename... Fs, typename... Gs>
  struct f<ts_<Fs, Gs>...> : ts_<ts_<Fs...>, ts_<Gs...>> {};
  template <//template <typename...> class F,
  		   typename... Fs, typename... Gs,
            typename... Hs>
  struct f<ts_<Fs, Gs, Hs>...> : ts_<ts_<Fs...>, ts_<Gs...>, ts_<Hs...>> {};
};

// ZIP_INPUT : Inputs are indexed
template<typename ... Ts>
struct zip_input_ : pipe_<input_<Ts...>,zip_index>{};

// PUSH_FRONT_ : Add anything you want to the front of the inputs.
template <typename... Ts>
struct push_front_ {
  template <typename... Inputs>
  struct f {
    typedef ts_<Ts..., Inputs...> type;
  };
};

// PUSH_BACK_ : Add anything you want to the back of the inputs
template <typename... Ts>
struct push_back_ {
  template <typename... Inputs>
  struct f {
    typedef ts_<Inputs..., Ts...> type;
  };
};

constexpr int circular_modulo(int i, int N) {
  return     (i>= 0 ) ? i % N
            : (i+(-i*N ))%N;
}

template <int I, int N>
using circular_modulo_t = std::integral_constant<int, circular_modulo(I,N)>;

// GET : Continue with the type a index N
template <int I>
struct at_c {
private:
  template <int Index, typename T>
  struct h {};
  template <typename T>
  struct h<I, T> {
    using type = T;
  };

  template <typename...>
  struct f_impl;
  template <typename... J, typename... U>
  struct f_impl<ts_<J, U>...>
      : h<J::value == circular_modulo(I, sizeof...(J)) ? I : J::value,
          U>... {};
public:
  template <typename...>
  struct f : ts_<nothing> {};

  template <typename T, typename... Ts>
  struct f<T, Ts...>
      : pipe_<zip_index,wraptype_<f_impl>>::template f<T, Ts...> {};
};

template<typename N>
using at_ = at_c<N::value>;

// FIRST : Continue with the first type
template<>
struct at_c<0> {
  template <typename... Ts>
  struct f {
    typedef nothing type;
  };
  template <typename T, typename... Ts>
  struct f<T, Ts...> {
    typedef T type;
  };
};

// SECOND : Continue with the first type
template<>
struct at_c<1> {
  template <typename... Ts>
  struct f {
    typedef nothing type;
  };
  template <typename T, typename T2, typename... Ts>
  struct f<T, T2, Ts...> {
    typedef T2 type;
  };
};

// THIRD : Continue with the third type
template<>
struct at_c<2> {
  template <typename... Ts>
  struct f {
    typedef nothing type;
  };
  template <typename T, typename T2, typename T3, typename... Ts>
  struct f<T, T2, T3, Ts...> {
    typedef T3 type;
  };
};

// LAST : Continue with the last type
struct last : at_c<-1> {};

// Nth : Continue with the Nth type
typedef at_c<0> _1st; using first = at_c<0>;
typedef at_c<1> _2nd; using second = at_c<1>;
typedef at_c<2> _3rd; using third = at_c<2>;
typedef at_c<3> _4th; using fourth = at_c<3>;
typedef at_c<4> _5th; using fifth = at_c<4>;
typedef at_c<5> _6th; using sixth = at_c<5>;
typedef at_c<6> _7th; using seventh = at_c<6>;
typedef at_c<7> _8th; using eighth = at_c<7>;
typedef at_c<8> _9th; using ninth = at_c<8>;


// FLATTEN : Continue with only one ts_. Sub-ts_ are removed.
// The dirty but necessary tool of our library
struct flatten {
  template <typename... Ts>
  struct flat;
  template <typename... Ls>
  struct flat<ts_<Ls...>> {
    typedef ts_<Ls...> type;
  };
  template <typename... Ls, typename T, typename... Ts>
  struct flat<ts_<Ls...>, T, Ts...> : flat<ts_<Ls..., T>, Ts...> {};
  template <typename... Ls, typename T, typename... Is, typename... Ts>
  struct flat<ts_<Ls...>, T, ts_<Is...>, Ts...>
      : flat<ts_<Ls..., T, Is...>, Ts...> {};

  template <typename... Ls, typename... Fs, typename... Ts>
  struct flat<ts_<Ls...>, ts_<Fs...>, Ts...> : flat<ts_<Ls..., Fs...>, Ts...> {
  };
  template <typename... Fs, typename... Gs, typename... Ls, typename... Ts>
  struct flat<ts_<Ls...>, ts_<Fs...>, ts_<Gs...>, Ts...>
      : flat<ts_<Ls..., Fs..., Gs...>, Ts...> {};
  template <typename... Fs, typename... Gs, typename... Hs, typename... Ls,
            typename... Ts>
  struct flat<ts_<Ls...>, ts_<Fs...>, ts_<Gs...>, ts_<Hs...>, Ts...>
      : flat<ts_<Ls..., Fs..., Gs..., Hs...>, Ts...> {};
  template <typename... Fs, typename... Gs, typename... Hs, typename... Is,
            typename... Ls, typename... Ts>
  struct flat<ts_<Ls...>, ts_<Fs...>, ts_<Gs...>, ts_<Hs...>, ts_<Is...>, Ts...>
      : flat<ts_<Ls..., Fs..., Gs..., Hs..., Is...>, Ts...> {};

  template <typename... Ts>
  struct f : fold_left_<wraptype_<flatten::flat>>::template f<ts_<>, Ts...> {};
};

// LENGTH : Continue with the number of types in the ts_.
struct length {
  template <typename... Ts>
  struct f {
    typedef i<sizeof...(Ts)> type;
  };
};

// SIZE : Continue with the sizeof(T). T is one input_
struct size {
  template <typename T>
  struct f {
    typedef i<sizeof(T)> type;
  };
};

// ALIGNMENT : Continue with the alignment of one input_.
struct alignment {
  template <typename T>
  struct f {
    typedef i<alignof(T)> type;
  };
};

// NOT_ : Boolean metafunction are inversed
template <typename... UnaryPredicate>
struct not_ : cond_<pipe_<UnaryPredicate...>, ts_<std::false_type>,
                    ts_<std::true_type>> {};

template <>
struct not_<> {
  template <typename T>
  struct f : ts_<std::integral_constant<decltype(!T::value), !T::value>> {};
};

// REMOVE_IF_ : Remove every type where the metafunction "returns"
// std::true_type
template<typename ...Up>
struct removeif_ : compose_null_<transform_<
					 cond_<pipe_<Up...>,input_<identity>
					,wrap_<input_append_>>>
					>{};
template<typename ... Up>
struct remove_if_
{
	template<typename ... Ts>
	struct f {
		template<typename T> using Expr = typename std::conditional<eval_pipe_<input_<T>,Up...>::value,identity,input_append_<T>>::type;
		using type = eval_pipe_<Expr<Ts>...> ;//pipe_<cond_<pipe_<Up...>,input_<identity>,wrap_<input_append_>>::template f<Ts>::type...>;
	};
};

// FILTER_ : Remove every type where the metafunction is false.
template <typename... UnaryPredicate>
struct filter_ : remove_if_<not_<UnaryPredicate...>> {};


// PARTITION_ : Continue with two list. First predicate is true, Second
// predicate is false
template <typename... UnaryPredicate>
struct partition_ : pipe_<fork_<pipe_<filter_<UnaryPredicate...>>,
                          pipe_<remove_if_<UnaryPredicate...>>>> {};


// REPLACE_IF_ : Replace the type by another if the predicate is true
template <typename UnaryPredicate, typename F>
struct replace_if_ : transform_<cond_<UnaryPredicate, F, identity>> {};

// ALL_OF_ : Thanks to Roland Bock for the inspiration
template <typename... UnaryPredicate>
struct all_of_ {
  template <typename... Ts>
  struct f {
    typedef typename std::is_same<
        ls_<b<true>,
            typename pipe_<UnaryPredicate...>::template f<Ts>::type...>,
        ls_<typename pipe_<UnaryPredicate...>::template f<Ts>::type...,
            b<true>>>::type type;
  };
};

// ANY_OF_ : Thanks to Roland Bock for the inspiration
template <typename... UnaryPredicate>
struct any_of_ {
  template <typename... Ts>
  struct f {
    typedef std::integral_constant<
        bool,
        !std::is_same<
            ls_<b<false>,
                typename pipe_<UnaryPredicate...>::template f<Ts>::type...>,
            ls_<typename pipe_<UnaryPredicate...>::template f<Ts>::type...,
                b<false>>>::value>
        type;
  };
};

// NONE_OF : Simply the inverse of any_of_
template <typename... UnaryPredicate>
struct none_of_ : not_<any_of_<UnaryPredicate...>> {};

// COUNT_IF_ : Count the number of type where the predicate is true
template <typename... F>
struct count_if_ : pipe_<filter_<F...>, length> {};

// FIND_IF_ : Return the first index that respond to the predicate, along with
// the type.
template <typename... F>
struct find_if_ : pipe_<zip_index, transform_<listify>,
                        filter_<unwrap, second, F...>, first, unwrap> {};

// CARTESIAN : Given two lists, continue with every possible lists of two types.
template <typename... BinF>  // Binary Function
struct cartesian_ {
  template <typename... Ts>
  struct f /*{*/
    //typedef ts_<Ts...> type;}
  ;
  template <typename A, typename B>
  struct f<A, B> : pipe_<BinF...>::template f<ts_<A, B>> {  };
  template <typename A, typename... B>
  struct f<A, ts_<B...>> : pipe_<transform_<BinF...>>::template f<ts_<A,B>...> {};
  template <typename... A, typename B>
  struct f<ts_<A...>, B> : pipe_<transform_<BinF...>>::template f<ts_<A,B>...> {};
  template <typename... A, typename... B>
  struct f<ts_<A...>, ts_<B...>>
  {
	typedef eval_pipe_<fork_<pipe_<ts_<A, ts_<B...>>, cartesian_<BinF...>>...>
		,flatten
		>
		type;
  };
};

// ROTATE : rotate
// The implementation may rely on undefined behavior.
// But so far clang and gcc are compliant
template <int I>
struct rotate_c {
  template <int N, typename T, typename... Ts>
  struct f_impl : f_impl<N - 1, Ts..., T> {};
  template <typename T, typename... Ts>
  struct f_impl<0, T, Ts...> {
    typedef ts_<T, Ts...> type;
  };

  template <typename... Ts>
  struct f : f_impl<I % sizeof...(Ts), Ts...> {};
};

template<typename V>
using rotate = rotate_c<V::value>;

// IS_ZERO : return if the type is it's type_zero
struct is_zero {
  template <typename... T>
  struct f {
    typedef b<false> type;
  };
  template <typename T>
  struct f<T> {
    typedef typename std::is_same<T, typename zero<T>::type>::type type;
  };
};

// detail : then_context
namespace detail {
template <bool, typename... Ts>
struct __then_context {};
}  // namespace detail

// IF, AND , OR , ENDIF
// If_ must be followed by endif. This is synthaxic sugar
template <typename... NPred>
struct if_ {
  template <typename... Ts>
  struct f {
    typedef typename cond_<
        pipe_<NPred...>, ts_<detail::__then_context<true, Ts...>>,
        ts_<detail::__then_context<false, Ts...>>>::template f<Ts...>::type
        type;
  };
};

template <typename... NPred>
struct or_if_ {
  template <typename... Ts>
  struct f {
    typedef ts_<Ts...> type;
  };
  template <typename... Ts>
  struct f<detail::__then_context<true, Ts...>> {
    typedef detail::__then_context<true, Ts...> type;
  };
  template <typename... Ts>
  struct f<detail::__then_context<false, Ts...>> {
    typedef detail::__then_context<eval_pipe_<input_<Ts...>, NPred...>::value,
                                   Ts...>
        type;
  };
};

template <typename... NPred>
struct and_if_ {
  template <typename... Ts>
  struct f {
    typedef ts_<Ts...> type;
  };
  template <typename... Ts>
  struct f<detail::__then_context<false, Ts...>> {
    typedef detail::__then_context<false, Ts...> type;
  };
  template <typename... Ts>
  struct f<detail::__then_context<true, Ts...>> {
    typedef detail::__then_context<eval_pipe_<input_<Ts...>, NPred...>::value,
                                   Ts...>
        type;
  };
};

template <typename... NPred>
struct else_if_ {
  template <typename... Ts>
  struct f {
    typedef ts_<Ts...> type;
  };
  template <typename... Ts>
  struct f<detail::__then_context<false, Ts...>> {
    typedef detail::__then_context<eval_pipe_<input_<Ts...>, NPred...>::value,
                                   Ts...>
        type;
  };
  template <typename... Ts>
  struct f<detail::__then_context<true, Ts...>> {
    typedef error_<else_if_, Ts...> type;
  };
};

template <typename... Es>
struct then_ {
  template <typename... Ts>
  struct f {
    typedef ts_<Ts...> type;
  };
  template <typename... Ts>
  struct f<detail::__then_context<false, Ts...>> {
    typedef detail::__then_context<false, Ts...> type;
  };
  template <typename... Ts>
  struct f<detail::__then_context<true, Ts...>> {
    typedef eval_pipe_<input_<Ts...>, Es...> type;
  };
};

template <typename... Es>
struct else_ {
  template <typename... Ts>
  struct f {
    typedef ts_<Ts...> type;
  };
  template <typename... Ts>
  struct f<detail::__then_context<false, Ts...>> {
    typedef eval_pipe_<input_<Ts...>, Es...> type;
  };
  template <typename... Ts>
  struct f<detail::__then_context<true, Ts...>> {
    typedef error_<else_, Ts...> type;
  };
};

struct endif {
  template <typename... Ts>
  struct f {
    typedef ts_<Ts...> type;
  };
  template <bool B, typename... Ts>
  struct f<detail::__then_context<B, Ts...>> {
    typedef ts_<Ts...> type;
  };
};

template <typename UnaryPredicate, typename ... F>
struct if_then_ : cond_<UnaryPredicate, pipe_<F...>, identity> {};

// ARITHMETIC METAFUNCTIONS
//
// post_operator : Internal type
template <template <typename...> class CRTP, typename... Ts>
struct post_op {
  template <typename... Us>
  struct f {
    using type = typename CRTP<Us..., Ts...>::type;
  };
};
// prefix_operator : Internal type
template <template <typename...> class CRTP, typename... Ts>
struct pre_op {
  template <typename... Us>
  struct f {
    using type = typename CRTP<Ts..., Us...>::type;
  };
};

// LESS
template<typename ... Ts>
struct less_ : post_op<less_,Ts...>{};
template<typename T, typename U>
struct less_<T,U> : ts_<decltype(std::declval<T>() < std::declval<U>())>{};
template<typename I0, I0 v0, typename I1, I1 v1>
struct less_<std::integral_constant<I0,v0>,std::integral_constant<I1,v1>> : ts_<std::integral_constant<decltype(v0 < v1), (v0 < v1)>>{};
template<intmax_t N0, intmax_t D0,intmax_t N1, intmax_t D1>
struct less_<std::ratio<N0,D0>,std::ratio<N1,D1>>: ts_<std::ratio_less<std::ratio<N0,D0>,std::ratio<N1,D1>>>{};
static_assert(eval_pipe_<input_<i<1>>, less_<i<2>>>::value, "");
static_assert(eval_pipe_<less_<i<1>, i<2>>>::value, "");
static_assert(eval_pipe_<input_<i<1>, i<2>>, less_<>>::value, "");

// LESS_EQ
template <typename... Ts>
struct less_eq_ : post_op<less_eq_, Ts...> {};
template <typename T, typename U>
struct less_eq_<T, U> : ts_<decltype(std::declval<T>() <= std::declval<U>())>{};
template<typename I0, I0 v0, typename I1, I1 v1>
struct less_eq_<std::integral_constant<I0,v0>,std::integral_constant<I1,v1>> : ts_<std::integral_constant<decltype(v0 <= v1), (v0 <= v1)>>{};
template<intmax_t N0, intmax_t D0,intmax_t N1, intmax_t D1>
struct less_eq_<std::ratio<N0,D0>,std::ratio<N1,D1>>: ts_<std::ratio_less_equal<std::ratio<N0,D0>,std::ratio<N1,D1>>>{};

// GREATER
template <typename... Ts>
struct greater_ : post_op<greater_, Ts...> {};
template <typename T, typename U>
struct greater_<T, U> : ts_<decltype(std::declval<T>() > std::declval<U>())>{};
template<typename I0, I0 v0, typename I1, I1 v1>
struct greater_<std::integral_constant<I0,v0>,std::integral_constant<I1,v1>> : ts_<std::integral_constant<decltype(v0 > v1), (v0 > v1)>>{};
template<intmax_t N0, intmax_t D0,intmax_t N1, intmax_t D1>
struct greater_<std::ratio<N0,D0>,std::ratio<N1,D1>>: ts_<std::ratio_greater<std::ratio<N0,D0>,std::ratio<N1,D1>>>{};

// GREATER
template <typename... Ts>
struct greater_eq_ : post_op<greater_eq_, Ts...> {};
template <typename T, typename U>
struct greater_eq_<T, U> : ts_<decltype(std::declval<T>() > std::declval<U>())>{};
template<typename I0, I0 v0, typename I1, I1 v1>
struct greater_eq_<std::integral_constant<I0,v0>,std::integral_constant<I1,v1>> : ts_<std::integral_constant<decltype(v0 > v1), (v0 > v1)>>{};
template<intmax_t N0, intmax_t D0,intmax_t N1, intmax_t D1>
struct greater_eq_<std::ratio<N0,D0>,std::ratio<N1,D1>>: ts_<std::ratio_greater_equal<std::ratio<N0,D0>,std::ratio<N1,D1>>>{};


// PLUS
template<typename ... Ts>
struct plus_ : post_op<plus_,Ts...>{};
template<typename T, typename U>
struct plus_<T,U> : ts_<decltype(std::declval<T>() + std::declval<U>())>{};
template<typename I0, I0 v0, typename I1, I1 v1>
struct plus_<std::integral_constant<I0,v0>,std::integral_constant<I1,v1>> : std::integral_constant<decltype(v0 + v1), v0 + v1>{};
template<intmax_t N0, intmax_t D0,intmax_t N1, intmax_t D1>
struct plus_<std::ratio<N0,D0>,std::ratio<N1,D1>> { using type = std::ratio_add<std::ratio<N0,D0>,std::ratio<N1,D1>>;};

// MINUS
template<typename ... Ts>
struct minus_ : post_op<minus_,Ts...>{};
template<typename T, typename U>
struct minus_<T,U> : ts_<decltype(std::declval<T>() - std::declval<U>())>{};
template<typename I0, I0 v0, typename I1, I1 v1>
struct minus_<std::integral_constant<I0,v0>,std::integral_constant<I1,v1>> : std::integral_constant<decltype(v0 - v1), v0 - v1>{};
template<intmax_t N0, intmax_t D0,intmax_t N1, intmax_t D1>
struct minus_<std::ratio<N0,D0>,std::ratio<N1,D1>> { using type = std::ratio_subtract<std::ratio<N0,D0>,std::ratio<N1,D1>>;};


// MULTIPLY
template<typename ... Ts>
struct multiply_ : post_op<multiply_,Ts...>{};
template<typename T, typename U>
struct multiply_<T,U> : ts_<decltype(std::declval<T>() * std::declval<U>())>{};
template<typename I0, I0 v0, typename I1, I1 v1>
struct multiply_<std::integral_constant<I0,v0>,std::integral_constant<I1,v1>> : std::integral_constant<decltype(v0 * v1), v0 * v1>{};
template<intmax_t N0, intmax_t D0,intmax_t N1, intmax_t D1>
struct multiply_<std::ratio<N0,D0>,std::ratio<N1,D1>> { using type = std::ratio_multiply<std::ratio<N0,D0>,std::ratio<N1,D1>>;};

// DIVIDE
template<typename ... Ts>
struct divide_ : post_op<divide_,Ts...>{};
template<typename T, typename U>
struct divide_<T,U> : ts_<decltype(std::declval<T>() / std::declval<U>())>{};
template<typename I0, I0 v0, typename I1, I1 v1>
struct divide_<std::integral_constant<I0,v0>,std::integral_constant<I1,v1>> : std::integral_constant<decltype(v0 / v1), v0 / v1>{};
template<intmax_t N0, intmax_t D0,intmax_t N1, intmax_t D1>
struct divide_<std::ratio<N0,D0>,std::ratio<N1,D1>> { using type = std::ratio_divide<std::ratio<N0,D0>,std::ratio<N1,D1>>;};

// EQUAL
template<typename ... Ts>
struct equal_ : post_op<equal_,Ts...>{};
template<typename T, typename U>
struct equal_<T,U> : ts_<decltype(std::declval<T>() == std::declval<U>())>{};
template<typename I0, I0 v0, typename I1, I1 v1>
struct equal_<std::integral_constant<I0,v0>,std::integral_constant<I1,v1>> : std::integral_constant<decltype(v0 == v1), v0 == v1>{};
template<intmax_t N0, intmax_t D0,intmax_t N1, intmax_t D1>
struct equal_<std::ratio<N0,D0>,std::ratio<N1,D1>> { using type = std::ratio_equal<std::ratio<N0,D0>,std::ratio<N1,D1>>;};

// MODULO 
template <typename... Ts>
struct modulo_ {
  typedef error_<modulo_<Ts...>> type;
};
template <typename P>
struct modulo_<P> {
  template <typename... Ts>
  struct f;
  template <typename I>
  struct f<I> {
    typedef decltype(std::declval<I>() % std::declval<P>()) type;
  };
};
template <typename T, T t>
struct modulo_<std::integral_constant<T, t>> {
  template <typename>
  struct f;
  template <typename U, U u>
  struct f<std::integral_constant<U, u>> {
    typedef std::integral_constant<decltype(u % t), u % t> type;
  };
};
template <intmax_t nA, intmax_t dA>
struct modulo_<std::ratio<nA, dA>> {
  template <typename...>
  struct f {
    typedef error_<plus_<std::ratio<nA, dA>>> type;
  };
};
template <>
struct modulo_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() % std::declval<B>()) type;
  };
  template <typename T, typename U, T t, U u>
  struct f<std::integral_constant<T, t>, std::integral_constant<U, u>> {
    typedef std::integral_constant<decltype(t % u), t % u> type;
  };
};

struct gcd {
  template <typename...>
  struct f {
    typedef error_<gcd> type;
  };
  template <typename T>
  struct f<T, typename zero<T>::type> {
    typedef T type;
  };
  template <typename T, typename U>
  struct f<T, U> : f<U, typename modulo_<>::template f<T, U>::type> {};
  template <typename T>
  struct f<T, T> {
    typedef T type;
  };
};

struct lcm {
  template <typename...>
  struct f;
  template <typename T, typename U>
  struct f<T, U> {
    typedef typename divide_<>::template f<
        typename multiply_<>::template f<T, U>::type,
        typename gcd::template f<T, U>::type>::type type;
  };
};

// SORT : Given a binary predicate, sort the types
// note : it's implicit that you receive two types, so you probably need to
// transform them. eg : sort by size : sort_<transform_<size>, greater_<> >


template <typename... BinaryPredicate>
struct sort_ {
	template <typename A, typename B>
	struct eager {
  		typedef ts_<> type;
	};

	template <typename A>
	struct eager<A, std::true_type> {
  		typedef ts_<A> type;
	};

	template <typename... L>
	struct sort_impl;
	template <typename T>
	struct sort_impl<T> {
    	typedef T type;
	};

	template <typename... Ts, typename F>
	struct sort_impl<ts_<F, Ts...>> {
    	typedef typename sort_impl<typename flatten::template f<
        	ts_<>,
        	typename eager<Ts, typename pipe_<BinaryPredicate...>::template f<
            	Ts, F>::type>::type...>::type>::type Yes_types;
    	typedef typename sort_impl<typename flatten::template f<
        	ts_<>,
        	typename eager<Ts, typename not_<pipe_<BinaryPredicate...>>::template f<
            	Ts, F>::type>::type...>::type>::type No_types;

    	typedef typename flatten::template f<ts_<>, Yes_types, F, No_types>::type type;
	};
	template <typename... Ts>
	struct f {
    	typedef typename sort_impl<ts_<Ts...>>::type type;
	};
};

// APPEND_RESULT
template<typename ...Es>
struct append_result_ : compose_<Es...,wrap_<push_back_>>{};

// PREPEND_RESULT
template<typename ...Es>
struct prepend_result_ : compose_<Es...,wrap_<push_front_>>{};

// GROUP_RANGE
// Recursively partition into two groups which result of those UnaryFunction is
// the same as the first. Basically it group all the types that have the same
// result into subrange.
template<typename ... Uf>
struct group_range_
{
    template<typename ... Ts> struct ff;
    template<typename ... Ts>  struct gs_;

    template<typename T>
    using SameAsExpr = te::eval_pipe_<te::ts_<T>,Uf...,te::wrap_<te::same_as_>>;
    template<typename T0,typename ... Ts>
    struct f : ff<gs_<T0,Ts...>>{};
    template<typename T0,typename ... Ts,typename ... Rs>
    struct ff<gs_<T0,Ts...>,Rs...>:
        ff< te::eval_pipe_<te::input_<Ts...>,te::remove_if_<Uf...,SameAsExpr<T0>>,te::wrap_<gs_>>
        ,   Rs...
        ,   te::eval_pipe_<te::input_<te::ls_<T0>,te::ls_<Ts>...>,te::filter_<te::unwrap,Uf...,SameAsExpr<T0>>,te::transform_<te::unwrap>>
        >{};
    template<typename ... RL>
    struct ff<gs_<>, RL...> {using type = te::ts_<RL...>;};
};

// UNIQUE : Keep only one of each different types
// The implementation is special but work.
struct unique : pipe_<group_range_<>, transform_<first>> {};

// GROUP : Given a Unary Function, Gather those that give the same result
template <typename... UnaryFunction>
struct group_by_ : pipe_<group_range_<UnaryFunction...>, flatten> {};

// COPY_ : Copy N times the inputs.
// Implemented as a higher meta-expression
template <unsigned int N>
struct copy_ : compose_< mkseq_<i<N>>, transform_<ts_<identity>>,
                          quote_<fork_>> {};

// REPEAT_ : Repeat N times the meta-expression
template <std::size_t N, typename... Es>
struct repeat_ : eval_pipe_<input_<Es...>, copy_<N>, flatten, quote_<pipe_>> {};

// SWIZZLE : Restructure the inputs using the index
template <int ... Is>
struct swizzle_ : fork_<at_c<Is>...> {};

// ON_ARGS_ : unwrap rewrap in the same template template.
template <typename... Es>
struct on_args_ {
  template <typename... Ts>
  struct f : ts_<error_<on_args_<Es...>, Ts...>> {};
  template <template <typename... Ts> class F, typename... Ts>
  struct f<F<Ts...>> {
    typedef eval_pipe_<input_<Ts...>, Es..., quote_<F>> type;
  };
  template <template <typename... Ts> class F, typename... Ts>
  struct f<F<Ts...> &> {
    typedef eval_pipe_<input_<Ts...>, Es..., quote_<F>,
                       wraptype_<std::add_lvalue_reference>>
        type;
  };
};

// ARRAYIFY
// If all types receive are the same, continue with std::array with the correct
// size.
struct arrayify {
  struct not_all_types_received_are_the_same;
  template <bool b, typename Head, typename... Rest>
  struct f_impl
      : error_<arrayify, not_all_types_received_are_the_same, Head, Rest...> {};
  template <typename Head, typename... Rest>
  struct f_impl<true, Head, Rest...> {
    typedef std::array<Head, sizeof...(Rest) + 1> type;
  };

  template <typename... Ts>
  struct f;
  template <typename Head, typename... Rest>
  struct f<Head, Rest...>
      : f_impl<same_as_<Head, Rest...>::template f<Rest..., Head>::type::value,
               Head, Rest...> {};
};

// BIND
template <int I, typename... Es>
struct bind_ {
  template <typename... Ts>
  struct f : eval_pipe_<
                 input_<i<sizeof...(Ts)>>, mkseq_<>,
                 transform_<cond_<same_as_<circular_modulo_t<I, sizeof...(Ts)>>,
                                  ts_<pipe_<Es...>>, ts_<identity>>>,
                 quote_<each_>>::template f<Ts...> {};
};

// BIND_ON_ARGS_
template <int I, typename... Es>
struct bind_on_args_ {
  template <typename... Ts>
  struct f
      : eval_pipe_<
            input_<i<sizeof...(Ts)>>, mkseq_<>,
            transform_<cond_<same_as_<circular_modulo_t<I, sizeof...(Ts)>>,
                             ts_<pipe_<ts_<Ts...>, Es...>>, ts_<identity>>>,
            quote_<each_>>::template f<Ts...> {};
};

template<int I, typename T>
struct insert_c : compose_null_<zip_index, transform_<listify>
				  , partition_<unwrap,first,less_<i<I>>>
				,fork_<pipe_<_1st,transform_<unwrap,_2nd>>
					  ,input_<T>
					  , pipe_<_2nd,transform_<unwrap,_2nd>>>
				,each_<
					wrap_<input_>
					, wrap_<push_back_>
					, wrap_<push_back_>>
					>
{};
template<typename V, typename T> using insert_ = insert_c<V::value,T>;

template<int I>
struct erase_c : compose_null_<
			   zip_index,transform_<listify>,remove_if_<unwrap,first,equal_<i<I>>>
				, transform_<unwrap,_2nd>,wrap_<input_>>
			{};

template<typename V> using erase_ = erase_c<V::value>;


};  // namespace te
#endif
