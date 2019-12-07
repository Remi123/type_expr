namespace rage {

template <typename T> T &&declval();

// First Class Citizen of rage

// INPUT : Universal type container of metafunctions.
template <typename... Ts> struct input {
  template <typename...> struct f { typedef input<Ts...> type; };
};
template <typename T> struct input<T> {
  typedef T type;
  template <typename... Ts> struct f { typedef T type; };
};

// LS_ : user-declared container
template <typename...> struct ls_ {};

// IDENTITY : Continue with whatever the inputs were.
struct identity {
  template <typename... Ts> struct f { typedef input<Ts...> type; };
  template <typename T> struct f<T> { typedef T type; };
};

// NOTHING : Universal representation of the concept of nothing
struct nothing;

// ZERO : User-specialized to retrive the zero of their type.
template<typename T> struct zero;

// I : Universal integer type.
template <int I> struct i {
  typedef i type;
  template <int B> i<I + B> operator+(i<B> b);
  template <int B> i<I - B> operator-(i<B> b);
  template <int B> i<I * B> operator*(i<B> b);
  template <int B> i<I / B> operator/(i<B> b);
  template <int B> i<I % B> operator%(i<B> b);
};
// First specialization of zero. The zero of any i<Num> is i<0>
template<int N>
struct zero<i<N>>
{
  typedef i<0> type;
};

// B : Universal boolean type.
template <bool B> struct b {
  typedef b type;
  static const bool value = B;
};
typedef b<true> true_type;
typedef b<false> false_type;


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
template <template <typename...> class F> struct lift_ {
  template <typename... Ts> struct f { typedef typename F<Ts...>::type type; };
};
// QUOTE_ : Universal wrapper.
template <template <typename...> class F> struct quote_ {
  template <typename... Ts> struct f { typedef F<Ts...> type; };
};

// FOLD_LEFT_ : Fold expression 
// The Farmer of the library
template <typename...> struct fold_left_;
template <typename F> struct fold_left_<F> {
  template <typename...> struct f {};
  template <typename A> struct f<A> { typedef A type; };

  template <typename A> struct f<input<A>> { typedef A type; };

  template <typename A, typename B, typename... Ts>
  struct f<A, B, Ts...> : f<typename F::template f<A, B>::type, Ts...> {};
};

// PIPE_EXPR : Internal-only. Take a type and send it as input to the next metafunction
// The Flour and Yeast of the library. 
template <typename...> struct pipe_expr {};
template <typename T, typename G> struct pipe_expr<T, G> {
  typedef typename G::template f<T>::type type;
};
template <typename... Ts, typename G> struct pipe_expr<input<Ts...>, G> {
  typedef typename G::template f<Ts...>::type type;
};

// PIPE_ : Universal container of metafunction. 
// The Bread and Butter of the library
template <typename... Cs> struct pipe_;
template <typename C, typename... Cs> struct pipe_<C, Cs...> {
  template <typename... Ts> struct f {
    typedef typename fold_left_<lift_<pipe_expr>>::template f<
        typename C::template f<Ts...>::type, Cs...>::type type;
  };
  typedef typename pipe_<C, Cs...>::template f<>::type type;
};

// FORK_ : Inputs are copied to each metafunctions 
// The Peanut Butter of the library
template <typename... Cs> struct fork_ {
  template <typename... Ts> struct f {
    typedef input<typename Cs::template f<Ts...>::type...> type;
  };
};

template <typename A, typename B> struct is_same : false_type {};
template <typename A> struct is_same<A, A> : true_type {};
template <typename A, typename B> struct is_not_same : true_type {};
template <typename A> struct is_not_same<A, A> : false_type {};

// IS_ : Comparaison metafunction.
template <typename... Ts> struct is_;
template <typename T, typename... Ts> struct is_<T, Ts...> {
  template <typename... Us> struct f {
    typedef typename is_same<ls_<T, Ts...>, ls_<Us...>>::type type;
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

// UNWRAP : Unwrap types inside a wrapper into input<Ts...>
struct unwrap {
  template <typename... Ts> struct f;
  template <template <typename...> class F, typename... Ts> struct f<F<Ts...>> {
    typedef input<Ts...> type;
  };
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

// MKSEQ_ : Continue with i<0>, i<1>, ... , i<N-1>
// Need optimization but it's for later. I'll do it soon.
template <typename Accessor>
struct mkseq_ : construct_fs<Accessor, quote_<mkseq_>> {};
template <int I> struct mkseq_<i<I>> {
  template <typename... Is> struct f_impl;

  template <int In, typename... Is> struct f_impl<input<i<0>, Is...>, i<In>> {
    typedef input<i<0>, Is...> type;
  };

  template <int In, typename... Is>
  struct f_impl<input<Is...>, i<In>> : f_impl<input<i<In>, Is...>, i<In - 1>> {
  };

  template <typename...> struct f {
    typedef typename f_impl<input<i<I - 1>>, i<I - 2>>::type type;
  };
};

static_assert(
    pipe_<input<i<1>>, mkseq_<plus<i<1>>>, is_<i<0>, i<1>>>::type::value, "");

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

// FIRST : Continue with the first type
struct first {
  template <typename... Ts> struct f;
  template <typename T, typename... Ts> struct f<T, Ts...> { typedef T type; };
};

// LAST : Continue with the last type
struct last {
  template <typename... Ts> struct detail_ls_ {};
  struct detail_end;

  template <typename... As> struct f_impl {};

  template <typename T> struct f_impl<T, detail_end> { typedef T type; };

  template <typename... As, typename... Bs>
  struct f_impl<detail_ls_<As...>, detail_ls_<Bs...>> : f_impl<As, Bs>... {};

  template <typename... Ts> struct f;
  template <typename T, typename... Ts>
  struct f<T, Ts...>
      : f_impl<detail_ls_<T, Ts...>, detail_ls_<Ts..., detail_end>> {};
};

// GET : Continue with the type a index N
template <int I> struct get_ {
  template <typename... Ts> struct f_impl {};
  template <typename T> struct f_impl<true_type, T> { typedef T type; };
  template <typename... Is, typename... Us>
  struct f_impl<input<Is...>, input<Us...>>
      : f_impl<typename is_same<Is, i<I>>::type, Us>... {};
  template <typename... Ts> struct f {
    static_assert( I >= 0 &&
        I < sizeof...(Ts),
        "ERROR Index is higher than the size of the type inputs lists");
    typedef
        typename mkseq_<i<sizeof...(Ts)>>::template f<>::type indexed_inputs;
    typedef input<typename f_impl<indexed_inputs, input<Ts...>>::type> type;
  };
};

template <typename... Ts> struct flat;
template <typename... Ls> struct flat<input<Ls...>> {
  typedef input<Ls...> type;
};
template <typename... Ls, typename T, typename... Ts>
struct flat<input<Ls...>, T, Ts...> : flat<input<Ls..., T>, Ts...> {};
template <template <typename...> class F, typename... Ls, typename... Fs,
          typename... Ts>
struct flat<input<Ls...>, F<Fs...>, Ts...> : flat<input<Ls..., Fs...>, Ts...> {
};
template <template <typename...> class F, typename... Fs,
          template <typename...> class G, typename... Gs, typename... Ls,
          typename... Ts>
struct flat<input<Ls...>, F<Fs...>, G<Gs...>, Ts...>
    : flat<input<Ls..., Fs..., Gs...>, Ts...> {};

// FLATTEN : Continue with only one input. Sub-input are removed.
struct flatten {
  template <typename... Ts> struct f {
    typedef typename flat<input<>, Ts...>::type type;
  };
};

// LENGTH : Continue with the number of types in the inputs.
struct length {
  template <typename... Ts> struct f : input<i<sizeof...(Ts)>> {};
};

// SIZE : Continue with the sizeof(T). T is one input
struct size {
  template <typename... Ts> struct f;
  template <typename T> struct f<T> : input<i<sizeof(T)>> {};
};

// ALIGNMENT : Continue with the alignment of one input.
struct alignment {
  template <typename... Ts> struct f;
  template <typename T> struct f<T> : input<i<alignof(T)>> {};
};



template <typename... Ts> struct conditional;
template <> struct conditional<true_type> {
  template <typename T, typename F> struct f { typedef T type; };
};
template <> struct conditional<false_type> {
  template <typename T, typename F> struct f { typedef F type; };
};

// COND_ : Similar to std::conditional but only accept metafunctions
template <typename... Ts> struct cond_;
template <typename P, typename T, typename F> struct cond_<P, T, F> {
  template <typename... Ts> struct f {
    typedef
        typename conditional<typename P::template f<Ts...>::type>::template f<
            typename T::template f<Ts...>::type,
            typename F::template f<Ts...>::type>::type type;
  };
};

// NOT_ : Boolean metafunction are inversed
template <typename...> struct not_;
template <typename P>
struct not_<P> : pipe_<cond_<P, input<false_type>, input<true_type>>> {};

// REMOVE_IF_ : Remove every type where the metafunction "returns" true_type
template <typename...> struct remove_if_;
template <typename P>
struct remove_if_<P> : pipe_<transform_<cond_<P, input<>, identity>>, flatten> {
};

// PARTITION_ : Continue with two list. First predicate is true, Second predicate is false
template <typename...> struct partition_;
template <typename P>
struct partition_<P> : fork_<pipe_<remove_if_<not_<P>>, listify>,
                             pipe_<remove_if_<P>, listify>> {};

// FILTER_ : Remove every type where the metafunction is false.
template<typename ... > struct filter_;
template<typename P>
struct filter_<P> : pipe_<remove_if_<not_<P>>> {};

// REPLACE_IF_ : Replace the type by another if the predicate is true
template <typename... Ts> struct replace_if_;
template <typename P, typename T> struct replace_if_<P, T> {
  template <typename... Ts> struct f {
    typedef input<typename conditional<
        typename P::template f<Ts>::type>::template f<T, Ts>::type...>
        type;
  };
};

namespace detail {
template<typename ... As>
  struct f_impl_fold_until;
  template<typename F,typename T, typename N, typename ... As>
  struct f_impl_fold_until<F,T,T,N,As...>
  { typedef T type;};
  template<typename F,typename T>
  struct f_impl_fold_until<F,T>
  { typedef rage::nothing type;};
  template<typename F,typename T>
  struct f_impl_fold_until<F,T,T>
  { typedef T type;};
 template<typename F,typename T, typename A,typename B,typename ... Zs>
  struct f_impl_fold_until<F, T, A ,B , Zs...> : 
    f_impl_fold_until< F,T,typename F::template f<B>::type , Zs...>
  {};
};

// FOLD_UNTIL_ : Fold until a type is meet
template<typename F, typename Type> struct fold_until_
{
  template<typename ... Ts>
  struct f;
  template<typename T,typename ... Ts>
  struct f<T,Ts...> : 
    detail::f_impl_fold_until<F,Type,typename F::template f<T>::type,Ts...>{};
};

// OR_ : Fold expression until true_type is meet
template<typename F>
struct or_ : fold_until_<F,true_type>
{
};

// AND_ : Fold expression until false_type is meet
template<typename F>
struct and_ : 
  fold_until_<F,false_type>
{
};

// COUNT_IF_ : Count the number of type where the predicate is true
template<typename F>
struct count_if_  
{
  template<typename ... Ts>
  struct f 
  {
    typedef typename pipe_<input<Ts...>, remove_if_<not_<F>>, length>::type type;
  };
};




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



// Below is not yet integrated into rage

template <typename A, typename B> struct less : b<(sizeof(A) < sizeof(B))> {};

template <typename A> struct less<void, A> : true_type {};
template <typename A> struct less<A, void> : false_type {};
template <> struct less<void, void> : true_type {};
template <> struct less<int, float> : true_type {};
template <> struct less<float, int> : false_type {};

template <int A, int B> struct less<i<A>, i<B>> : b<(A < B)> {};

template <typename A, typename B> struct greater : b<!less<A, B>::value> {};

template <typename A, typename B> struct eager { typedef input<> type; };

template <typename A> struct eager<A, true_type> { typedef A type; };

struct sort {
  template <typename... L> struct sort_impl;
  template <typename T> struct sort_impl<T> { typedef T type; };

  template <typename... Ts, typename F> struct sort_impl<input<F, Ts...>> {

    typedef typename sort_impl<typename flat<
        input<>,
        typename eager<Ts, typename less<Ts, F>::type>::type...>::type>::type
        Less;
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

}; // namespace rage
