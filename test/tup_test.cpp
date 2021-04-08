//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#include <assert.h>

#include <string>

#include <type_traits>

#include "type_expr.hpp"
#include "type_tup.hpp"




// TYPE_TUP Test
int main() {
	int i = 42;
	//te::tup<int> t0{};
	te::tup<int> t1{42};
	te::tup<int> t2{i};
	te::tup<int> t3{std::move(i)};
	assert(t1.get<0>() == 42 && t2.get<0>() == 42 && t3.get<0>() == 42);

	int j = 42, k = 43;
	te::tup<int&> tr1{j};

	te::tup<int,int> tii {1,2};
	assert(tii.get<0>() == 1 && tii.get<1>() == 2);

	int l = 22,  m = 23;
	te::tup<int&,int> trii{l,9};
	te::tup<int,int> tii2{std::move(m),9};
	assert(trii.get<0>() == 22 && trii.get<1>() == 9);
	assert(tii2.get<0>() == 23 && tii2.get<1>() == 9);

	te::tup<te::tup<int>&, te::tup<int>> titi{t1, {9}};
	assert(titi.get<0>().get<0>() == 42 && titi.get<1>().get<0>() == 9);
  //tt2.get<0>() = 2;
  //assert(tt2.get<0>() == 2);



  //auto tt3 = te::make_tup(4, 5, 6);
  //assert(tt3.get<2>() == 6);
  ////auto tt6 = tup_cat(tt1, tt2, tt3);
  ////assert(tt6.get<4>() == 5);

  //auto ttt = te::make_tup(tt1,tt2);

  //auto ttsorted = tup_sort(tt6);
  // TYPE_TUP
  return 0;
}
