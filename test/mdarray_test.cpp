//     Copyright 2021 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#include <type_mdarray.hpp>

#include <assert.h>
#include <memory>
#include <numeric>

int main()
{
    static_assert(te::mdarray<int,2,3,4>{}.size() == 24,"");
    static_assert(std::is_same<te::mdarray<int,1,2,3>::multibuffer,int[1][2][3]>::value,"");

    {   //Test Ctor
        constexpr auto a =  te::mdarray<int,1> {10};
        constexpr auto b = te::mdarray<int,2,4>{1,2,3,4,5,6,7,8};
        constexpr auto c = te::mdarray<int,4,2>{b};
        {
            static_assert(std::is_same<te::mdarray<int,2,3,1>::multibuffer,int[2][3][1]>::value,"");
            static_assert(a[0] == 10,"");
            static_assert(b[{0,0}] == 1 && b[{1,3}] == 8,"");
            static_assert(c[{0,0}] == 1 && c[{3,1}] == 8,"");
        }
    }
    {
        constexpr te::mdarray<int,2,3,4> a{};
        static_assert(a.size() == 24 && a.size() == 24,"");
        static_assert(a.shape()[0] == 2 && a.shape()[1] == 3 && a.shape()[2] == 4 ,"");
        static_assert(a.strides()[0] == 12 && a.strides()[1] == 4 && a.strides()[2] == 1 ,"");
        static_assert(a.get_index({0,0,0}) == 0,"");
        static_assert(a.get_index({1,0,0}) == 12,"");
        for(auto dim : a.shape())
            assert(dim != 0); // Just important to loop
        for(auto stri : a.strides())
            assert(stri != 0); // Just important to loop
    }
    {   //Test Move
        std::unique_ptr<int> p1(new int(41));
        std::unique_ptr<int> p2(new int(42));
        std::unique_ptr<int> p3(new int(43));
        std::unique_ptr<int> p4(new int(44));
        te::mdarray<std::unique_ptr<int>,1,2> a{std::move(p1),std::move(p2)};
        te::mdarray<std::unique_ptr<int>,1,2> b{std::move(p3),std::move(p4)};
        {
            assert( a == a && a != b);
            assert( *a[0] == 41 && *a[1] == 42 && *b[0] == 43 && *b[1] == 44);
            assert( (*a[{0,0}] == 41 && *a[{0,1}] == 42 ));
            assert( (*b[{0,0}] == 43 && *b[{0,1}] == 44 ));        
        }
    }

    {
        constexpr te::mdarray<int,2,4> a = {1,2,3,4,5,6,7,8};
        constexpr te::mdarray<int,8> b{a};
        constexpr te::mdarray<int,2,4> c{2,3,4,5,6,7,8,9};
        {
            static_assert( a.size() == 8 && a[0] == 1 && a[7] == 8,"");
            static_assert( b.size() == 8 && b[0] == 1 && b[7] == 8,"");
            static_assert( c.size() == 8 && c.front() == 2 && c.back() == 9,"");
            static_assert(a == b,"");
            static_assert(a >= b,"");
            static_assert(a <= b,"");
            static_assert(c != a,"");
            static_assert(a < c,"");
            static_assert(c > a,"");
        }
    }
    {
        te::mdarray<int,3,4,2> c{}; std::iota(c.begin(),c.end(),0);
        int index = 0;
        for(int dimension = 0; dimension < c.num_dimensions();++dimension)
        {
            for(int extension = 0; extension < c.extent(dimension); ++extension)
            {
                assert(c[index] == index);
                ++index;
            }
        }
    }
    {
        constexpr te::mdarray<int,3,4,2> d{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23};
        int sum = 0;
        for(std::size_t i = 0; i< d.extent(0);++i)
        for(std::size_t j = 0; j< d.extent(1);++j)
        for(std::size_t k = 0; k< d.extent(2);++k)
            sum += d[{i,j,k}]+1;
        assert(sum == (d.size())*(d.size()+1)/2);
    }
    {
    //           [Rows,Columns]
        constexpr te::mdarray<int,2,3> A{1,2,3
                                        ,4,5,6};
        constexpr te::mdarray<int,3,2> B{1,2,3,4,5,6};  
        te::mdarray<int,2,2> result{};
        result.fill(0);
        for(std::size_t i = 0; i < A.extent(0); ++i) 
        for(std::size_t j = 0; j < B.extent(1); ++j)
        for(std::size_t k = 0; k < B.extent(0); ++k)
            result[{i,j}] += A[{i,k}] * B[{k,j}];   
        auto expected = te::mdarray<int,2,2>{ 22, 28, 49, 64};
        assert(result == expected);  
    }
    {
        te::mdarray<int,2,3> A{1,2,3,4,5,6};
        te::mdarray<int,2,3> B{6,5,4,3,2,1}; 
        std::swap(A,B);
        assert((A == te::mdarray<int,6>{6,5,4,3,2,1}));
        assert((B == te::mdarray<int,6>{1,2,3,4,5,6}));
        static_assert(sizeof(A) == 6*sizeof(int),"");
    }
}
