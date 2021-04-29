//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#include <assert.h>

#include <iostream>
#include <tuple>
#include <string>
#include <memory>


#include <type_expr.hpp>
#include <type_tup.hpp>


//struct bad_padding {
	//short s;
	//int i;
	//char c;
//};

//struct good_padding{
	//int i;
	//short s;
	//char c;
//};
	//static_assert(
			//sizeof(te::tup<short,int,char>) < sizeof(std::tuple<short,int,char>)
			//,"");
	//static_assert(sizeof(good_padding) < sizeof(bad_padding),"");
	//static_assert(sizeof(good_padding) == sizeof(te::tup<short,int,char>),"");
	////tup's types are internally sorted from biggest to smallest size to avoid
	//// padding issues while still allowing typical get<int N>() access 


// TYPE_TUP Test
int main() 
{
	using nocopy = std::unique_ptr<int>;

	{
		te::tup<int> t0{}; // Default Ctor
	}
	{
		int i = 42;
		te::tup<int> t1{42};
		te::tup<int> t2{i};
		te::tup<int> t3{std::move(i)};
		assert(t1.get<0>() == 42 && t2.get<0>() == 42 && t3.get<0>() == 42);
	}
	{
		constexpr te::tup<int> t = 8; 
		constexpr te::tup<int,float> t2{ 8,4 }; 
		constexpr te::tup<const char *> t3{"Hello World"};
		static_assert(t.get<0>() == 8,"");
		static_assert(t2.get<0>() == 8,"");
		static_assert(t3.get<0>() == "Hello World");
	}
	{
		int j = 42, k = 43;
		te::tup<int&> tr1{j};
		te::tup<int,int> tii {1,2};
		assert(tii.get<0>() == 1 && tii.get<1>() == 2);
	}
	{
		int l = 22,  m = 23;
		te::tup<int&,int> trii{l,9};
		te::tup<int,int> tii2{std::move(m),9};
		assert(trii.get<0>() == 22 && trii.get<1>() == 9);
		assert(tii2.get<0>() == 23 && tii2.get<1>() == 9);
	}
	{
		te::tup<int> t1{42};
		te::tup<te::tup<int>&, te::tup<int>> titi{t1, {9}};
		assert(titi.get<0>().get<0>() == 42 && titi.get<1>().get<0>() == 9);
	}
	{
		auto tup_assignment = te::tup<int>{1}, tup_to_move = te::tup<int>{42}; 
		auto tup_moved = std::move(tup_to_move);
		assert(tup_assignment.get<0>() == 1 && tup_moved.get<0>() == 42);
	}
	{
		te::tup<std::string>{"Hello"};
		te::tup<std::string,int> str_int("Hello",42);
		assert(str_int.get<1>() == 42 && str_int.get<0>() == "Hello");
		te::tup<int,std::string> int_str{42,"Hello"};
		assert(int_str.get<0>() == 42 && int_str.get<1>() == "Hello");
	}
	{
		auto tt3 = te::make_tup(4, 5, 6);
		assert(tt3.get<2>() == 6);
	}
	{
		nocopy nc {};
		te::tup<nocopy> tnc (std::move(nc));
		te::tup<nocopy> tup_nocopy{ std::move(tnc)};
	}
	{
		int i = 42;
		auto tfwd = te::forward_as_tup(i,54);
	}
	{
		te::tup<int> t1{42};
		te::tup<int,float> t2{23,100.0f};
		te::tup<std::string,int > t3{"Hello",100};
		auto tt5 = te::tup_cat(t1, t2, t3);
		assert((tt5.get<0>() == 42 &&
				tt5.get<1>() == 23 &&
				tt5.get<2>() == 100.0f &&
				tt5.get<3>() == "Hello" &&
				tt5.get<4>() == 100
			  ));
	}
	{
		te::tup<int,std::string,int,std::string> sisi{1,"Hello",3,"World"};
		assert(( 
			sisi.get<int>() == 1 &&
			sisi.get<int,1>() == 3 &&
			sisi.get<std::string>() == "Hello" &&
			sisi.get<std::string,1>() == "World")); 
		//assert(test == true);
		sisi.get<int>() = 0;
		assert(std::get<0>(sisi) == 0 );
		assert((std::get<int,1>(sisi) == 3));
		assert(sisi.get<int>() == 0);
	}
#if __cplusplus >= 201703L
	{
		int x{}, y{2}; float z{3.0f};
		te::tup<int&,const int,float&&> tpl{x,2,std::move(z)};
		const auto& [a,b,c] = tpl;
		a = 42;
		assert(a == x);
	}
#endif
  	return 0;
}
