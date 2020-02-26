//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#include <assert.h>

#include <string>
#include <type_traits>

#include "type_var.hpp"

namespace te = type_expr;
// TYPE_TUP Test
int main() {
  te::var<int, float> vnull{};
  assert(vnull.index() == -1 && vnull.get_if<int>() == nullptr);
  te::var<int, float, short> va{1}, vb{0.9f};
  assert(va.index() == 0 && va.is<int>());
  assert(*va.get_if<int>() == 1 && va.get_if<short>() == nullptr);
  assert(vb.index() == 1 && vb.is<float>());
  assert(*vb.get_if<float>() == 0.9f);
  te::var<std::string, int> vsi{std::string("Hello World")};
  assert(vsi.is<std::string>() && *vsi.get_if<std::string>() == "Hello World");

  return 0;
}
