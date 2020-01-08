
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


