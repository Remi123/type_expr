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

namespace te {

// -------------------------------------------------------
// FIRST CLASS CITIZEN OF TYPE_EXPR
// -------------------------------------------------------

// INPUT : Universal type container of types (aka typelist).
template <typename... Ts>
struct input_ {
  typedef input_ type;
  template <typename...>
  struct f {
    typedef input_<Ts...> type;
  };
};
template <typename T>
struct input_<T> {
  typedef T type;
  template <typename... Ts>
  struct f {
    typedef T type;
  };
};
template <typename... Ts>
struct input_<input_<Ts...>> : input_<Ts...> {};

// NOTHING : Universal representation of the concept of nothing
using nothing = input_<>;

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

// IDENTITY : Continue with whatever the input_s were.
struct identity {
  template <typename... Ts>
  struct f : input_<Ts...> {};
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

// LIFT_ : Universal customization point using template template. Get the ::type
// The Farming field of our library
template <template <typename...> class F>
struct lift_ {
  template <typename... Ts>
  struct f {
    typedef typename F<Ts...>::type type;
  };
};
// QUOTE_ : Universal wrapper. Doesn't get the ::type. Use with template alias
// The Other Farming field of our library
template <template <typename...> class F>
struct quote_ {
  template <typename... Ts>
  struct f {
    typedef F<Ts...> type;
  };
};

// QUOTE_STD_INTEGER_SEQUENCE
// Specialization for std::integer_sequence
template <typename T>
struct quote_std_integer_sequence_ {
  template <typename... Is>
  struct f {
    typedef std::integer_sequence<T, Is::value...> type;
  };
};

// QUOTE_STD_ARRAY
// Specialization for std::array
struct quote_std_array {
  template <typename T, typename N>
  struct f {
    typedef std::array<int, N::value> type;
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

// FOLD_LEFT_ : Fold expression
// The Farmer of the library
template <typename F>
struct fold_left_ {
  template <typename... Ts>
  struct f {
    typedef error_<fold_left_<F>, Ts...> type;
  };
  template <typename A>
  struct f<A> {
    typedef A type;
  };

  template <typename... A>
  struct f<input_<A...>> {
    typedef typename input_<A...>::type type;
  };
  // helper alias to use with the monster below
  template <typename A, typename B>
  using f_impl = typename F::template f<A, B>::type;

  template <typename T0, typename T1, typename... Ts>
  struct f<T0, T1, Ts...> : f<f_impl<T0, T1>, Ts...> {};
  template <typename T0, typename T1, typename T2, typename... Ts>
  struct f<T0, T1, T2, Ts...> : f<f_impl<f_impl<T0, T1>, T2>, Ts...> {};
  template <typename T0, typename T1, typename T2, typename T3, typename... Ts>
  struct f<T0, T1, T2, T3, Ts...>
      : f<f_impl<f_impl<f_impl<T0, T1>, T2>, T3>, Ts...> {};
  template <typename T0, typename T1, typename T2, typename T3, typename T4,
            typename... Ts>
  struct f<T0, T1, T2, T3, T4, Ts...>
      : f<f_impl<f_impl<f_impl<f_impl<T0, T1>, T2>, T3>, T4>, Ts...> {};
  template <typename T0, typename T1, typename T2, typename T3, typename T4,
            typename T5, typename... Ts>
  struct f<T0, T1, T2, T3, T4, T5, Ts...>
      : f<f_impl<f_impl<f_impl<f_impl<f_impl<T0, T1>, T2>, T3>, T4>, T5>,
          Ts...> {};
  template <typename T0, typename T1, typename T2, typename T3, typename T4,
            typename T5, typename T6, typename... Ts>
  struct f<T0, T1, T2, T3, T4, T5, T6, Ts...>
      : f<f_impl<f_impl<f_impl<f_impl<f_impl<f_impl<T0, T1>, T2>, T3>, T4>, T5>,
                 T6>,
          Ts...> {};
  template <typename T0, typename T1, typename T2, typename T3, typename T4,
            typename T5, typename T6, typename T7, typename... Ts>
  struct f<T0, T1, T2, T3, T4, T5, T6, T7, Ts...>
      : f<f_impl<
              f_impl<f_impl<f_impl<f_impl<f_impl<f_impl<T0, T1>, T2>, T3>, T4>,
                            T5>,
                     T6>,
              T7>,
          Ts...> {};
};

// PIPE_EXPR : Internal-only. Take a type and send it as input_ to the next
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
struct pipe_context<input_<Ts...>, G> {
  typedef typename G::template f<Ts...>::type type;
};
template <typename ErrorT, typename G>
struct pipe_context<error_<ErrorT>, G> {
  typedef error_<ErrorT> type;
};

// PIPE_ : Universal container of metafunction.
// The Bread and Butter of the library
template <typename... Cs>
struct pipe_;

// PIPE_ : Helper alias
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
        typename fold_left_<lift_<pipe_context>>::template f<input_<Ts...>,
                                                             Es...>::type type;
  };
  // No ::type. This is a problem since it's always instanciated even if not
  // asked. required to have an alias eval_pipe_ = typename
  // pipe_<Fs...>::template f<>::type; to instanciate to the result type;

  template <typename... Ts, typename... Us>
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
    typedef typename std::is_same<input_<Ts...>, input_<Us...>>::type type;
  };
};
// TRANSFORM_ :
// Similar to haskell's map. Also similar to std::transform
template <typename... Fs>
struct transform_ {
  template <typename... Ts>
  struct f {
    typedef input_<eval_pipe_<input_<Ts>, pipe_<Fs...>>...> type;
  };
};

// FORK_ : Inputs are copied to each metafunctions
// The Peanut Butter of the library
template <typename... Cs>
struct fork_ {
  template <typename... Ts>
  struct f {
    typedef input_<eval_pipe_<input_<Ts...>, Cs>...> type;
  };
};

// EACH : Badly named mix between fork and transform. Requiere the same number
// of expressions as arguments than inputs.
template <typename... Es>
struct each_ {
  template <typename... Ts>
  struct f {
    typedef input_<eval_pipe_<input_<Ts>, Es>...> type;
  };
};

// DRAW_EVAL : Construct a function using the inputs, then evaluate using the
// inputs
template <typename... PipableFct>
struct draw_eval_ {
  template <typename... Ts>
  struct f {
    using metafct = eval_pipe_<input_<Ts...>, PipableFct..., quote_<pipe_>>;
    using type = typename metafct::template f<Ts...>::type;
  };
};

static_assert(
    eval_pipe_<input_<int, float, char>,
               draw_eval_<transform_<input_<identity>>, quote_<each_>>,
               same_as_<int, float, char>>::value,
    "");

// UNWRAP : Universal unwrapper.
struct unwrap {
  template <typename T>
  struct f {
    typedef typename specialization<unwrap, T>::type type;
  };
  template <template <typename T, T...> class F, typename U, U... values>
  struct f<F<U, values...>> : input_<std::integral_constant<U, values>...> {};

  template <template <typename... Ts> class F, typename... Ts>
  struct f<F<Ts...>> : input_<Ts...> {};
  template <template <typename... Ts> class F, typename... Ts>
  struct f<const F<Ts...>> : input_<Ts...> {};
  template <template <typename... Ts> class F, typename... Ts>
  struct f<F<Ts...> &> : input_<Ts...> {};
  template <template <typename... Ts> class F, typename... Ts>
  struct f<const F<Ts...> &> : input_<Ts...> {};
};
// specialization unwrap for std::array
// Caution about std::size_t to int conversion
template <typename T, std::size_t value>
struct specialization<unwrap, std::array<T, value>> : input_<T, i<value>> {};

// NOT_SAME_AS_
template <typename... Ts>
struct not_same_as_ {
  template <typename... Us>
  struct f {
    typedef std::integral_constant<
        bool, !std::is_same<input_<Ts...>, input_<Us...>>::value>
        type;
  };
};

// CONTAINER : Continue with a type that can contain a template template
// OR THE TYPE ITSELF IF NOT A TEMPLATE TEMPLATE;
struct container {
  struct sorry_not_implemented {};
  template <typename T>
  struct f : input_<T> {};
  template <template <typename...> class F, typename... Ts>
  struct f<F<Ts...>> : input_<quote_<F>> {};
};

template <template <typename...> class F>
struct container_is_ : pipe_<container, same_as_<quote_<F>>> {};

static_assert(
    eval_pipe_<input_<ls_<int>>, container, same_as_<quote_<ls_>>>::value, "");

// LISTIFY_ : wrap input_s into ls_
// Could have been implemented by struct listify : quote_<ls_>{}
struct listify {
  template <typename... Ts>
  struct f {
    typedef ls_<Ts...> type;
  };
};

// REVERSE : Reverse the order of the types
struct reverse {
  template <typename...>
  struct f_impl;
  template <template <typename...> class F, typename... Ts, typename T>
  struct f_impl<F<Ts...>, T> {
    typedef F<T, Ts...> type;
  };

  template <typename... Ts>
  struct f {
    typedef
        typename pipe_<input_<input_<>, Ts...>, fold_left_<lift_<f_impl>>>::type
            type;
  };
};

// CALL_F : The input_ is a metafunction. Call it.
struct call_f {
  template <typename... T>
  struct f : pipe_<T...> {};
};

// CONSTRUCT_FS : Construct a metafunction from the input_s. and call it.
template <typename... Fs>
struct construct_fs_ : pipe_<Fs..., call_f> {
  template <typename... Ts>
  struct f : pipe_<input_<Ts...>, Fs..., call_f> {};
};

template <typename... Predicate>
struct conditional {
 private:
  struct Type_not_a_predicate {};

 public:
  template <typename...>
  struct f : input_<error_<Type_not_a_predicate, Predicate...>> {};
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
// Implementation of mkseq -------------------------------------------
template <std::size_t... i>
struct index_sequence {
  typedef std::size_t value_type;

  typedef index_sequence<i...> type;
  typedef input_<std::integral_constant<int, i>...> to_types;
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
struct mkseq {
  struct not_integral_constant_int {};
  template <typename... Ts>
  struct f {
    typedef error_<not_integral_constant_int> type;
  };
  template <int I>
  struct f<i<I>> {
    typedef typename make_index_sequence<I>::to_types type;
  };
};

// ZIP : Join together two list of type in multiple inputs
struct zip {
  template <typename...>
  struct f {
    typedef error_<zip> type;
  };
  template <template <typename...> class F, template <typename...> class G,
            typename... Ts, typename... Us>
  struct f<F<Ts...>, G<Us...>> {
    typedef input_<input_<Ts, Us>...> type;
  };
};
static_assert(
    eval_pipe_<input_<input_<int, int>, input_<float, char>>, zip,
               same_as_<input_<int, float>, input_<int, char>>>::value,
    "");

// ZIP_INDEX
struct zip_index {
  template <typename... Ts>
  struct f_impl {
    typedef nothing type;
  };
  template <typename... Is, typename... Ts>
  struct f_impl<input_<Is...>, input_<Ts...>> {
    typedef input_<input_<Is, Ts>...> type;
  };

  template <typename... Ts>
  struct f {
    typedef typename f_impl<typename mkseq::template f<i<sizeof...(Ts)>>::type,
                            input_<Ts...>>::type type;
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
  struct f<input_<Is, Ts>...> {
    typedef input_<Ts...> type;
  };
};

// UNZIP
struct unzip {
  template <typename...>
  struct f;
  template <template <typename...> class F, typename... Fs, typename... Gs>
  struct f<F<Fs, Gs>...> : input_<input_<Fs...>, input_<Gs...>> {};
};

// PUSH_FRONT_ : Add anything you want to the front of the inputs.
template <typename... Ts>
struct push_front_ {
  template <typename... Inputs>
  struct f {
    typedef input_<Ts..., Inputs...> type;
  };
};

// PUSH_BACK_ : Add anything you want to the back of the inputs
template <typename... Ts>
struct push_back_ {
  template <typename... Inputs>
  struct f {
    typedef input_<Inputs..., Ts...> type;
  };
};

// GET : Continue with the type a index N
template <int I>
struct get_ {
  struct index_out_of_range {};
  template <bool b, typename... Ts>
  struct fff {
    typedef error_<index_out_of_range> type;
  };
  template <typename... Ts>
  struct fff<true, Ts...> {
    template <typename...>
    struct f_impl {};
    template <typename T>
    struct f_impl<i<I>, T> {
      typedef T type;
    };
    template <typename... Is, typename... Us>
    struct f_impl<input_<Is...>, input_<Us...>> : f_impl<Is, Us>... {};

    typedef typename mkseq::template f<i<sizeof...(Ts)>>::type indexed_input_s;
    typedef typename f_impl<indexed_input_s, input_<Ts...>>::type type;
  };

  template <typename... Ts>
  struct f {
    typedef
        typename fff<(I < sizeof...(Ts)) && (0 < sizeof...(Ts)), Ts...>::type
            type;
  };
};

// FIRST : Continue with the first type
struct first {
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
struct second {
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
struct third {
  template <typename... Ts>
  struct f {
    typedef nothing type;
  };
  template <typename T, typename T2, typename T3, typename... Ts>
  struct f<T, T2, T3, Ts...> {
    typedef T3 type;
  };
};

// Nth : Continue with the Nth type
typedef first _1st;
typedef second _2nd;
typedef third _3rd;

// LAST : Continue with the last type
struct last {
  template <typename... Ts>
  struct f {
    typedef eval_pipe_<input_<Ts...>, get_<sizeof...(Ts) - 1>> type;
  };
};

template <typename... Ts>
struct flat;
template <typename... Ls>
struct flat<input_<Ls...>> {
  typedef input_<Ls...> type;
};
template <typename... Ls, typename T, typename... Ts>
struct flat<input_<Ls...>, T, Ts...> : flat<input_<Ls..., T>, Ts...> {};
template <typename... Ls, typename T, typename... Is, typename... Ts>
struct flat<input_<Ls...>, T, input_<Is...>, Ts...>
    : flat<input_<Ls..., T, Is...>, Ts...> {};

template <typename... Ls, typename... Fs, typename... Ts>
struct flat<input_<Ls...>, input_<Fs...>, Ts...>
    : flat<input_<Ls..., Fs...>, Ts...> {};
template <typename... Fs, typename... Gs, typename... Ls, typename... Ts>
struct flat<input_<Ls...>, input_<Fs...>, input_<Gs...>, Ts...>
    : flat<input_<Ls..., Fs..., Gs...>, Ts...> {};
template <typename... Fs, typename... Gs, typename... Hs, typename... Ls,
          typename... Ts>
struct flat<input_<Ls...>, input_<Fs...>, input_<Gs...>, input_<Hs...>, Ts...>
    : flat<input_<Ls..., Fs..., Gs..., Hs...>, Ts...> {};
template <typename... Fs, typename... Gs, typename... Hs, typename... Is,
          typename... Ls, typename... Ts>
struct flat<input_<Ls...>, input_<Fs...>, input_<Gs...>, input_<Hs...>,
            input_<Is...>, Ts...>
    : flat<input_<Ls..., Fs..., Gs..., Hs..., Is...>, Ts...> {};

template <typename T>
struct flat_impl {
  typedef input_<T> type;
};
template <typename... Ts>
struct flat_impl<input_<Ts...>> {
  typedef input_<Ts...> type;
};

// FLATTEN : Continue with only one input_. Sub-input_ are removed.
// The dirty but necessary tool of our library
struct flatten {
  template <typename... Ts>
  struct f {
    typedef typename flat<input_<>, Ts...>::type type;
  };
};

// LENGTH : Continue with the number of types in the input_s.
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
struct not_ : cond_<pipe_<UnaryPredicate...>, input_<std::false_type>,
                    input_<std::true_type>> {};

template <>
struct not_<> {
  template <typename T>
  struct f : input_<std::integral_constant<decltype(!T::value), !T::value>> {};
};
// REMOVE_IF_ : Remove every type where the metafunction "returns"
// std::true_type
template <typename... UnaryPredicate>
struct remove_if_
    : pipe_<
          transform_<cond_<pipe_<UnaryPredicate...>, input_<>, quote_<input_>>>,
          flatten> {};

// PARTITION_ : Continue with two list. First predicate is true, Second
// predicate is false
template <typename... UnaryPredicate>
struct partition_ : fork_<pipe_<remove_if_<not_<UnaryPredicate...>>, listify>,
                          pipe_<remove_if_<UnaryPredicate...>, listify>> {};

// FILTER_ : Remove every type where the metafunction is false.
template <typename... UnaryPredicate>
struct filter_ : remove_if_<not_<UnaryPredicate...>> {};

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

static_assert(eval_pipe_<input_<float, int, float, int>,
                         find_if_<same_as_<int>>, same_as_<i<1>, int>>::value,
              "");

// CARTESIAN : Given two lists, continue with every possible lists of two types.
template <typename... BinF>  // Binary Function
struct cartesian_ {
  template <typename... Ts>
  struct f {
    typedef input_<Ts...> type;
  };
  template <typename A, typename B>
  struct f<A, B> {
    typedef typename pipe_<BinF...>::template f<A, B>::type type;
  };
  template <typename A, typename... B>
  struct f<A, input_<B...>> : input_<eval_pipe_<input_<A, B>, BinF...>...> {};
  template <typename... A, typename B>
  struct f<input_<A...>, B> : input_<eval_pipe_<input_<A, B>, BinF...>...> {};
  template <typename... A, typename... B>
  struct f<input_<A...>, input_<B...>> {
    typedef eval_pipe_<
        fork_<pipe_<input_<A, input_<B...>>, cartesian_<BinF...>>...>, flatten>
        type;
  };
};
static_assert(
    eval_pipe_<input_<int, float>, cartesian_<>, same_as_<int, float>>::value,
    "");
static_assert(eval_pipe_<input_<int, input_<float>>, cartesian_<>,
                         same_as_<int, float>>::value,
              "");
static_assert(eval_pipe_<input_<int, input_<float, int>>, cartesian_<listify>,
                         same_as_<ls_<int, float>, ls_<int, int>>>::value,
              "");
static_assert(
    eval_pipe_<
        input_<input_<int[1], int[2]>, input_<int[3], int[4]>>,
        cartesian_<listify>,
        same_as_<te::ls_<int[1], int[3]>, te::ls_<int[1], int[4]>,
                 te::ls_<int[2], int[3]>, te::ls_<int[2], int[4]>>>::value,
    "");
// ROTATE : rotate
// The implementation may rely on undefined behavior.
// But so far clang and gcc are compliant
template <int I>
struct rotate_ {
  template <int N, typename T, typename... Ts>
  struct f_impl : f_impl<N - 1, Ts..., T> {};
  template <typename T, typename... Ts>
  struct f_impl<0, T, Ts...> {
    typedef input_<T, Ts...> type;
  };

  template <typename... Ts>
  struct f : f_impl<I % sizeof...(Ts), Ts...> {};
};

static_assert(eval_pipe_<input_<int, float, short, int[2]>, rotate_<5>,
                         same_as_<float, short, int[2], int>>::value,
              "");

static_assert(eval_pipe_<input_<int, float, short, int[2]>, rotate_<-1>,
                         same_as_<int[2], int, float, short>>::value,
              "");

// IS_ZERO : return if the input_ is it's type_zero
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

static_assert(eval_pipe_<input_<b<0>>, is_zero>::value, "");
static_assert(eval_pipe_<input_<b<true>>, not_<is_zero>>::value, "");

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
        pipe_<NPred...>, input_<detail::__then_context<true, Ts...>>,
        input_<detail::__then_context<false, Ts...>>>::template f<Ts...>::type
        type;
  };
};

template <typename... NPred>
struct or_if_ {
  template <typename... Ts>
  struct f {
    typedef input_<Ts...> type;
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
    typedef input_<Ts...> type;
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
    typedef input_<Ts...> type;
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
    typedef input_<Ts...> type;
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
    typedef input_<Ts...> type;
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
    typedef input_<Ts...> type;
  };
  template <bool B, typename... Ts>
  struct f<detail::__then_context<B, Ts...>> {
    typedef input_<Ts...> type;
  };
};

template <typename UnaryPredicate, typename F>
struct if_then_ : cond_<UnaryPredicate, F, identity> {};

// ARITHMETIC METAFUNCTIONS

// std::integral_constant's boolean operator
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue < Uvalue)> operator<(
    std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue <= Uvalue)> operator<=(
    std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue > Uvalue)> operator>(
    std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue >= Uvalue)> operator>=(
    std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue == Uvalue)> operator==(
    std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);

template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
std::ratio_less<std::ratio<nA, dA>, std::ratio<nB, dB>> operator<(
    const std::ratio<nA, dA> &, const std::ratio<nB, dB> &);
template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
std::ratio_less_equal<std::ratio<nA, dA>, std::ratio<nB, dB>> operator<=(
    const std::ratio<nA, dA> &, const std::ratio<nB, dB> &);
template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
std::ratio_greater<std::ratio<nA, dA>, std::ratio<nB, dB>> operator>(
    const std::ratio<nA, dA> &, const std::ratio<nB, dB> &);
template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
std::ratio_greater_equal<std::ratio<nA, dA>, std::ratio<nB, dB>> operator>=(
    const std::ratio<nA, dA> &, const std::ratio<nB, dB> &);
template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
std::ratio_equal<std::ratio<nA, dA>, std::ratio<nB, dB>> operator==(
    const std::ratio<nA, dA> &, const std::ratio<nB, dB> &);

// LESS
template <typename... Ts>
struct less_ {
  typedef error_<less_<Ts...>> type;
};
template <typename P>
struct less_<P> {
  template <typename B>
  struct f {
    typedef decltype(std::declval<B>() < std::declval<P>()) type;
  };
};
template <>
struct less_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() < std::declval<B>()) type;
  };
};
// LESS_EQ
template <typename... Ts>
struct less_eq_ {
  typedef error_<less_<Ts...>> type;
};
template <typename P>
struct less_eq_<P> {
  template <typename B>
  struct f {
    typedef decltype(std::declval<B>() <= std::declval<P>()) type;
  };
};
template <>
struct less_eq_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() <= std::declval<B>()) type;
  };
};

// GREATER
template <typename... Ts>
struct greater_ {
  typedef error_<greater_<Ts...>> type;
};
template <typename P>
struct greater_<P> {
  template <typename B>
  struct f {
    typedef decltype(std::declval<B>() > std::declval<P>()) type;
  };
};
template <>
struct greater_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() > std::declval<B>()) type;
  };
};
// GREATER_EQ
template <typename... Ts>
struct greater_eq_ {
  typedef error_<greater_eq_<Ts...>> type;
};
template <typename P>
struct greater_eq_<P> {
  template <typename B>
  struct f {
    typedef decltype(std::declval<B>() >= std::declval<P>()) type;
  };
};
template <>
struct greater_eq_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() >= std::declval<B>()) type;
  };
};

// PLUS
template <typename... Ts>
struct plus_ {
  typedef error_<plus_<Ts...>> type;
};
template <typename P>
struct plus_<P> {
  template <typename... Ts>
  struct f;
  template <typename I>
  struct f<I> {
    typedef decltype(std::declval<I>() + std::declval<P>()) type;
  };
};
template <typename T, T t>
struct plus_<std::integral_constant<T, t>> {
  template <typename>
  struct f;
  template <typename U, U u>
  struct f<std::integral_constant<U, u>>
      : std::integral_constant<decltype(t + u), t + u> {};
};
template <intmax_t nA, intmax_t dA>
struct plus_<std::ratio<nA, dA>> {
  template <typename>
  struct f;
  template <intmax_t nB, intmax_t dB>
  struct f<std::ratio<nB, dB>> {
    typedef std::ratio_add<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

template <>
struct plus_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() + std::declval<B>()) type;
  };
  template <typename T, typename U, T t, U u>
  struct f<std::integral_constant<T, t>, std::integral_constant<U, u>> {
    typedef std::integral_constant<decltype(t + u), t + u> type;
  };
  template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
  struct f<std::ratio<nA, dA>, std::ratio<nB, dB>> {
    typedef std::ratio_add<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

// MINUS
template <typename... Ts>
struct minus_ {
  typedef error_<minus_<Ts...>> type;
};
template <typename P>
struct minus_<P> {
  template <typename... Ts>
  struct f;
  template <typename I>
  struct f<I> {
    typedef decltype(std::declval<I>() - std::declval<P>()) type;
  };
};
template <typename T, T t>
struct minus_<std::integral_constant<T, t>> {
  template <typename>
  struct f;
  template <typename U, U u>
  struct f<std::integral_constant<U, u>>
      : std::integral_constant<decltype(t - u), t - u> {};
};
template <intmax_t nA, intmax_t dA>
struct minus_<std::ratio<nA, dA>> {
  template <typename>
  struct f;
  template <intmax_t nB, intmax_t dB>
  struct f<std::ratio<nB, dB>> {
    typedef std::ratio_subtract<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

template <>
struct minus_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() - std::declval<B>()) type;
  };
  template <typename T, typename U, T t, U u>
  struct f<std::integral_constant<T, t>, std::integral_constant<U, u>> {
    typedef std::integral_constant<decltype(t - u), t - u> type;
  };
  template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
  struct f<std::ratio<nA, dA>, std::ratio<nB, dB>> {
    typedef std::ratio_subtract<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

// Multiply
template <typename... Ts>
struct multiply_ {
  typedef error_<multiply_<Ts...>> type;
};
template <typename P>
struct multiply_<P> {
  template <typename... Ts>
  struct f;
  template <typename I>
  struct f<I> {
    typedef decltype(std::declval<I>() * std::declval<P>()) type;
  };
};
template <typename T, T t>
struct multiply_<std::integral_constant<T, t>> {
  template <typename>
  struct f;
  template <typename U, U u>
  struct f<std::integral_constant<U, u>>
      : std::integral_constant<decltype(t * u), t * u> {};
};
template <intmax_t nA, intmax_t dA>
struct multiply_<std::ratio<nA, dA>> {
  template <typename>
  struct f;
  template <intmax_t nB, intmax_t dB>
  struct f<std::ratio<nB, dB>> {
    typedef std::ratio_multiply<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

template <>
struct multiply_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() * std::declval<B>()) type;
  };
  template <typename T, typename U, T t, U u>
  struct f<std::integral_constant<T, t>, std::integral_constant<U, u>> {
    typedef std::integral_constant<decltype(t * u), t * u> type;
  };
  template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
  struct f<std::ratio<nA, dA>, std::ratio<nB, dB>> {
    typedef std::ratio_multiply<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

// Multiply
template <typename... Ts>
struct divide_ {
  typedef error_<divide_<Ts...>> type;
};
template <typename P>
struct divide_<P> {
  template <typename... Ts>
  struct f;
  template <typename I>
  struct f<I> {
    typedef decltype(std::declval<I>() / std::declval<P>()) type;
  };
};
template <typename T, T t>
struct divide_<std::integral_constant<T, t>> {
  template <typename>
  struct f;
  template <typename U, U u>
  struct f<std::integral_constant<U, u>>
      : std::integral_constant<decltype(t / u), t / u> {};
};
template <intmax_t nA, intmax_t dA>
struct divide_<std::ratio<nA, dA>> {
  template <typename>
  struct f;
  template <intmax_t nB, intmax_t dB>
  struct f<std::ratio<nB, dB>> {
    typedef std::ratio_divide<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

template <>
struct divide_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() / std::declval<B>()) type;
  };
  template <typename T, typename U, T t, U u>
  struct f<std::integral_constant<T, t>, std::integral_constant<U, u>> {
    typedef std::integral_constant<decltype(t / u), t / u> type;
  };
  template <intmax_t nA, intmax_t dA, intmax_t nB, intmax_t dB>
  struct f<std::ratio<nA, dA>, std::ratio<nB, dB>> {
    typedef std::ratio_divide<std::ratio<nA, dA>, std::ratio<nB, dB>> type;
  };
};

// EQUAL
template <typename... Ts>
struct equal_ {
  typedef error_<equal_<Ts...>> type;
};
template <typename P>
struct equal_<P> {
  template <typename... Ts>
  struct f;
  template <typename I>
  struct f<I> {
    typedef decltype(std::declval<I>() == std::declval<P>()) type;
  };
};
template <>
struct equal_<> {
  template <typename P, typename B>
  struct f {
    typedef decltype(std::declval<P>() == std::declval<B>()) type;
  };
};

// PLUS
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

static_assert(eval_pipe_<input_<std::ratio<1, 4>>, plus_<std::ratio<2, 5>>,
                         equal_<std::ratio<26, 40>>>::value,
              "");

static_assert(
    eval_pipe_<input_<i<3>>, plus_<i<1>>, if_then_<same_as_<i<4>>, plus_<i<2>>>,
               same_as_<i<6>>>::value,
    "");
static_assert(
    eval_pipe_<input_<i<3>>, plus_<i<1>>, if_then_<same_as_<i<3>>, plus_<i<2>>>,
               same_as_<i<4>>>::value,
    "");

// SORT : Given a binary predicate, sort the types
// note : it's implicit that you receive two types, so you probably need to
// transform them. eg : sort by size : sort_<transform_<size>, greater_<> >

template <typename A, typename B>
struct eager {
  typedef input_<> type;
};

template <typename A>
struct eager<A, std::true_type> {
  typedef input_<A> type;
};

template <typename... BinaryPredicate>
struct sort_ {
  template <typename... L>
  struct sort_impl;
  template <typename T>
  struct sort_impl<T> {
    typedef T type;
  };

  template <typename... Ts, typename F>
  struct sort_impl<input_<F, Ts...>> {
    typedef typename sort_impl<typename flat<
        input_<>,
        typename eager<Ts, typename pipe_<BinaryPredicate...>::template f<
                               Ts, F>::type>::type...>::type>::type Yes_types;
    typedef typename sort_impl<typename flat<
        input_<>,
        typename eager<Ts, typename not_<pipe_<BinaryPredicate...>>::template f<
                               Ts, F>::type>::type...>::type>::type No_types;

    typedef typename flat<input_<>, Yes_types, F, No_types>::type type;
  };
  template <typename... Ts>
  struct f {
    typedef typename sort_impl<input_<Ts...>>::type type;
  };
};

// PUSH_OUT
// A very strange algorithm that is technically half a quicksort.
template <typename... BinaryPredicate>
struct push_out_ {
  template <typename... L>
  struct sort_impl;
  template <typename T>
  struct sort_impl<T> {
    typedef T type;
  };

  template <typename... Ts, typename F>
  struct sort_impl<input_<F, Ts...>> {
    typedef typename sort_impl<typename flat<
        input_<>,
        typename eager<Ts, typename not_<pipe_<BinaryPredicate...>>::template f<
                               Ts, F>::type>::type...>::type>::type No_types;

    typedef typename flat<input_<>, F, No_types>::type type;
  };
  template <typename... Ts>
  struct f {
    typedef typename sort_impl<input_<Ts...>>::type type;
  };
};

// RECURSIVE_PARTITION
// Recursively partition into two groups which result of those UnaryFunction is
// the same as the first. Basically it group all the types that have the same
// result into subrange.
template <typename... UnaryFunction>
struct recursive_partition_ {
  template <typename... Ts>
  struct f_impl;
  template <typename... Results>
  struct f_impl<input_<Results...>, input_<>> {
    typedef input_<Results...> type;
  };
  template <typename... Results, typename Head>
  struct f_impl<input_<Results...>, input_<Head>> {
    typedef input_<Results..., Head> type;
  };

  template <typename... Results, typename Head>
  struct f_impl<input_<Results...>, Head> {
    typedef input_<Results..., Head> type;
  };

  template <typename... Results, typename Head, typename... Tails>
  struct f_impl<input_<Results...>, input_<Head, Tails...>>
      : f_impl<
            input_<
                Results...,
                eval_pipe_<
                    input_<Head, Tails...>,
                    remove_if_<UnaryFunction...,
                               not_same_as_<typename pipe_<
                                   UnaryFunction...>::template f<Head>::type>>,
                    flatten>>,
            eval_pipe_<input_<Tails...>,
                       remove_if_<UnaryFunction...,
                                  same_as_<typename pipe_<UnaryFunction...>::
                                               template f<Head>::type>>,
                       flatten>> {};

  template <typename... Ts>
  struct f : f_impl<input_<>, input_<Ts...>> {};
};

static_assert(
    eval_pipe_<input_<char, int, float, int, float, char, char>,
               recursive_partition_<>, flatten,
               same_as_<char, char, char, int, int, float, float>>::value,
    "");
static_assert(eval_pipe_<input_<i<1>, i<2>, i<2>, i<4>>,
                         recursive_partition_<modulo_<i<3>>>, transform_<first>,
                         same_as_<i<1>, i<2>>>::value,
              "");

// UNIQUE : Keep only one of each different types
// The implementation is special but work.
// struct unique : push_out_<lift_<std::is_same>> {};
struct unique : pipe_<recursive_partition_<>, transform_<first>> {};

static_assert(eval_pipe_<input_<void, int, void, float, float, int>, unique,
                         same_as_<void, int, float>>::value,
              "");

// GROUP : Given a Unary Function, Gather those that give the same result
template <typename... UnaryFunction>
struct group_by_ : pipe_<recursive_partition_<UnaryFunction...>, flatten> {};

static_assert(
    eval_pipe_<
        input_<i<1>, i<2>, i<3>, i<4>>, group_by_<modulo_<i<2>>>,
        same_as_<std::integral_constant<int, 1>, std::integral_constant<int, 3>,
                 std::integral_constant<int, 2>,
                 std::integral_constant<int, 4>>>::value,
    "");

template <typename... UnaryFunction>
using group_range_ = recursive_partition_<UnaryFunction...>;

// COPY_ : Copy N times the inputs.
// Implemented as a higher meta-expression
template <unsigned int N>
struct copy_ : eval_pipe_<input_<i<N>>, mkseq, transform_<input_<identity>>,
                          quote_<fork_>> {};

// REPEAT_ : Repeat N times the meta-expression
template <std::size_t N, typename... Es>
struct repeat_ : eval_pipe_<input_<Es...>, copy_<N>, flatten, quote_<pipe_>> {};

static_assert(
    eval_pipe_<input_<int>,
               repeat_<2, lift_<std::add_pointer>, lift_<std::add_const>>,
               same_as_<int *const *const>>::value,
    "");

// SWIZZLE : Restructure the inputs using the index
template <std::size_t... Is>
struct swizzle_ : fork_<get_<Is>...> {};

static_assert(
    eval_pipe_<input_<int, int *, int **, int ***>, swizzle_<2, 1, 0, 3, 1>,
               same_as_<int **, int *, int, int ***, int *>>::value,
    "");

// ON_ARGS_ : unwrap rewrap in the same template template.
//
template <typename... Es>
struct on_args_ {
  template <typename... Ts>
  struct f : input_<error_<on_args_<Es...>, Ts...>> {};
  template <template <typename... Ts> class F, typename... Ts>
  struct f<F<Ts...>> {
    typedef eval_pipe_<input_<Ts...>, Es..., quote_<F>> type;
  };
  template <template <typename... Ts> class F, typename... Ts>
  struct f<F<Ts...> &> {
    typedef eval_pipe_<input_<Ts...>, Es..., quote_<F>,
                       lift_<std::add_lvalue_reference>>
        type;
  };
};
static_assert(eval_pipe_<input_<ls_<int, int[2], int[3]>>,
                         on_args_<sort_<transform_<size>, greater_<>>>,
                         same_as_<ls_<int[3], int[2], int>>>::value,
              "");

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
static_assert(
    eval_pipe_<input_<int, float, int, float, float>, recursive_partition_<>,
               transform_<arrayify>,
               same_as_<std::array<int, 2>, std::array<float, 3>>>::value,
    "");

// BIND
template <int I, typename... Es>
struct bind_ {
  template <typename... Ts>
  struct f
      : eval_pipe_<
            input_<i<sizeof...(Ts)>>, mkseq,
            transform_<cond_<
                same_as_<i<I >= 0 ? I % sizeof...(Ts)
                                  : (sizeof...(Ts) - (-I % sizeof...(Ts)))>>,
                input_<pipe_<Es...>>, input_<identity>>>,
            quote_<each_>>::template f<Ts...> {};
};

static_assert(
    eval_pipe_<input_<int, float, char>, bind_<0, lift_<std::add_pointer>>,
               same_as_<int *, float, char>>::value,
    "");
static_assert(
    eval_pipe_<input_<int, float, char>, bind_<-1, lift_<std::add_pointer>>,
               same_as_<int, float, char *>>::value,
    "");
static_assert(
    eval_pipe_<input_<int, float, char>, bind_<-2, lift_<std::add_pointer>>,
               same_as_<int, float *, char>>::value,
    "");

// BIND_ON_ARGS_
template <int I, typename... Es>
struct bind_on_args_ {
  template <typename... Ts>
  struct f
      : eval_pipe_<
            input_<i<sizeof...(Ts)>>, mkseq,
            transform_<cond_<
                same_as_<i<I >= 0 ? I % sizeof...(Ts)
                                  : (sizeof...(Ts) - (-I % sizeof...(Ts)))>>,
                input_<pipe_<input_<Ts...>, Es...>>, input_<identity>>>,
            quote_<each_>>::template f<Ts...> {};
};
static_assert(
    eval_pipe_<input_<i<1>, i<2>, i<3>>, bind_on_args_<-1, fold_left_<plus_<>>>,
               same_as_<i<1>, i<2>, i<6>>>::value,
    "Replace the last with the sum of all");
static_assert(
    eval_pipe_<input_<int, float, char>, bind_on_args_<-2, first, listify>,
               same_as_<int, ls_<int>, char>>::value,
    "");

};  // namespace te
#endif
