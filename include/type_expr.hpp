#include <type_traits>
#include <utility>

namespace type_expr {

// -------------------------------------------------------
// FIRST CLASS CITIZEN OF TYPE_EXPR
// -------------------------------------------------------


// INPUT : Universal type container of metafunctions.
template <typename... Ts> struct input {
  typedef input type;
  template <typename...> struct f { typedef input<Ts...> type; };
};
template <typename T> struct input<T> {
  typedef T type;
  template <typename... Ts> struct f { typedef T type; };
};

// NOTHING : Universal representation of the concept of nothing
using nothing = input<>;

// ERROR : Template used for debugging
template <typename MSG> struct error_ {
  template <typename...> struct f { typedef error_<MSG> type; };
};

// LS_ : user-declared container
template <typename...> struct ls_ {};

// IDENTITY : Continue with whatever the inputs were.
struct identity {
  template <typename... Ts> struct f { typedef input<Ts...> type; };
  template <typename T> struct f<T> { typedef T type; };
};

// I : Universal integer type.
template <int V> using i = std::integral_constant<int, V>;

// ZERO : User-specialized to retrieve the zero of their type.
template <typename T> struct zero;

// First specialization of zero. The zero of any i<Num> is i<0>
template <typename T, T value> struct zero<std::integral_constant<T, value>> {
  typedef std::integral_constant<T, 0> type;
};

// B : Universal boolean type.
template <bool B> using b = std::integral_constant<bool, B>;

// FRACTION : Universal fraction type.
template<typename Num, typename Den>
struct fraction_;
/*{*/
  //typedef typename decltype(std::declval<Num>() / std::declval<Den>() )  reduce;
/*};*/

// -------------------------------------------------------
// METAFUNCTION
// -------------------------------------------------------

// LIFT_ : Universal customization point using template template. Get the ::type
// The Farming field of our library
template <template <typename...> class F> struct lift_ {
  template <typename... Ts> struct f { typedef typename F<Ts...>::type type; };
};
// QUOTE_ : Universal wrapper. Doesn't get the ::type. Use with template alias
// The Other Farming field of our library
template <template <typename...> class F> struct quote_ {
  template <typename... Ts> struct f { typedef F<Ts...> type; };
};

// FOLD_LEFT_ : Fold expression
// The Farmer of the library
template <typename F> struct fold_left_ {

  template <typename...> struct f { typedef error_<fold_left_<int>> type; };
  template <typename A> struct f<A> { typedef A type; };

  template <typename... A> struct f<input<A...>> {
    typedef typename input<A...>::type type;
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

// PIPE_EXPR : Internal-only. Take a type and send it as input to the next
// metafunction.
// The Flour and Yeast of the library.
template <typename...> struct pipe_expr { typedef nothing type; };
template <typename T, typename G> struct pipe_expr<T, G> {
  typedef typename G::template f<T>::type type;
};
template <typename... Ts, typename G> struct pipe_expr<input<Ts...>, G> {
  typedef typename G::template f<Ts...>::type type;
};
template <typename ErrorT, typename G> struct pipe_expr<error_<ErrorT>, G> {
  typedef error_<ErrorT> type;
};

// PIPE_ : Universal container of metafunction.
// The Bread and Butter of the library
template <typename... Cs> struct pipe_ {
  template <typename... Ts> struct f {
    typedef typename fold_left_<lift_<pipe_expr>>::template f<input<Ts...>,
                                                              Cs...>::type type;
  };
  using type = typename pipe_<Cs...>::template f<>::type ;
  // TODO : This ::type is a problem since it's always instanciated
};
template <> struct pipe_<> {
  template <typename...> struct f { typedef nothing type; };
  typedef nothing type;
};

template <typename... Fs> using pipe_t = typename pipe_<Fs...>::template f<>::type;
// template <typename... Fs> constexpr int pipe_v = pipe_<Fs...>::type::value;

// FORK_ : Inputs are copied to each metafunctions
// The Peanut Butter of the library
template <typename... Cs> struct fork_ {
  template <typename... Ts> struct f {
    typedef input<typename Cs::template f<Ts...>::type...> type;
  };
};

// UNWRAP : Universal unwrapper.
struct unwrap {
  struct unwrappable_type {};
  template <typename... Ts> struct f {
    typedef input<error_<unwrappable_type>> type;
  };
  template <template <typename... Ts> class F, typename... Ts>
  struct f<F<Ts...>> {
    typedef typename input<Ts...>::type type;
  };
};

template <typename A, typename B> struct is_same : std::false_type {};
template <typename A> struct is_same<A, A> : std::true_type {};
template <typename A, typename B> struct is_not_same : std::true_type {};
template <typename A> struct is_not_same<A, A> : std::false_type {};

// IS_ : Comparaison metafunction.
template <typename... Ts> struct is_ {
  template <typename... Us> struct f {
    typedef typename is_same<ls_<Ts...>, ls_<Us...>>::type type;
  };
};

// ISNT_
template <typename... Ts> struct isnt_;
template <typename T, typename... Ts> struct isnt_<T, Ts...> {
  template <typename... Us> struct f {
    typedef typename is_not_same<ls_<T, Ts...>, ls_<Us...>>::type type;
  };
};

// TRANSFORM_ : Similar to haskell's map. Also similar to std::transform
template <typename F> struct transform_ {
  template <typename... Ts> struct f {
    typedef input<typename F::template f<Ts>::type...> type;
  };
};

// LISTIFY_ : wrap inputs into ls_
struct listify {
  template <typename... Ts> struct f { typedef ls_<Ts...> type; };
};

// REVERSE : Reverse the order of the types
struct reverse {
  template <typename...> struct f_impl;
  template <template <typename...> class F, typename... Ts, typename T>
  struct f_impl<F<Ts...>, T> {
    typedef F<T, Ts...> type;
  };

  template <typename... Ts> struct f {
    typedef
        typename pipe_<input<input<>, Ts...>, fold_left_<lift_<f_impl>>>::type
            type;
  };
};

// CALL_F : The input is a metafunction. Call it.
struct call_f {
  template <typename ... T> struct f : pipe_<T...> {};
};

// CONSTRUCT_FS : Construct a metafunction from the inputs. and call it.
template <typename... Fs> struct construct_fs : pipe_<Fs..., call_f> {
  template <typename... Ts> struct f : pipe_<input<Ts...>, Fs..., call_f> {};
};

template <typename... Ts> struct conditional;
template <> struct conditional<std::true_type> {
  template <typename T, typename F> struct f { typedef T type; };
};
template <> struct conditional<std::false_type> {
  template <typename T, typename F> struct f { typedef F type; };
};

// COND_ : Similar to std::conditional but only accept metafunctions
template <typename P, typename T, typename F> struct cond_ {
  template <typename... Ts> struct f {
    typedef
        typename conditional<typename P::template f<Ts...>::type>::template f<
            T, F>::type::template f<Ts...>::type type;
  };
};


// Implementation of mkseq
template< std::size_t ... i >
struct index_sequence
{
        typedef std::size_t value_type;

        typedef index_sequence<i...> type;
        typedef input<std::integral_constant<int,i>...> to_types;
};


// this structure doubles index_sequence elements.
// s- is number of template arguments in IS.
template< std::size_t s, typename IS >
struct doubled_index_sequence;

template< std::size_t s, std::size_t ... i >
struct doubled_index_sequence< s, index_sequence<i... > >
{
        typedef index_sequence<i..., (s + i)... > type;
};

// this structure incremented by one index_sequence, iff NEED-is true, 
// otherwise returns IS
template< bool NEED, typename IS >
struct inc_index_sequence;

template< typename IS >
struct inc_index_sequence<false,IS>{ typedef IS type; };

template< std::size_t ... i >
struct inc_index_sequence< true, index_sequence<i...> >
{
        typedef index_sequence<i..., sizeof...(i)> type;
};



// helper structure for make_index_sequence.
template< std::size_t N >
struct make_index_sequence_impl : 
        inc_index_sequence< (N % 2 != 0), 
        typename doubled_index_sequence< N / 2,
        typename make_index_sequence_impl< N / 2> ::type
        >::type
        >
{};

// helper structure needs specialization only with 0 element.
template<>struct make_index_sequence_impl<0>{ typedef index_sequence<> type; };



// OUR make_index_sequence,  gcc-4.4.7 doesn't support `using`, 
// so we use struct instead of it.
template< std::size_t N >
struct make_index_sequence : make_index_sequence_impl<N>::type {};

//index_sequence_for  any variadic templates
template< typename ... T >
struct index_sequence_for : make_index_sequence< sizeof...(T) >{};





// MKSEQ_ : Continue with i<0>, i<1>, ... , i<N-1>
struct mkseq {
  template <typename... Ts> struct f { typedef nothing type; };
  template <int I> struct f<i<I>> {
    typedef typename make_index_sequence<I>::to_types type;
  };
};

// ZIP : Join together two list of type in multiple ls_
struct zip {

  template <typename...> struct f { typedef error_<zip> type; };
  template <template <typename...> class F, typename... Ts, typename... Us>
  struct f<F<Ts...>, F<Us...>> {
    typedef input<ls_<Ts, Us>...> type;
  };
};
static_assert(pipe_<input<ls_<int, int>, ls_<float, char>>, zip,
                    is_<ls_<int, float>, ls_<int, char>>>::type::value,
              "");

// ZIP_INDEX
struct zip_index {
  template <typename... Ts> struct f_impl { typedef nothing type; };
  template <typename... Is, typename... Ts>
  struct f_impl<input<Is...>, input<Ts...>> {
    typedef input<ls_<Is, Ts>...> type;
  };

  template <typename... Ts> struct f {
    typedef typename f_impl<typename mkseq::template f<i<sizeof...(Ts)>>::type,
                            input<Ts...>>::type type;
  };
};

// UNZIP_INDEX
struct unzip_index {
  struct unzip_index_not_recognized {};
  template <typename... Ts> struct f {
    typedef error_<unzip_index_not_recognized> type;
  };
  template <typename... Is, typename... Ts> struct f<ls_<Is, Ts>...> {
    typedef input<Ts...> type;
  };
};

// PUSH_FRONT_ : Add anything you want to the front of the inputs.
template <typename... Ts> struct push_front_ {
  template <typename... Inputs> struct f {
    typedef input<Ts..., Inputs...> type;
  };
};

// PUSH_BACK_ : Add anything you want to the back of the inputs
template <typename... Ts> struct push_back_ {
  template <typename... Inputs> struct f {
    typedef input<Inputs..., Ts...> type;
  };
};

// GET : Continue with the type a index N
template <int I> struct get_ {

  template <bool b, typename... Ts> struct fff { typedef nothing type; };
  template <typename... Ts> struct fff<true, Ts...> {
        template <typename... > struct f_impl {};
          template <typename T> struct f_impl<i<I>, T> { typedef T type; };
          template <typename... Is, typename... Us>
          struct f_impl<input<Is...>, input<Us...>> : f_impl<Is, Us>... {};

    typedef typename mkseq::template f<i<sizeof...(Ts)>>::type indexed_inputs;
    typedef input<typename f_impl<indexed_inputs, input<Ts...>>::type> type;
  };

  template <typename... Ts> struct f {
    typedef typename fff<(I < sizeof...(Ts)) &&( 0 < sizeof...(Ts)), Ts...>::type type;
  };
};

// FIRST : Continue with the first type
struct first {
  template <typename... Ts> struct f { typedef nothing type; };
  template <typename T, typename... Ts> struct f<T, Ts...> { typedef T type; };
};

// SECOND : Continue with the first type
struct second {
  template <typename... Ts> struct f { typedef nothing type; };
  template <typename T, typename T2, typename... Ts> struct f<T, T2, Ts...> {
    typedef T2 type;
  };
};

// THIRD : Continue with the third type
struct third {
  template <typename... Ts> struct f { typedef nothing type; };
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
  template <typename... Ts> struct detail_ls_ {};
  struct detail_end;

  template <typename... As> struct f_impl {};

  template <typename T> struct f_impl<T, detail_end> { typedef T type; };

  template <typename... As, typename... Bs>
  struct f_impl<detail_ls_<As...>, detail_ls_<Bs...>> : f_impl<As, Bs>... {};

  template <typename... Ts> struct f { typedef nothing type; };
  template <typename T, typename... Ts>
  struct f<T, Ts...>
      : f_impl<detail_ls_<T, Ts...>, detail_ls_<Ts..., detail_end>> {};
};

template <typename... Ts> struct flat;
template <typename... Ls> struct flat<input<Ls...>> {
  typedef input<Ls...> type;
};
template <typename... Ls, typename T, typename... Ts>
struct flat<input<Ls...>, T, Ts...> : flat<input<Ls..., T>, Ts...> {};
template <typename... Ls, typename T, typename... Is, typename... Ts>
struct flat<input<Ls...>, T, input<Is...>, Ts...>
    : flat<input<Ls..., T, Is...>, Ts...> {};

template <typename... Ls, typename... Fs, typename... Ts>
struct flat<input<Ls...>, input<Fs...>, Ts...>
    : flat<input<Ls..., Fs...>, Ts...> {};
template <typename... Fs, typename... Gs, typename... Ls, typename... Ts>
struct flat<input<Ls...>, input<Fs...>, input<Gs...>, Ts...>
    : flat<input<Ls..., Fs..., Gs...>, Ts...> {};
template <typename... Fs, typename... Gs, typename... Hs, typename... Ls,
          typename... Ts>
struct flat<input<Ls...>, input<Fs...>, input<Gs...>, input<Hs...>, Ts...>
    : flat<input<Ls..., Fs..., Gs..., Hs...>, Ts...> {};
template <typename... Fs, typename... Gs, typename... Hs, typename... Is,
          typename... Ls, typename... Ts>
struct flat<input<Ls...>, input<Fs...>, input<Gs...>, input<Hs...>,
            input<Is...>, Ts...>
    : flat<input<Ls..., Fs..., Gs..., Hs..., Is...>, Ts...> {};

template <typename T> struct flat_impl { typedef input<T> type; };
template <typename... Ts> struct flat_impl<input<Ts...>> {
  typedef input<Ts...> type;
};

// FLATTEN : Continue with only one input. Sub-input are removed.
// The dirty but necessary tool of our library
struct flatten {
  template <typename... Ts> struct f {
    typedef typename flat<input<>, Ts...>::type type;
  };
};

// LENGTH : Continue with the number of types in the inputs.
struct length {
  template <typename... Ts> struct f { typedef i<sizeof...(Ts)> type; };
};

// SIZE : Continue with the sizeof(T). T is one input
struct size {
  template <typename T> struct f { typedef i<sizeof(T)> type; };
};

// ALIGNMENT : Continue with the alignment of one input.
struct alignment {
  template <typename T> struct f { typedef i<alignof(T)> type; };
};

// NOT_ : Boolean metafunction are inversed
template <typename P>
struct not_ : cond_<P, input<std::false_type>, input<std::true_type>> {};

// REMOVE_IF_ : Remove every type where the metafunction "returns"
// std::true_type
template <typename P>
struct remove_if_ : pipe_<transform_<cond_<P, input<>, identity>>, flatten> {};

// PARTITION_ : Continue with two list. First predicate is true, Second
// predicate is false
template <typename...> struct partition_;
template <typename P>
struct partition_<P> : fork_<pipe_<remove_if_<not_<P>>, listify>,
                             pipe_<remove_if_<P>, listify>> {};

// FILTER_ : Remove every type where the metafunction is false.
template <typename P> struct filter_ : remove_if_<not_<P>> {};

// REPLACE_IF_ : Replace the type by another if the predicate is true
template <typename... Ts> struct replace_if_;
template <typename P, typename F> struct replace_if_<P, F> {
  template <typename... Ts> struct f {
    typedef input<typename conditional<typename P::template f<Ts>::type>::
                      template f<typename F::template f<Ts>::type, Ts>::type...>
        type;
  };
};

namespace detail {
template <typename... As> struct f_impl_fold_until;
template <typename F, typename T, typename N, typename... As>
struct f_impl_fold_until<F, T, T, N, As...> {
  typedef T type;
};
template <typename F, typename T> struct f_impl_fold_until<F, T, T> {
  typedef T type;
};
template <typename F, typename T> struct f_impl_fold_until<F, T> {
  typedef nothing type;
};
template <typename F, typename T, typename A, typename B, typename... Zs>
struct f_impl_fold_until<F, T, A, B, Zs...>
    : f_impl_fold_until<F, T, typename F::template f<B>::type, Zs...> {};
}; // namespace detail

// FOLD_UNTIL_ : Fold until a type is meet
template <typename F, typename Type> struct fold_until_ {
  template <typename... Ts> struct f;
  template <typename T, typename... Ts>
  struct f<T, Ts...>
      : detail::f_impl_fold_until<F, Type, typename F::template f<T>::type,
                                  Ts...> {};
};

// ALL_OF_ : Thanks to Roland Bock for the inspiration
template <typename P> struct all_of_ {
  template <typename... Ts> struct f {
    typedef typename is_same<
        ls_<b<true>, typename P::template f<Ts>::type...>,
        ls_<typename P::template f<Ts>::type..., b<true>>>::type type;
  };
};

// ANY_OF_ : Thanks to Roland Bock for the inspiration
template <typename P> struct any_of_ {
  template <typename... Ts> struct f {
    typedef typename is_not_same<
        ls_<b<false>, typename P::template f<Ts>::type...>,
        ls_<typename P::template f<Ts>::type..., b<false>>>::type type;
  };
};

// NONE_OF : Simply the inverse of any_of_
template <typename P> struct none_of_ : not_<any_of_<P>> {};

// COUNT_IF_ : Count the number of type where the predicate is true
template <typename F> struct count_if_ : pipe_<remove_if_<not_<F>>, length> {};

// FIND_IF_ : Return the first index that respond to the predicate, along with
// the type.
template <typename F>
struct find_if_
    : pipe_<zip_index, filter_<pipe_<unwrap, second, F>>, first, unwrap> {};

static_assert(pipe_<input<float, int, float, int>, find_if_<is_<int>>,
                    is_<i<1>, int>>::type::value,
              "");

// PRODUCT : Given two lists, continue with every possible lists of two types.
struct product {
  template <typename... Ts> struct f;
  template <typename A, typename... Ts> struct f<ls_<A, ls_<Ts...>>> {
    typedef input<ls_<A, Ts>...> type;
  };
  template <typename... As, typename... Bs>
  struct f<ls_<As...>, ls_<Bs...>>
      : pipe_<input<ls_<As, ls_<Bs...>>...>, transform_<product>, flatten> {};
};

// ROTATE : rotate
// The implementation may rely on undefined behavior.
// But so far clang and gcc are compliant
template <int I> struct rotate_ {
  template <int N, typename T, typename... Ts>
  struct f_impl : f_impl<N - 1, Ts..., T> {};
  template <typename T, typename... Ts> struct f_impl<0, T, Ts...> {
    typedef input<T, Ts...> type;
  };

  template <typename... Ts> struct f : f_impl<I % sizeof...(Ts), Ts...> {};
};

static_assert(pipe_t<input<int, float, short, int[2]>, rotate_<5>,
                     is_<float, short, int[2], int>>::value,
              "");

static_assert(pipe_t<input<int, float, short, int[2]>, rotate_<-1>,
                     is_<int[2], int, float, short>>::value,
              "");

// IS_ZERO : return if the input is it's type_zero
struct is_zero {
  template <typename... ts> struct f_impl { typedef b<false> type; };
  template <typename T> struct f_impl<T, T> { typedef b<true> type; };

  template <typename... T> struct f { typedef b<false> type; };
  template <typename T> struct f<T> {
    typedef typename f_impl<T, typename zero<T>::type>::type type;
  };
};

static_assert(pipe_<input<b<0>>, is_zero>::type::value, "");
static_assert(pipe_<input<b<true>>, not_<is_zero>>::type::value, "");

// IF, AND , OR : WIP
// Not satisfying will revisit later
/*template<typename ... Ps>*/
// struct if_ : pipe_<Ps..., cond_<is_zero,input<b<false>>,input< b<true>>> >
//{};
// template<typename P>
// struct and_: pipe_<P,cond_<is_zero,input<b<false>>,identity>>
//{};
// template<typename P>
// struct or_: pipe_<P,cond_<not_<is_zero>,input<b<true>>, identity>>
//{};

// static_assert(pipe_<input<b<false>>, if_< is_zero , and_<is_<b<true>>>
// >>::type::value,"");

template <typename P, typename F> struct if_then_ : cond_<P, F, identity> {};

// ARITHMETIC METAFUNCTIONS

// This is for making std::integral_constant compatible with this library.

// Integral operator
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<decltype(Tvalue + Uvalue), Tvalue + Uvalue>
operator+(std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<decltype(Tvalue - Uvalue), Tvalue - Uvalue>
operator-(std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<decltype(Tvalue * Uvalue), Tvalue * Uvalue>
operator*(std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<decltype(Tvalue / Uvalue), Tvalue / Uvalue>
operator/(std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<decltype(Tvalue % Uvalue), Tvalue % Uvalue>
operator%(std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);

// Boolean operator
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue < Uvalue)>
operator<(std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue <= Uvalue)>
operator<=(std::integral_constant<T, Tvalue>,
           std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue > Uvalue)>
operator>(std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue >= Uvalue)>
operator>=(std::integral_constant<T, Tvalue>,
           std::integral_constant<U, Uvalue>);

// LESS
template <typename... Ts> struct less_ { typedef error_<less_<Ts...>> type; };
template <typename P> struct less_<P> {
  template <typename B> struct f {
    typedef decltype(std::declval<B>() < std::declval<P>()) type;
  };
};
template <> struct less_<> {
  template <typename P, typename B> struct f {
    typedef decltype(std::declval<P>() < std::declval<B>()) type;
  };
};
// LESS_EQ
template <typename... Ts> struct less_eq_ {
  typedef error_<less_<Ts...>> type;
};
template <typename P> struct less_eq_<P> {
  template <typename B> struct f {
    typedef decltype(std::declval<B>() <= std::declval<P>()) type;
  };
};
template <> struct less_eq_<> {
  template <typename P, typename B> struct f {
    typedef decltype(std::declval<P>() <= std::declval<B>()) type;
  };
};

// GREATER
template <typename... Ts> struct greater_ {
  typedef error_<greater_<Ts...>> type;
};
template <typename P> struct greater_<P> {
  template <typename B> struct f {
    typedef decltype(std::declval<B>() > std::declval<P>()) type;
  };
};
template <> struct greater_<> {
  template <typename P, typename B> struct f {
    typedef decltype(std::declval<P>() > std::declval<B>()) type;
  };
};
// GREATER_EQ
template <typename... Ts> struct greater_eq_ {
  typedef error_<greater_eq_<Ts...>> type;
};
template <typename P> struct greater_eq_<P> {
  template <typename B> struct f {
    typedef decltype(std::declval<B>() >= std::declval<P>()) type;
  };
};
template <> struct greater_eq_<> {
  template <typename P, typename B> struct f {
    typedef decltype(std::declval<P>() >= std::declval<B>()) type;
  };
};

// PLUS
template <typename... Ts> struct plus_ { typedef error_<plus_<Ts...>> type; };
template <typename P> struct plus_<P> {
  template <typename... Ts> struct f;
  template <typename I> struct f<I> {
    typedef decltype(std::declval<I>() + std::declval<P>()) type;
  };
};
template <> struct plus_<> {
  template <typename P, typename B> struct f {
    typedef decltype(std::declval<P>() + std::declval<B>()) type;
  };
};
// MINUS
template <typename... Ts> struct minus_ { typedef error_<minus_<Ts...>> type; };
template <typename P> struct minus_<P> {
  template <typename... Ts> struct f;
  template <typename I> struct f<I> {
    typedef decltype(std::declval<I>() - std::declval<P>()) type;
  };
};
template <> struct minus_<> {
  template <typename P, typename B> struct f {
    typedef decltype(std::declval<P>() - std::declval<B>()) type;
  };
};

// MULTIPLY
template <typename... Ts> struct multiply_ {
  typedef error_<multiply_<Ts...>> type;
};
template <typename P> struct multiply_<P> {
  template <typename... Ts> struct f;
  template <typename I> struct f<I> {
    typedef decltype(std::declval<I>() * std::declval<P>()) type;
  };
};
template <> struct multiply_<> {
  template <typename P, typename B> struct f {
    typedef decltype(std::declval<P>() * std::declval<B>()) type;
  };
};
// DIVIDE
template <typename... Ts> struct divide_ {
  typedef error_<divide_<Ts...>> type;
};
template <typename P> struct divide_<P> {
  template <typename... Ts> struct f;
  template <typename I> struct f<I> {
    typedef decltype(std::declval<I>() / std::declval<P>()) type;
  };
};
template <> struct divide_<> {
  template <typename P, typename B> struct f {
    typedef decltype(std::declval<P>() / std::declval<B>()) type;
  };
};
// MODULO
template <typename... Ts> struct modulo_ {
  typedef error_<modulo_<Ts...>> type;
};
template <typename P> struct modulo_<P> {
  template <typename... Ts> struct f{typedef error_<modulo_<P>> type;};
  template <typename I> struct f<I> {
    typedef decltype(std::declval<I>() % std::declval<P>()) type;
  };
};
template <> struct modulo_<> {
  template <typename P, typename B> struct f {
    typedef decltype(std::declval<P>() % std::declval<B>()) type;
  };
};

struct gcd {
  template <typename...> struct f{ typedef error_<gcd> type;};
  template <typename T> struct f<T, typename zero<T>::type> { typedef T type; };
  template <typename T, typename U>
  struct f<T, U> : f<U, typename modulo_<>::template f<T,U>::type> {};
  template <typename T>
  struct f<T, T> { typedef T type; };
};


struct lcm {
  template <typename...> struct f;
  template <typename T, typename U> struct f<T, U> {
    typedef typename divide_<>::template f<
        typename multiply_<>::template f<T, U>::type,
        typename gcd::template f<T, U>::type>::type type;
  };
};

static_assert(pipe_t<input<i<3>>, plus_<i<1>>, if_then_<is_<i<4>>, plus_<i<2>>>,
                     is_<i<6>>>::value,
              "");
static_assert(pipe_t<input<i<3>>, plus_<i<1>>, if_then_<is_<i<3>>, plus_<i<2>>>,
                     is_<i<4>>>::value,
              "");

// INSERT_AT
template <int Index, typename F>
struct insert_at_
    : pipe_<
          fork_<pipe_<zip_index, filter_<pipe_<unwrap, first, less_<i<Index>>>>,
                      unzip_index>,
                F,
                pipe_<zip_index,
                      remove_if_<pipe_<unwrap, first, less_<i<Index>>>>,
                      unzip_index>>,
          flatten> {};

static_assert(
    pipe_<input<int, float, int[2], float[2]>,
          insert_at_<2, input<char, char[2]>>,
          is_<int, float, char, char[2], int[2], float[2]>>::type::value,
    "");

static_assert(pipe_<input<int, float>, insert_at_<1, identity>,
                    is_<int, int, float, float>>::type::value,
              "");




// FRACTION : Arithmetic type. 
template<typename N, typename D>
struct fraction
{
        /*typedef typename gcd::template f<N,D>::type Gcd;*/
        //typedef fraction<typename divide_<>::template f<N,Gcd>::type,
                /*typename divide_<>::template f<D,Gcd>::type > Reduce;*/

        template<typename N2, typename D2>
                fraction<
                decltype(std::declval<N>() * std::declval<D2>() + std::declval<N2>() * std::declval<D>()),
                decltype(std::declval<D>() * std::declval<D2>())
                        >
                        operator+(const fraction<N2,D2>&){};

        template<typename N2, typename D2>
                fraction<
                decltype(std::declval<N>() * std::declval<D2>() - std::declval<N2>() * std::declval<D>()),
                decltype(std::declval<D>() * std::declval<D2>())
                        >
                        operator-(const fraction<N2,D2>&);

        template<typename N2, typename D2>
                fraction<
                decltype(std::declval<N>() * std::declval<N2>()),
                decltype(std::declval<D>() * std::declval<D2>())
                        >
                        operator*(const fraction<N2,D2>&);
        template<typename N2, typename D2>
                fraction<
                decltype(std::declval<N>() / std::declval<N2>()),
                decltype(std::declval<D>() * std::declval<D2>())
                        >
                        operator/(const fraction<N2,D2>&);



};

template<typename N, typename D,typename N2>
        fraction<
decltype(std::declval<N>() + (std::declval<N2>() * std::declval<D>())  ),
        decltype(std::declval<D>())
        >
        operator+(const N2&,const fraction<N,D>){};
template<typename N, typename D,typename N2>
        fraction<
decltype(std::declval<N>() + (std::declval<N2>() * std::declval<D>())  ),
        decltype(std::declval<D>())
        >
        operator+(const fraction<N,D>,const N2&){};
template<typename N, typename D,typename N2>
        fraction<
decltype(std::declval<N2>() * std::declval<D>() - std::declval<N>() ),
        decltype(std::declval<D>())
        >
        operator-(const N2&,const fraction<N,D>){};
template<typename N, typename D,typename N2>
        fraction<
decltype(std::declval<N>() - (std::declval<N2>() * std::declval<D>())  ),
        decltype(std::declval<D>())
        >
        operator-(const fraction<N,D>,const N2&){};


template<typename N, typename D,typename N2>
        fraction<
decltype(std::declval<N>() * std::declval<N2>()  ),
        decltype(std::declval<D>())
        >
        operator*(const N2&,const fraction<N,D>){};
template<typename N, typename D,typename N2>
        fraction<
decltype(std::declval<N>() * std::declval<N2>()  ),
        decltype(std::declval<D>())
        >
        operator*(const fraction<N,D>,const N2&){};
//WIP
template<typename N, typename D,typename N2>
        fraction<
decltype(std::declval<N2>() / std::declval<N>()  ),
        decltype(std::declval<D>())
        >
        operator/(const N2&,const fraction<N,D>){};
template<typename N, typename D,typename N2>
        fraction<
decltype(std::declval<N>() * std::declval<N2>()  ),
        decltype(std::declval<D>())
        >
        operator/(const fraction<N,D>,const N2&){};




/*template<typename N>*/
//struct fraction<N,N> : std::integral_constant<int,1> 
//{
  //typedef N Gcd;
  //typedef std::integral_constant<int,1> Reduce;
/*};*/

template<typename N>
struct fraction<N,typename zero<N>::type>
{
//empty
};

template<typename N, typename D>
struct zero<fraction<N,D>>
{
  typedef fraction<typename zero<N>::type,D> type;
};

template<int I, int J>
using frac = fraction<std::integral_constant<int,I>, std::integral_constant<int,J>>;
static_assert(pipe_t<input<frac<9,5>>, plus_<frac<3,4>> , is_<frac<51,20>>>::value,"");
static_assert(pipe_<input<frac<5,5>>, plus_<frac<3,1>>, is_<frac<20,5>> >::type::value,"");
static_assert(pipe_<input<frac<2,5>>, plus_<frac<3,5>>, is_<frac<25,25>> >::type::value,"");

// Below is not yet integrated into type_expr


template <typename A, typename B> struct eager { typedef input<> type; };

template <typename A> struct eager<A, std::true_type> { typedef A type; };

template<typename Predicate>
struct sort {
  template <typename... L> struct sort_impl;
  template <typename T> struct sort_impl<T> { typedef T type; };

  template <typename... Ts, typename F> struct sort_impl<input<F, Ts...>> {

    typedef typename sort_impl<typename flat<
        input<>, typename eager<Ts, typename Predicate::template f<Ts, F>::type>::
                     type...>::type>::type Less;
    typedef typename sort_impl<typename flat<
        input<>, typename eager<Ts, typename not_<Predicate>::template f<Ts, F>::
                                        type>::type...>::type>::type More;

    typedef typename flat<input<>, Less, F, More>::type type;
  };
  template <typename... Ts> struct f {
    typedef typename sort_impl<input<Ts...>>::type type;
  };
};

/*template<typename ... Ts>*/
// struct unique
//{
// typedef typename sort<Ts...>::type sorted;
// typedef typename unique<sorted>::type type;
//};

// template<typename  T>
// struct unique<T>
//{typedef ls_<T> type;

//};

// template<typename ... Ts,typename ... Us>
// struct unique<ls_<Ts...>,ls_<Us...>>
//{
// typedef typename flat<typename eager<Ts,typename
// is_not_same<Ts,Us>::type>::type ... >::type type;
//};

// template<typename F,typename ... Ts>
// struct unique<ls_<F,Ts...>>
//{
// typedef typename unique<ls_<F,Ts...>,ls_<Ts...,ls_<>>>::type type;
/*};*/
// int t2 = sort<float,int,int,void,void,float,short,double>::type{};
// int t1 = unique<i<3>>::type {};
// int t = unique<i<3>,i<1>,i<3>,i<4>,i<3>,i<1>,i<2>,i<3>,i<5>>::type {};

}; // namespace type_expr
