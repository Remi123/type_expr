#include <type_traits>

// This is for making std::integral_constant compatible with us.

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
template <typename T, T Tvalue, typename U, U Uvalue>
std::integral_constant<bool, (Tvalue < Uvalue)>
operator<(std::integral_constant<T, Tvalue>, std::integral_constant<U, Uvalue>);

namespace type_expr {

template <typename T> T &&declval();

// First Class Citizen of rage

// NOTHING : Universal representation of the concept of nothing
struct nothing {
  template <typename...> struct f { typedef nothing type; };
};

// INPUT : Universal type container of metafunctions.
template <typename... Ts> struct input {
  typedef input type;
  template <typename...> struct f { typedef input<Ts...> type; };
};
template <typename T> struct input<T> {
  typedef T type;
  template <typename... Ts> struct f { typedef T type; };
};

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

// ZERO : User-specialized to retrieve the zero of their type.
template <typename T> struct zero;

// I : Universal integer type.
template <int V> using i = std::integral_constant<int, V>;

// This would have been the ideal implementation of std::integral_constant
/*template <int I> struct i {*/
// typedef i type;
// constexpr static int value = I;
// template <int B> i<I + B> operator+(i<B> b);
// template <int B> i<I - B> operator-(i<B> b);
// template <int B> i<I * B> operator*(i<B> b);
// template <int B> i<I / B> operator/(i<B> b);
// template <int B> i<I % B> operator%(i<B> b);
/*};*/

// First specialization of zero. The zero of any i<Num> is i<0>
template <typename T, T value> struct zero<std::integral_constant<T, value>> {
  typedef std::integral_constant<T, 0> type;
};

// B : Universal boolean type.
template <bool B> using b = std::integral_constant<bool, B>;

// -------------------------------------------------------
// METAFUNCTION
// -------------------------------------------------------

template <typename...> struct Adding;
template <typename I, typename J> struct Adding<I, J> {
  typedef decltype(declval<I>() + declval<J>()) type;
};

template <typename...> struct Substracting;
template <typename I, typename J> struct Substracting<I, J> {
  typedef decltype(declval<I>() - declval<J>()) type;
};

template <typename...> struct Multipling;
template <typename I, typename J> struct Multipling<I, J> {
  typedef decltype(declval<I>() * declval<J>()) type;
};

template <typename...> struct Dividing;
template <typename I, typename J> struct Dividing<I, J> {
  typedef decltype(declval<I>() / declval<J>()) type;
};

template <typename...> struct Modulo;
template <typename I, typename J> struct Modulo<I, J> {
  typedef decltype(declval<I>() % declval<J>()) type;
};

template <typename...> struct add {
  template <typename... Ts> struct f;
  template <typename I, typename P> struct f<I, P> {
    typedef decltype(declval<I>() + declval<P>()) type;
  };
};

// ARITHMETIC METAFUNCTIONS
struct gcd {
  template <typename...> struct f;
  template <typename T> struct f<T, typename zero<T>::type> { typedef T type; };
  template <typename T, typename U>
  struct f<T, U> : f<U, typename Modulo<U, T>::type> {};
};

struct lcm {
  template <typename...> struct f;
  template <typename T, typename U> struct f<T, U> {
    typedef typename Dividing<typename Multipling<T, U>::type,
                              typename gcd::template f<T, U>::type>::type type;
  };
};

template <typename... Ts> struct less { typedef error_<less<Ts...>> type; };
template <typename P> struct less<P> {
  template <typename B> struct f {
    typedef decltype(declval<B>() < declval<P>()) type;
  };
};
template <> struct less<> {
  template <typename P, typename B> struct f {
    typedef decltype(declval<P>() < declval<B>()) type;
  };
};

template <typename P> struct plus {
  template <typename... Ts> struct f;
  template <typename I> struct f<I> {
    typedef decltype(declval<I>() + declval<P>()) type;
  };
};
template <typename P> struct minus {
  template <typename... Ts> struct f;
  template <typename I> struct f<I> {
    typedef decltype(declval<I>() - declval<P>()) type;
  };
};
template <typename P> struct multiply {
  template <typename... Ts> struct f;
  template <typename I> struct f<I> {
    typedef decltype(declval<I>() * declval<P>()) type;
  };
};
template <typename P> struct divide {
  template <typename... Ts> struct f;
  template <typename I> struct f<I> {
    typedef decltype(declval<I>() / declval<P>()) type;
  };
};
template <typename P> struct modulo {
  template <typename... Ts> struct f;
  template <typename I> struct f<I> {
    typedef decltype(declval<I>() % declval<P>()) type;
  };
};

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
  template <typename...> struct f { typedef input<> type; };
  template <typename A> struct f<A> { typedef A type; };

  template <typename... A> struct f<input<A...>> {
    typedef typename input<A...>::type type;
  };

  template <typename A, typename B, typename... Ts>
  struct f<A, B, Ts...> : f<typename F::template f<A, B>::type, Ts...> {};
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
  typedef typename pipe_<Cs...>::template f<>::type type;
};
template <> struct pipe_<> {
  template <typename...> struct f { typedef nothing type; };
  typedef nothing type;
};

template <typename... Fs> using pipe_t = typename pipe_<Fs...>::type;
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
    typedef typename input<Ts...>::template f<>::type type;
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
  template <typename T> struct f : pipe_<T> {};
};

// CONSTRUCT_FS : Construct a metafunction from the inputs. and call it.
template <typename... Fs> struct construct_fs {
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

// MKSEQ_ : Continue with i<0>, i<1>, ... , i<N-1>
// Need optimization but it's for later. I'll do it soon.
struct mkseq {
  template <typename... Is> struct f_impl;

  template <int In, typename... Is> struct f_impl<input<i<0>, Is...>, i<In>> {
    typedef input<i<0>, Is...> type;
  };

  template <int In, typename... Is>
  struct f_impl<input<Is...>, i<In>> : f_impl<input<i<In>, Is...>, i<In - 1>> {
  };

  template <bool B, typename I> struct fff;
  template <int I> struct fff<true, i<I>> {
    typedef typename f_impl<input<>, i<I - 1>>::type type;
  };
  template <int I> struct fff<false, i<I>> { typedef input<> type; };

  template <typename... Ts> struct f { typedef nothing type; };
  template <int I> struct f<i<I>> {
    typedef typename fff<(0 < I), i<I>>::type type;
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
static_assert(pipe_<input<int, float>, zip_index,
                    is_<ls_<i<0>, int>, ls_<i<1>, float>>>::type::value,
              "");
;
static_assert(
    pipe_<input<i<1>>, plus<i<1>>, mkseq, is_<i<0>, i<1>>>::type::value, "");

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
  template <typename... Ts> struct f_impl {};
  template <typename T> struct f_impl<i<I>, T> { typedef T type; };
  template <typename... Is, typename... Us>
  struct f_impl<input<Is...>, input<Us...>> : f_impl<Is, Us>... {};

  template <bool b, typename... Ts> struct fff { typedef nothing type; };
  template <typename... Ts> struct fff<true, Ts...> {
    typedef typename mkseq::template f<i<sizeof...(Ts)>>::type indexed_inputs;
    typedef input<typename f_impl<indexed_inputs, input<Ts...>>::type> type;
  };

  template <typename... Ts> struct f {
    typedef typename fff<(0 <= I && I < sizeof...(Ts)), Ts...>::type type;
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
template <typename... Ls, typename... Fs, typename... Ts>
struct flat<input<Ls...>, input<Fs...>, Ts...>
    : flat<input<Ls..., Fs...>, Ts...> {};
template <typename... Fs, typename... Gs, typename... Ls, typename... Ts>
struct flat<input<Ls...>, input<Fs...>, input<Gs...>, Ts...>
    : flat<input<Ls..., Fs..., Gs...>, Ts...> {};

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

static_assert(pipe_t<input<i<3>>, plus<i<1>>, if_then_<is_<i<4>>, plus<i<2>>>,
                     is_<i<6>>>::value,
              "");
static_assert(pipe_t<input<i<3>>, plus<i<1>>, if_then_<is_<i<3>>, plus<i<2>>>,
                     is_<i<4>>>::value,
              "");

// INSERT_AT
template <int Index, typename F>
struct insert_at_
    : pipe_<
          fork_<
              pipe_<zip_index, filter_<pipe_<unwrap, first, less<i<Index>>>>,
                    unzip_index>,
              F,
              pipe_<zip_index, remove_if_<pipe_<unwrap, first, less<i<Index>>>>,
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

// Below is not yet integrated into type_expr

//*template <typename A> struct less<void, A> : std::true_type {};*/
// template <typename A> struct less<A, void> : std::false_type {};
// template <> struct less<void, void> : std::true_type {};
// template <> struct less<int, float> : std::true_type {};
/*template <> struct less<float, int> : std::false_type {};*/

template <typename A, typename B>
struct greater : b<!less<>::template f<A, B>::type::value> {};

template <typename A, typename B> struct eager { typedef input<> type; };

template <typename A> struct eager<A, std::true_type> { typedef A type; };

struct sort {
  template <typename... L> struct sort_impl;
  template <typename T> struct sort_impl<T> { typedef T type; };

  template <typename... Ts, typename F> struct sort_impl<input<F, Ts...>> {

    typedef typename sort_impl<typename flat<
        input<>, typename eager<Ts, typename less<>::template f<Ts, F>::type>::
                     type...>::type>::type Less;
    typedef typename sort_impl<typename flat<
        input<>,
        typename eager<Ts, typename greater<Ts, F>::type>::type...>::type>::type
        More;
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
