//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)
#ifndef TYPE_EXPR_VAR_HPP
#define TYPE_EXPR_VAR_HPP

#include <new>

#include "type_expr.hpp"

namespace type_expr {

namespace te = type_expr;

// VAR_IMPL
template <typename... Ts>
class var_impl {
  int32_t m_index = -1;
  using storage_t = typename std::aligned_union<1, Ts...>::type;
  storage_t m_storage;

 public:
  ~var_impl();           // Dtor
  var_impl() = default;  // Default Ctor
  template <typename T>
  var_impl(T&& value)
      : m_index(te::eval_pipe_<te::input_<Ts...>, te::find_if_<te::is_<T>>,
                               first>::value) {
    new (&m_storage) T(std::forward<T>(value));
  }
  template <typename T>
  var_impl(const T& value)
      : m_index(te::eval_pipe_<te::input_<Ts...>, te::find_if_<te::is_<T>>,
                               first>::value) {
    new (&m_storage) T(value);
  }

  template <typename T>
  operator T() = delete;

  template <typename T>
  T* get_if() {
    if (m_index == -1 || not is<T>())
      return nullptr;
    else
      return reinterpret_cast<T*>(&m_storage);
  }

  bool is_empty() { return m_index == -1; }

  template <typename T>
  bool is() {
    return m_index == te::eval_pipe_<te::input_<Ts...>,
                                     te::find_if_<te::is_<T>>, first>::value;
  }

  // STATIC ASSERTION
  // Mandatory but doesn't contribute to readability
  static_assert(te::eval_pipe_<te::input_<Ts...>,
                               te::none_of_<te::lift_<std::is_array>>>::value,
                "No arrays accepted");
  static_assert(
      te::eval_pipe_<te::input_<Ts...>,
                     te::none_of_<te::lift_<std::is_reference>>>::value,
      "No reference accepted");
  static_assert(
      te::eval_pipe_<te::input_<Ts...>, te::none_of_<te::is_<void>>>::value,
      "No reference accepted");
};

// DTOR
template <typename... Ts>
var_impl<Ts...>::~var_impl() {
  if (m_index != -1) {
    const bool fold_trick[] = {
        (is<Ts>() ? reinterpret_cast<Ts&>(m_storage).~Ts(), true : false)...};
    m_index = -1;
  }
}

// VAR
template <typename... Ts>
using var =
    te::eval_pipe_<te::input_<Ts...>, te::unique, te::quote_<te::var_impl>>;
template <typename T>
using opt = te::var<T>;

}  // namespace type_expr
#endif
