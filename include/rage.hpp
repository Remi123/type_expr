namespace rage {

template<typename T>
T&& declval();

// Identity
struct identity;

template<int I>
struct i
{
        template<int B>
        i<I+B> operator+(i<B> b);
        template<int B>
        i<I-B> operator-(i<B> b);
        template<int B>
        i<I*B> operator*(i<B> b);
        template<int B>
        i<I/B> operator/(i<B> b);
};
template<typename ... > struct ls_{};

template<bool B>
struct b{typedef b type; static const bool value = B;};
typedef b<true> true_type;
typedef b<false> false_type;

template<typename ...>
struct Add;
template<typename I,typename J>
struct Add<I,J> {typedef decltype(declval<I>() + declval<J>()) type;};

template<typename ... Ts>
struct input{
    template<typename ...>
    struct f{
    typedef input<Ts...> type;
        };
};
template< typename  T>
struct input<T>
{
typedef T type;

    template<typename ... Ts>
    struct f{
    typedef T  type;
        };
};

template<typename ...>
struct add
{template<typename ... Ts>
    struct f; 
    template<typename I, typename P>
    struct f<I,P> {typedef decltype(declval<I>() + declval<P>() ) type;};
};

template<typename P>
struct plus
{template<typename ... Ts>
    struct f; 
    template<typename I>
    struct f<I> {typedef decltype(declval<I>() + declval<P>() ) type;};
};
template<typename P>
struct minus
{template<typename ... Ts>
    struct f; 
    template<typename I>
    struct f<I> {typedef decltype(declval<I>() - declval<P>() ) type;};
};
template<typename P>
struct multiply
{template<typename ... Ts>
    struct f; 
    template<typename I>
    struct f<I> {typedef decltype(declval<I>() * declval<P>() ) type;};
};
template<typename P>
struct divide
{template<typename ... Ts>
    struct f; 
    template<typename I>
    struct f<I> {typedef decltype(declval<I>() / declval<P>() ) type;};
};


// LIFT_
template<template<typename ... >class F> struct lift_{
    template<typename ... Ts>
    struct f {
        typedef typename F<Ts...>::type type;
    } ;
};
// QUOTE_
template<template<typename ... >class F>
 struct quote_{
    template<typename ... Ts>
    struct f {
        typedef F<Ts...> type;
    };
};

// FOLD_LEFT_
template<typename ...>
struct fold_left_;
template<typename F >
struct fold_left_<F>
{
        template<typename ...> struct f
        {};
        template<typename A> struct f<A> 
        {typedef A type;};

 template<typename A > struct f<input<A>> 
        {typedef A type;};
               
template<typename A, typename B, typename ... Ts > 
        struct f<A,B,Ts...> : f<typename F::template f<A,B>::type ,Ts... >
        {};
};



template<typename ...>
struct pipe_expr{};
template<typename T,typename G>
struct pipe_expr<T,G>
{
    typedef typename G::template f<T>::type type;
};
template<typename ... Ts,typename G>
struct pipe_expr<input<Ts...>,G>
{
    typedef typename G::template f<Ts...>::type type;
};

// PIPE_ 
// The Bread and Butter of the library
template<typename ... Cs>
struct pipe_; 
template<typename C,typename ... Cs>
struct pipe_<C,Cs...> 
{
    template<typename ... Ts> struct f {
    typedef typename fold_left_<lift_<pipe_expr>>::template f<
                            typename C::template f<Ts...>::type,
                            Cs...>::type type;
};
    typedef typename pipe_<C,Cs...>::template f<>::type type;

};


// FORK_
template<typename... Cs>
struct fork_
{
    template<typename ... Ts>
    struct f
    {
        typedef input<typename Cs::template f<Ts...>::type...> type;
    };
};

template<typename A, typename B>
struct is_same : false_type{};
template<typename A>
struct is_same<A,A> : true_type{};
template<typename A, typename B>
struct is_not_same : true_type{};
template<typename A>
struct is_not_same<A,A> : false_type{};

// IS_
template<typename ... Ts>
struct is_;
template<typename T,typename ... Ts>
struct is_<T,Ts...>
{
    template<typename ... Us>
    struct f
    {
        typedef typename is_same<ls_<T,Ts...>, ls_<Us...> >::type type; 
    };
};

// ISNT_
template<typename ... Ts>
struct isnt_;
template<typename T,typename ... Ts>
struct isnt_<T,Ts...>
{
    template<typename ... Us>
    struct f
    {
        typedef typename is_not_same<ls_<T,Ts...>, ls_<Us...> >::type type; 
    };
};

// TRANSFORM_
template<typename F>
struct transform_
{
    template<typename ... Ts>
    struct f
    {
        typedef input<typename F::template f<Ts>::type...> type;
    };
};

// LISTIFY_
struct listify
{
    template<typename ... Ts>
    struct f
    {   
        typedef input<ls_<Ts...>> type;
    };
};


// UNPACK
struct unpack
{
    template<typename ... Ts>
    struct f;
    template<template<typename ...> class F, typename ... Ts>
    struct f<F<Ts...>>
    {   
        typedef input<Ts... > type;
    };
};

// REVERSE
struct reverse
{
    template<typename ... > struct f_impl;
    template<template<typename ...> class F,typename... Ts,typename T> 
    struct f_impl<F<Ts...>,T>
    {
        typedef F<T,Ts...> type;
    };

    template<typename ... Ts>
    struct f
    {
        typedef typename pipe_<input<input<>,Ts...>, fold_left_<lift_<f_impl>> >::type type;
    };
};

// MKSEQ_
// Need optimization but it's for later. I'll do it soon.
template<typename I>
struct mkseq_;
template<int I>
struct mkseq_<i<I>>
{
    template< typename... Is>
    struct f_impl;

    template<int In, typename... Is>
    struct f_impl<input<i<0>,Is...>,i<In>  >
    { typedef input<i<0>,Is...> type; };

    template<int In, typename... Is>
    struct f_impl<input<Is...>, i<In>  > : f_impl<input<i<In>,Is...>, i<In-1> >
    { };

    template<typename ...> struct f
    {
        typedef typename f_impl<input<i<I-1>>,i<I-2>>::type type;
    };
};



// PUSH_FRONT_
template<typename ... Ts>
struct push_front_
{
    template<typename ... Inputs>
    struct f {typedef input<Ts...,Inputs...> type;};
};

// PUSH_BACK_
template<typename ... Ts>
struct push_back_
{
    template<typename ... Inputs>
    struct f {typedef input<Inputs...,Ts...> type;};
};


// FIRST
struct first
{
    template<typename ... Ts>
    struct f;
    template<typename T,typename ... Ts>
    struct f<T,Ts...>{typedef T type;};
};

// LAST
struct last
{
    template<typename ... Ts> struct detail_ls_{};
    struct detail_end;

    template<typename ... As>
    struct f_impl{} ; 

    template<typename T>
    struct f_impl<T,detail_end> {typedef T type;};    

    template<typename ... As, typename ... Bs>
    struct f_impl<detail_ls_<As...>, detail_ls_<Bs...>> : f_impl<As,Bs>...{};    

    template<typename ... Ts>
    struct f;
    template<typename T,typename ... Ts>
    struct f<T,Ts...> : f_impl<detail_ls_<T,Ts...>, detail_ls_<Ts...,detail_end>>
    {};
};

// GET
template<int I>
struct get_
{
    template<typename ... Ts>
    struct f_impl{};
    template<typename T>
    struct f_impl<true_type, T> 
    {typedef T type;};
    template<typename ...Is, typename ... Us>
    struct f_impl<input<Is...>, input<Us...>>
         : f_impl<typename is_same<Is,i<I>>::type , Us>...
        {};
    template<typename ... Ts>
    struct f
    {
        static_assert(I <= sizeof...(Ts),"ERROR Index is higher than the size of the type inputs lists");
    typedef typename mkseq_<i<sizeof...(Ts)>>::template f<>::type indexed_inputs;
    typedef input<typename f_impl<indexed_inputs,input<Ts...>>::type> type; 
    };
};


template<typename ... Ts>
struct flat;
template<typename... Ls>
struct flat<input<Ls...>>{ typedef input<Ls...> type;};
template<typename... Ls,typename T,  typename ...Ts>
struct flat<input<Ls...>, T, Ts...> : flat<input<Ls..., T>, Ts...> {};
template<template<typename ...> class F,typename... Ls, typename ... Fs, typename ... Ts>
struct flat<input<Ls...>, F<Fs...>, Ts...> : flat<input<Ls..., Fs...>, Ts...> {};
template<
template<typename ...> class F,typename... Fs,
template<typename ...> class G,typename... Gs,
 typename ... Ls, typename ... Ts>
struct flat<input<Ls...>, F<Fs...>,G<Gs...> ,Ts...> : flat<input<Ls..., Fs...,Gs...>, Ts...> {};

struct flatten
{
    template<typename ... Ts>
    struct f 
    {
        typedef typename flat<input<>,Ts...>::type type;
    };
};

struct identity
{
    template<typename ... Ts>
    struct f {typedef input<Ts...> type;};
    //template<typename T>
    //struct f<T> {typedef T type;};
};

// COND_
template<typename ... Ts>
struct conditional;
template<>
struct conditional<true_type>
{
   template<typename T, typename F>
    struct f {typedef T type;}; 
};
template<>
struct conditional<false_type>
{
   template<typename T, typename F>
    struct f {typedef F type;}; 
};

template<typename ... Ts>
struct cond_;
template<typename P,typename T, typename F>
struct cond_<P,T,F>
{
    template<typename ... Ts>
    struct f
    {
        typedef typename conditional<typename P::template f<Ts...>::type>::template 
                                    f< typename T::template f<Ts...>::type,
                                       typename F::template f<Ts...>::type
                                       >::type type;
    };
};

// NOT_
template<typename...>
struct not_;
template<typename P>
struct not_<P> : pipe_<cond_<P,input<false_type>, input<true_type>>>
{
};

// REMOVE_IF_
template<typename ... >
struct remove_if_;
template<typename P>
struct remove_if_<P> :pipe_<transform_<cond_<P,input<>, identity >> ,flatten, listify>
{
};

template<typename ...>
struct partition_;
template<typename P>
struct partition_<P> : pipe_<fork_<
   remove_if_<not_<P>>, remove_if_<P>    

>>
                                    {};

pipe_<input<int,float, short>,partition_<is_<int>> >::type t = 0;

//Below is not yet integrated into rage

template<typename A, typename B>
struct less : b<(sizeof(A) < sizeof(B))>
{
    
};

template<typename A>
struct less<void , A> : true_type
{};
template<typename A>
struct less< A,void> : false_type
{};
template<>
struct less< void,void> : true_type
{};
template<>
struct less<int, float> : true_type{}; 
template<>
struct less<float, int> : false_type{}; 

template<int A, int B>
struct less<i<A>,i<B>> : b<(A<B)>
{};

template<typename A, typename B>
struct greater : b<!less<A,B>::value>
{
};


template<typename A, typename B>
struct eager
{
    typedef input<> type;
};

template<typename A>
struct eager<A,true_type>
{
    typedef A type;
};



struct sort{
        template<typename ... L>
        struct sort_impl;
        template<typename T>
        struct sort_impl<T>
        {typedef T type;};

        template<typename ... Ts,typename F>
        struct sort_impl<input<F,Ts...>> {
           
            typedef typename  sort_impl<
                typename flat<input<>,
                                typename eager<Ts,typename less<Ts,F>::type
                                >::type...
                            >::type>::type Less;
            typedef  typename  sort_impl<
                typename flat<input<>,
                                typename eager<Ts,typename greater<Ts,F>::type
                                >::type...
                            >::type>::type More;
            typedef typename flat<input<>,Less,F,More>::type type;
        };
    template<typename ... Ts>
    struct f { typedef typename sort_impl<input<Ts...>>::type type;};
};


/*template<typename ... Ts>*/
//struct unique
//{
    //typedef typename sort<Ts...>::type sorted;
    //typedef typename unique<sorted>::type type;
//};

//template<typename  T>
//struct unique<T>
//{typedef ls_<T> type;
    
//};

//template<typename ... Ts,typename ... Us>
//struct unique<ls_<Ts...>,ls_<Us...>>
//{
    //typedef typename flat<typename eager<Ts,typename is_not_same<Ts,Us>::type>::type ... >::type type;
//};

//template<typename F,typename ... Ts>
//struct unique<ls_<F,Ts...>>
//{
    //typedef typename unique<ls_<F,Ts...>,ls_<Ts...,ls_<>>>::type type;
/*};*/
//int t2 = sort<float,int,int,void,void,float,short,double>::type{};
// int t1 = unique<i<3>>::type {};
// int t = unique<i<3>,i<1>,i<3>,i<4>,i<3>,i<1>,i<2>,i<3>,i<5>>::type {};


}; //namespace rage
