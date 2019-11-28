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
        pipe_fold_test = i<5>{};

        auto pipe_pipe_test = pipe_<pipe_< input<i<3>> , plus<i<1>> >,
                                    plus<i<2>> >::type{};
        static_assert( std::is_same<decltype(pipe_pipe_test), i<6>>::value , "");

        auto pipe_less = pipe_<input<i<1>,i<2> >, lift_<less>>::type{};
        static_assert(pipe_less.value, "");

        auto pipe_is_test = pipe_<input<i<1>>, is_<i<1>> >::type{};
        static_assert(pipe_is_test.value,"");

        auto pipe_is_test_multi = pipe_<input<i<1>,i<2>>, is_<i<1>,i<2>> >::type{};
        static_assert(pipe_is_test_multi.value,"");
        // This is such a good feature; instead of always comparing with ls_<...> 


        return 0;
}
