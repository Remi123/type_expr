//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
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

template<typename T>
struct type_identity {using type = T;};

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

// PUSH_FRONT_ : Add anything you want to the front of the inputs.
template <typename... Ts>
struct push_front_ {
  template <typename... Inputs>
  struct f {
    typedef ts_<Ts..., Inputs...> type;
  };
};

// DROP_FRONT : Remove the first element
struct drop_front {
    template<typename T, typename ... Ts>
    struct f : te::ts_<Ts...>{};
};

// PUSH_BACK_ : Add anything you want to the back of the inputs
template <typename... Ts>
struct push_back_ {
  template <typename... Inputs>
  struct f {
    typedef ts_<Inputs..., Ts...> type;
  };
};

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

template<typename KvasirMetaClosure>
using kv_ = wrap_<KvasirMetaClosure::template f>;

//template <typename... Ts>
//struct input_<metal::list<Ts...>> : ts_<Ts...> {};

//template <typename... Ts>
//struct input_<boost::mp11::mp_list<Ts...>> : ts_<Ts...> {};

//template <typename... Ts>
/*struct input_<boost::fusion::list<Ts...>> : ts_<Ts...> {};*/


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
  using f =  fold_left_<wraptype_<pipe_context>>::template f<ts_<Ts...>, Es...>;  
  // No ::type a la type_traits. The problem since it's always instanciated even if not
  // asked. required to have an alias eval_pipe_ = typename
  // pipe_<Fs...>::template f<>::type; to instanciate to the result type;
  // As a result, pipe_::type is itself
  using type = pipe_;

  template <typename... Us>
  constexpr pipe_<Es..., Us...> const operator|(const pipe_<Us...> &) const {
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
	using f =  std::is_same<ts_<Ts...>, ts_<Us...>>;
};

// TRANSFORM_ :
// Similar to haskell's map. Also similar to std::transform
template <typename... Es>
struct for_each_ {
  template <typename... Ts>
  struct f {
    typedef ts_<eval_pipe_<ts_<Ts>,Es...>...> type;
  };
};
template<typename ... Es> using transform_ = for_each_<Es...>;
template<typename ... Es> using map_ = for_each_<Es...>;

// FORK_ : Inputs are copied to each metafunctions
// The Peanut Butter of the library
template <typename... Es>
struct fork_ {
  template <typename... Ts>
  struct f {
    typedef ts_<eval_pipe_<ts_<Ts...>, Es>...> type;
  };
};

// EACH : Badly named mix between fork and transform. Requiere the same number
// of expressions as arguments than inputs.
template <typename... Es>
struct each_ {
  template <typename... Ts>
  struct f {
    typedef ts_<eval_pipe_<ts_<Ts>, Es>...> type;
  };
};

// WRITE_ : Construct a function using the inputs, then evaluate using the
// inputs. Without a doubt the most powerful function of my library
template <typename... PipableExpr>
struct write_ {
  template <typename... Ts>
  struct f {
    using metaexpr = eval_pipe_<ts_<Ts...>, PipableExpr..., quote_<pipe_>>;
    using type = typename metaexpr::template f<Ts...>::type;
  };
};

template<typename ... PipableExpr>
struct write_debug_
{
	template <typename... Ts>
  struct f {
    using type = eval_pipe_<ts_<Ts...>, PipableExpr..., quote_<pipe_>>;
  };
};

// WRITE_NULL_ : Construct a function using the inputs, then evaluate from nothing
// A variation where we don't use the inputs
template <typename... PipableExpr>
struct write_null_ {
  template <typename... Ts>
  struct f {
    using metaexpr = eval_pipe_<ts_<Ts...>, PipableExpr..., quote_<pipe_>>;
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

// post_operator : Internal type
template <template <typename...> class CRTP, typename... Ts>
struct post_op {
  template <typename... Us>
	struct f {using type = typename CRTP<Us..., Ts...>::type;};
};
// prefix_operator : Internal type
template <template <typename...> class CRTP, typename... Ts>
struct pre_op {
  template <typename... Us>
  struct f {
    using type = typename CRTP<Ts..., Us...>::type;
  };
};

namespace detail {
    template <bool b, int HalfN, typename Is>
    struct expanding;
    template <bool b, int HalfN, typename... Is>
    struct expanding<b, HalfN, ts_<Is...>> {
    using type =
        te::ts_<Is..., std::integral_constant<int, (Is::value + HalfN)>...>;
    };
    template <int HalfN, typename... Is>
    struct expanding<true, HalfN, ts_<Is...>> {
    using type =
        te::ts_<Is...,  std::integral_constant<int, (Is::value + HalfN)>...,
                    std::integral_constant<int, sizeof...(Is) * 2>>;
    };
}  // namespace detail

template<typename ... N>
struct mkseq_ : post_op<mkseq_,N...>{};

template<int N>
struct mkseq_<std::integral_constant<int,N>> 
{
	using current_sequence = typename mkseq_<std::integral_constant<int,N/2>>::type;
	using type_impl = typename detail::expanding<N % 2, N / 2, current_sequence>::type;
	using type = type_impl;
	template<typename ... > struct f {using type = type_impl;}; 
};

template<> struct mkseq_<std::integral_constant<int,0>>
{
    using type = te::ts_<>;
	template<typename ... > using f = ts_<ts_<>>; 
};
template<typename ValueType>
struct mkseq_<ValueType> : mkseq_<te::i<ValueType::value>>{};

template<std::size_t N> using mkseq_c = mkseq_<i<N>>;
template<std::size_t N> using iota = mkseq_<i<N>>;
template<std::size_t N> using iota_c = mkseq_c<N>;


// ZIP : Join together two list of type in multiple inputs
struct zip {
  template <typename...>
  struct f {
    typedef error_<zip> type;
  };
  template < typename... Ts, typename... Us>
  struct f<ts_<Ts...>, ts_<Us...>> {
    typedef ts_<ts_<Ts, Us>...> type;
  };
  template <typename... fs, typename... gs,
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
  template <typename... Fs, typename... Gs>
  struct f<ts_<Fs, Gs>...> : ts_<ts_<Fs...>, ts_<Gs...>> {};
  template <typename... Fs, typename... Gs, typename... Hs>
  struct f<ts_<Fs, Gs, Hs>...> : ts_<ts_<Fs...>, ts_<Gs...>, ts_<Hs...>> {};
};

// ZIP_INPUT : Inputs are indexed
template<typename ... Ts>
struct zip_input_ : pipe_<ts_<Ts...>,zip_index>{};


// Fct : Fibonacci
template<typename Type>
constexpr Type _fibonacci(Type n, bool t, Type a, Type b)
{
	return n? fibonacci(n-1,!t,a+(!t?b:0),(t?a:0)+b):(t?a:b);
}
template<typename Type>
constexpr Type fibonacci(Type n)
{
	return _fibonacci(n,false,Type(0),Type(1));
}

// Fct : Circular modulo.
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
  template<typename T>  struct f<T> : ts_<T>{};
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
  template<typename T>  struct f<T> : ts_<T>{};
  template<typename T,typename T2>  struct f<T,T2> : ts_<T>{};
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
struct flatten : write_null_<transform_<wrap_<input_append_>>>
{};

// LENGTH : Continue with the number of types in the ts_.
struct length {
  template <typename... Ts>
  struct f {
    typedef i<sizeof...(Ts)> type;
  };
};

// SIZE : Continue with the sizeof(T). T is one input_
namespace detail{
	template<typename T>
	struct size_impl{using type = te::i<sizeof(T)>;};
	template<>
	struct size_impl<void> {using type = te::i<0>;};
}
struct size {
  template <typename T>
  struct f: detail::size_impl<T>{};
};
//struct size::f<void> { using type = i<0>;};

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
template<typename ... Up>
struct remove_if_
{
	template<typename ... Ts>
	struct f {
		template<typename T> using Expr = typename std::conditional<eval_pipe_<ts_<T>,Up...>::value,identity,input_append_<T>>::type;
		using type = eval_pipe_<Expr<Ts>...> ;
	};
};

// FILTER_ : Remove every type where the metafunction is false.
template <typename... UnaryPredicate>
struct filter_ : remove_if_<not_<UnaryPredicate...>> {};

//KEEP_IF_ : Same as filter_ but with better name
template<typename ... UnaryPredicate>
struct keep_if_ : remove_if_<UnaryPredicate..., not_<>>{};

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
namespace te_impl{
struct impl_cartesian
{
    template<typename T, typename U>
    struct f;
    template<typename T, typename U>
    struct f<ts_<T>,ts_<U>>
    {
        using type = ts_<ts_<T,U>>;
    };
    template<typename T, typename ... Us>
    struct f<ts_<T>,ts_<Us...>>
    {
        using type = eval_pipe_<ts_<T>,fork_<push_back_<Us>...>>;
    };
    template<typename ... Ts, typename ... Us>
    struct f<ts_<Ts...>,ts_<Us...>>
    {
        using type = eval_pipe_<ts_<Ts...>,transform_<fork_<push_back_<Us>...>>,flatten>;
    };
};

template<typename T>
struct as_list{using type = ts_<T>;};
template<typename ... Ts>
struct as_list<ts_<Ts...>>{using type = ts_<Ts...>;};
};

struct cartesian 
{
template<typename ... Ts>
struct f{
    using type = eval_pipe_<input_<typename te_impl::as_list<Ts>::type ...>,fold_left_<te_impl::impl_cartesian>>;
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

template <typename UnaryPredicate, typename ... F>
struct if_then_ : cond_<UnaryPredicate, pipe_<F...>, identity> {};

// ARITHMETIC METAFUNCTIONS
//
//
// LESS
template<typename ... Ts>
struct less_ : post_op<less_,Ts...>{};
template<typename T, typename U>
struct less_<T,U> : ts_<decltype(std::declval<T>() < std::declval<U>())>{};
template<typename I0, I0 v0, typename I1, I1 v1>
struct less_<std::integral_constant<I0,v0>,std::integral_constant<I1,v1>> : ts_<std::integral_constant<decltype(v0 < v1), (v0 < v1)>>{};
template<intmax_t N0, intmax_t D0,intmax_t N1, intmax_t D1>
struct less_<std::ratio<N0,D0>,std::ratio<N1,D1>>: ts_<std::ratio_less<std::ratio<N0,D0>,std::ratio<N1,D1>>>{};

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
struct modulo_ : post_op<modulo_,Ts...> {};
template <typename I,typename P>
struct modulo_<I,P> : ts_<decltype(std::declval<I>() % std::declval<P>())>{};
template <typename T, T t, typename U, U u>
struct modulo_< std::integral_constant<T, t>, std::integral_constant<U,u>> 
: ts_<std::integral_constant<decltype(t % u), t % u>> {  };



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

template<typename ... BP>
struct sort_
{
	template<typename ...> struct f: ts_<> {};

	template<typename T>
		struct f<T>
		{
			using type = T;
		};

	template<typename T, typename U>
		struct f<T,U> : cond_<pipe_<BP...>,ts_<T,U>, ts_<U,T>>::template f<T,U> {};

	template<typename T, typename ... Ts>
		struct f<T,Ts...>
		{
			using left = eval_pipe_<ts_<Ts...>,
		  		  keep_if_<fork_<identity,ts_<T>>,BP...>, // See note A below
		  		  sort_<BP...>, // Recursive algo
		  		  wrap_<input_append_>>; // See note B below
			using right = eval_pipe_<ts_<Ts...>,
		  		  remove_if_<fork_<identity,ts_<T>>,BP...>, // See note A below
		  		  sort_<BP...>, // Recursive algo
		  		  wrap_<input_append_>>; // See note B below
			using type = eval_pipe_<left,input_append_<T>,right>;

		};
	/* note A : To adapt a keep_if/remove_if to a binary predicate, the first
	 * type of the sequence f<T,Ts...> is appended and integrated to the binary
	 * predicate <BP...>. the sequence of transformation in the BP is this
	 * InputType -> ts_<InputType,T> -> BP...
	 *
	 * note B : The trick of this algo is to create a meta-expression that append
	 * the results to a sequence of types. input_append is basically a push_back.
	 * So left and right result in input_append_<ResultTypes...>, with Resultypes being
	 * the result of keep_if and remove_if respectively.
	 *
	 * In ::type, we join the three meta-expression and evaluate the result
	 * */
};
template<>
struct sort_<> : sort_<less_<>>{}; // Default BinaryPredicate is less.

// Nth_ELEMENT
template<typename N,typename ... Bp>
struct nth_element_ : pipe_<sort_<Bp...>,at_<N>>{};

template<int N,typename ... Bp>
using nth_element_c = nth_element_<i<N>,Bp...>;

// APPEND_RESULT
template<typename ...Es>
struct append_result_ : write_<Es...,wrap_<push_back_>>{};

// PREPEND_RESULT
template<typename ...Es>
struct prepend_result_ : write_<Es...,wrap_<push_front_>>{};

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
struct copy_ : write_< mkseq_<i<N>>, transform_<ts_<identity>>,
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
            ts_<i<sizeof...(Ts)>>, mkseq_<>,
            transform_<cond_<same_as_<circular_modulo_t<I, sizeof...(Ts)>>,
                             ts_<pipe_<ts_<Ts...>, Es...>>, ts_<identity>>>,
            quote_<each_>>::template f<Ts...> {};
};

template<int I, typename T>
struct insert_c : write_null_<zip_index, transform_<listify>
				  , partition_<unwrap,first,less_<i<I>>>
				,fork_<pipe_<_1st,transform_<unwrap,_2nd>,wrap_<input_append_>>
					  ,ts_<input_append_<T>>
					  , pipe_<_2nd,transform_<unwrap,_2nd>>, wrap_<input_append_>>
					>
{};
template<typename V, typename T> using insert_ = insert_c<V::value,T>;

template<int I>
struct erase_c : write_null_<
			   zip_index,transform_<listify>,remove_if_<unwrap,first,equal_<i<I>>>
				, transform_<unwrap,_2nd>,wrap_<input_>>
			{};

template<typename V> using erase_ = erase_c<V::value>;

namespace te_impl{
template<typename ... Ts>
struct power_set_impl;
template<typename T>
struct power_set_impl<T> : ts_<ts_<>,ts_<T>>{};
template<typename T,typename ...Ts>
struct power_set_impl<T,Ts...>
{
	using r1 = eval_pipe_<ts_<Ts...>,wraptype_<te_impl::power_set_impl>,wrap_<input_>>;
	using type = eval_pipe_<r1,fork_<identity,transform_<push_front_<T>>>,flatten>;
};
template<typename T,typename U>
struct power_set_impl<T,U> : ts_<ts_<>,ts_<T>,ts_<U>,ts_<T,U>>{};

};

// POWER_SET : Same as mp11, but the order is different
struct power_set : wraptype_<te_impl::power_set_impl>{};

// FOLD_RIGHT : Fold right instead of left.
template<typename ... BF> using fold_right_ = te::pipe_<te::reverse,te::fold_left_<BF...>>;

// FOLD_LEFT_LIST_ : Same as fold_left, but accumulate the result into a list 
template<typename ... BF>
struct fold_left_list_
{
private :
    template<typename T,typename B> struct _f;
    template<typename T,typename ... Ts, typename B> struct _f<ts_<T,Ts...>,B>
    {
        using result = eval_pipe_<input_<T,B>,BF...>;
        using type = ts_<result,Ts...,result>;
    };
        
public :
    template<typename T,typename ... Ts> 
    struct f : pipe_<fold_left_<wraptype_<_f>>,drop_front>::template f<ts_<T>,Ts...>{};
};

};  // namespace te
#endif
