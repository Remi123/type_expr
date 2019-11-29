﻿namespace rage {

template<typename T>
T&& declval();

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
  //template<typename ... As > struct f<input<As...>> 
        //{typedef ls_<As...> type;};
               
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
struct is_same : b<false>{};
template<typename A>
struct is_same<A,A> : b<true>{};
template<typename A, typename B>
struct is_not_same : b<true>{};
template<typename A>
struct is_not_same<A,A> : b<false>{};


template<typename ... Ts>
struct is_
{
    template<typename ... Us>
    struct f
    {
        typedef typename is_same<ls_<Ts...>, ls_<Us...> >::type type; 
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




template<typename A, typename B>
struct less : b<(sizeof(A) < sizeof(B))>
{
    
};

template<typename A>
struct less<void , A> : b<true>
{};
template<typename A>
struct less< A,void> : b<false>
{};
template<>
struct less< void,void> : b<true>
{};
template<>
struct less<int, float> : b<true>{}; 
template<>
struct less<float, int> : b<false>{}; 

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
    typedef ls_<> type;
};

template<typename A>
struct eager<A,b<true>>
{
    typedef A type;
};


template<typename ...L>
struct join : join <ls_<>,L...>{};
template<typename L>
struct join<L> { typedef L type;}; 
template<typename ...L, typename ...R>
struct join<ls_<L...>,ls_<>,R...> : join<ls_<L...>,R...> 
{};
template<typename ...L, typename ...R>
struct join<ls_<L...>,ls_<>,ls_<>,R...> : join<ls_<L...>,R...> 
{};
template<typename ...L, typename ...R>
struct join<ls_<L...>,ls_<>,ls_<>,ls_<>,R...> : join<ls_<L...>,R...> 
{};
template<typename ...L, typename T, typename ...R>
struct join<ls_<L...>,T,R...> : join<ls_<L...,T>,R...> 
{};
template<typename ...L, typename ... T, typename ...R>
struct join<ls_<L...>,ls_<T...>,R...> : join<ls_<L...,T...>,R...> 
{};
template<typename ...Ls>
struct sort;
template<typename T>
struct sort<T> {typedef ls_<T> type;};
template<>
struct sort<ls_<>> {typedef ls_<> type;};
template<typename ... L>
struct sort_impl;
template<typename T>
struct sort_impl<T>
{typedef T type;};

template<typename ... Ts,typename F>
struct sort_impl<F,Ts...> {
   
    typedef typename  sort<
        typename join<ls_<>,
                        typename eager<Ts,typename less<Ts,F>::type
                        >::type...
                    >::type>::type Less;
    typedef  typename  sort<
        typename join<ls_<>,
                        typename eager<Ts,typename greater<Ts,F>::type
                        >::type...
                    >::type>::type More;
    typedef typename join<Less,F,More>::type type;
};

template< typename ...Ls>
struct sort
{
    typedef typename sort_impl<Ls...>::type type;
};
template<typename ...Ls>
struct sort<ls_<Ls...>>
{
    typedef typename sort_impl<Ls...>::type type;
};

template<typename ... Ts>
struct unique
{
    typedef typename sort<Ts...>::type sorted;
    typedef typename unique<sorted>::type type;
};

template<typename  T>
struct unique<T>
{typedef ls_<T> type;
    
};

template<typename ... Ts,typename ... Us>
struct unique<ls_<Ts...>,ls_<Us...>>
{
    typedef typename join<typename eager<Ts,typename is_not_same<Ts,Us>::type>::type ... >::type type;
};

template<typename F,typename ... Ts>
struct unique<ls_<F,Ts...>>
{
    typedef typename unique<ls_<F,Ts...>,ls_<Ts...,ls_<>>>::type type;
};
//int t2 = sort<float,int,int,void,void,float,short,double>::type{};
// int t1 = unique<i<3>>::type {};
// int t = unique<i<3>,i<1>,i<3>,i<4>,i<3>,i<1>,i<2>,i<3>,i<5>>::type {};


}; //namespace rage
