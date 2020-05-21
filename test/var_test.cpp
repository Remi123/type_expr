//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#include <assert.h>

#include <string>
#include <type_traits>

#include "type_var.hpp"

// TYPE_TUP Test
int main() {
  {
    te::var<int, float> vnull{};
    assert(vnull.is_empty() && vnull.get_if<int>() == nullptr);
    te::var<int, float, short> va{1}, vb{0.9f};
    assert(not va.is_empty() && va.is<int>());
    assert(*va.get_if<int>() == 1 && va.get_if<short>() == nullptr);
    assert(not vb.is_empty() && vb.is<float>());
    assert(*vb.get_if<float>() == 0.9f);
    te::var<std::string, int> vsi{std::string("Hello World")};
    assert(vsi.is<std::string>() &&
           *vsi.get_if<std::string>() == "Hello World");
  }
  {
    te::var<int, int, float, std::string> v = std::string("Hello");
    te::var<int, float, std::string> vv = v;
    assert(*v.get_if<std::string>() == "Hello" &&
           *vv.get_if<std::string>() == "Hello");
    te::var<std::string, int> vs = std::string("World");
    te::var<std::string, int> vs_m = std::move(vs);
    assert(vs_m.is<std::string>() && *vs_m.get_if<std::string>() == "World");
    std::string hw{"Hello World"};
    vs_m = std::move(hw);
    assert(*vs_m.get_if<std::string>() == "Hello World");
  }
  {
    std::string hw{"Hello World"};
    te::var<std::string> v_en = std::move(hw);
    std::string bm{"Bonjour Monde"};
    te::var<std::string> v_fr = bm;
    v_en = std::move(std::string("Hello Again World"));
    v_fr = std::string("Rebonjour Monde");
  }
  return 0;
}
