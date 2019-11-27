template<int I>
struct i;

template<bool B>
struct b{typedef b type; static const bool value = B;};


template<typename A, typename B>
struct less : b<(sizeof(A) < sizeof(B))>
{
    static_assert(sizeof(A) != sizeof(B), "");
};
template<typename A>
struct less<A , A> : b<true>
{};
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
struct is_same : b<false>{};
template<typename A>
struct is_same<A,A> : b<true>{};
template<typename A, typename B>
struct is_not_same : b<true>{};
template<typename A>
struct is_not_same<A,A> : b<false>{};


template<typename ... > struct ls_;
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
{typedef ls_<T> type;};

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
int t2 = sort<float,int,int,void,void,float,short,double>::type{};
// int t1 = unique<i<3>>::type {};
// int t = unique<i<3>,i<1>,i<3>,i<4>,i<3>,i<1>,i<2>,i<3>,i<5>>::type {};

