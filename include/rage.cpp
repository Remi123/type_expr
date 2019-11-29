#include "rage.hpp"
#include <assert.h>
#include <type_traits>
int main()
{
        //ls_<float,int,void,short,double> t2 = sort<float,int,int,void,void,float,short,double>::type{};
        using namespace rage;

        static_assert(std::is_same<i<(4+3*2/1)> , i<10>>::value, "");

        auto input_test = pipe_<input< i<1> > 
                                , input <i<2> > >::type{};
        static_assert(std::is_same<decltype(input_test) , i<2>>::value, "");

        auto sort_test = sort<i<3>,i<1>,i<3>,i<4>,i<3>,i<1>,i<2>,i<3>,i<5>>::type {};
        sort_test = ls_<i<1>, i<1>, i<2>, i<3>, i<3>, i<3>, i<3>, i<4>, i<5> >{};

        auto pipe_test = pipe_<input<i<3>>,plus<i<1>>, plus<i<2>>>::type {};
        static_assert( std::is_same<decltype(pipe_test), i<6>>::value , "");

        auto fold_test = pipe_< input<i<1>,i<2>,i<3> >
                                , fold_left_<add<>>
                                >::type{} ;
        static_assert( std::is_same<decltype(fold_test), i<6>>::value , "");

        static_assert (std::is_same<decltype(pipe_test) ,decltype( fold_test) >::value, "");

        auto pipe_fold_test =  pipe_<input<i<1>,i<4> >,fold_left_<lift_<Add>>>::type{};
        static_assert( std::is_same<decltype(pipe_fold_test), i<5>>::value , "");

        auto pipe_pipe_test = pipe_<pipe_< input<i<3>> , plus<i<1>> >,
                                    plus<i<2>> >::type{};
        static_assert( std::is_same<decltype(pipe_pipe_test), i<6>>::value , "");

        static_assert(pipe_<input<int,int>,
                             quote_<ls_>,
                             is_<ls_<int,int>>>::type::value,"");
        // quote_ is a little special but once carefully examined, it is a quite simple.
        // if you are not yet familiar with template template parameter, let's start with
        // a familiar classes : std::vector<int> . Well int is what we call a template
        // argument, but std::vector is the template "container". Note that I didn't say 
        // std::vector<int>, only std::vector. 
        // pipe_<input<int>, quote_<std::vector> >::type would create a std::vector<int> 
        
        auto pipe_less = pipe_<input<i<1>,i<2> >, lift_<less>>::type{};
        static_assert(pipe_less.value, "");
        // We are testing lift_ with a simple metafunction.
        // less continue with a bool_type. But lift_ wrap the meta argument.
        // Basically, if at some point we have input<i<1>,i<2>>, and lift_<less>
        // continue with with less<i<1>,i<2>>::type.
        // Lift_ is the customization point. As long as your type contain a ::type, 
        // it is compatible. 

        auto pipe_is_test = pipe_<input<i<1>>, is_<i<1>> >::type{};
        static_assert(pipe_is_test.value,"");
        // is_ simply check if the types at this point is the same as the metaargument 
        // and continue with b<true> or b<false> depending of the result

        auto pipe_is_test_multi = pipe_<input<i<1>,i<2>>, is_<i<1>,i<2>> >::type{};
        static_assert(pipe_is_test_multi.value,"");
        // This is such a good feature; instead of always listify then compare with ls_<...> 


        static_assert(pipe_<    input<i<1>,int> ,
                        listify,
                        is_<ls_<i<1>,int>> >::type::value,"");
        static_assert(pipe_<input<ls_<int,int> > ,
                            unpack ,listify,
                            is_<ls_<int,int>>  >::type::value, "");
        static_assert(pipe_<input<ls_<int,int>> ,unpack ,is_<int,int>  >::type::value, "");
        //Doesn't mean that we're not going to support it


        static_assert(pipe_<input<i<1>,i<2>>, 
                        transform_<plus<i<3>>>,
                        is_<i<4>,i<5>>
                        >::type::value,"");
        // Transform apply this metafunctor to the types one by one
        // Can also be called a map_ but I disgress transform_ is a better name

        
        static_assert(pipe_<input<i<1>>,fork_<  plus<i<1>>, 
                                                plus<i<2>> 
                                                >   , is_<i<2>, i<3>> >::type::value,"");
        // pipes and forks in a single expression ? Cool Stuff


        static_assert(pipe_<input<int,float>,reverse,is_<float,int>>::type::value,"");
        // types of list a reversing simply is metafunction this.

        static_assert(pipe_<mkseq_<i<5>> , 
                            is_<i<0>, i<1>, i<2>, i<3>, i<4>> >::type::value,"");
        static_assert(    pipe_<mkseq_<i<2>>, 
                            transform_<input<int>>, 
                            is_<int,int>>::type::value,"");
        //mkseq_ is simply creating a list of zero-index int_types
        // The current implementation is not the best one, but will soon be.

        return 0;
}
