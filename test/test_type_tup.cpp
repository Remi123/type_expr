//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#include "type_expr.hpp"
#include <assert.h>
#include <type_traits>
#include "type_tup.hpp"

namespace te = type_expr;
// TYPE_TUP Test
int main()
{
  te::tup<int, float> tt2{1, 2.0f};
  tt2.get<0>() = 2;
  assert(tt2.get<0>() == 2);
  auto tt3 = te::make_tup(1, 2, 3);
  assert(tt3.get<2>() == 3);
  auto tt7 = tup_cat(tt2, tt3, tt2);
  assert(tt7.get<4>() == 3);
  // TYPE_TUP
  return 0;
}

