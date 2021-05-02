//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include <assert.h>

#include <string>
#include <type_traits>

#include "variant.hpp"

int main() 
{
	{// Default
	  te::variant<int,float> a {};
	  assert(a.is<int>());
		std::cout << reinterpret_cast<int&>(a.m_storage) << std::endl;
	}
	{// Simple Assignment
	te::variant<int,float> v{4};
	assert(v.is<int>());
	std::cout << reinterpret_cast<int&>(v.m_storage) << std::endl;
	assert(reinterpret_cast<int&>(v.m_storage) == 4 );
	}
#if __cplusplus >= 201402L
	{
	te::variant<int,float> v{42};
  assert(v.is<int>());
  assert(*v.get_if<int>() == 42);
	}
#endif
  return 0;
}
