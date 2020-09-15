//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)
#ifndef TYPE_EXPR_VAR_HPP
#define TYPE_EXPR_VAR_HPP

#include <new>

#include "type_expr.hpp"

namespace te {

// VAR_IMPL
template <typename... Ts>
class var_impl {
  int32_t m_index = -1;
  using storage_t = typename std::aligned_union<1, Ts...>::type;
  storage_t m_storage;

 public:
  // -------------------------------------------------------------------------
  // Rule Of Five
  ~var_impl() { destroy(); }  // Dtor
  var_impl() = default;       // Default Ctor
  // -------------------------------------------------------------------------
  // Rules of 5 for var_impl<Ts...> type
  constexpr var_impl(var_impl<Ts...>&& other)
      : m_index(other.m_index) {  // Move Ctor
    const bool fold_trick[] = {(other.is<Ts>()
                                ? new (&m_storage) Ts(std::forward<Ts>(
                                      reinterpret_cast<Ts&>(other.m_storage))),
                                true : false)...};
  }
  constexpr var_impl(var_impl& other) : m_index(other.m_index) {  // Copy Ctor
    const bool fold_trick[] = {
        (other.is<Ts>() ? new (&m_storage)
                              const Ts(reinterpret_cast<Ts&>(other.m_storage)),
         true : false)...};
  }
  constexpr var_impl& operator=(var_impl&& other) {  // Move Assign
    destroy();
    const bool fold_trick[] = {(other.is<Ts>()
                                ? new (&m_storage) Ts(std::forward<Ts>(
                                      reinterpret_cast<Ts&>(other.m_storage))),
                                true : false)...};
    m_index = other.m_index;
    return *this;
  }
  constexpr var_impl& operator=(const var_impl& other) {  // Copy Assign
    destroy();
    const bool fold_trick[] = {
        (other.is<Ts>() ? new (&m_storage)
                              Ts(reinterpret_cast<Ts&>(other.m_storage)),
         true : false)...};
    m_index = other.m_index;
    return *this;
  }
  // -------------------------------------------------------------------------
  // Rules of 5 for types that can be contained
  //
  // simple helper to retrive the index from the type list
  template <typename T>
  using index_helper = te::eval_pipe_<
      te::ts_<Ts...>, te::zip_index,
      te::filter_<te::second, te::same_as_<T>>,
      te::cond_<not_<te::same_as_<>>, te::first, te::ts_<te::i<-1>>>>;
  //
  template <typename T>
  var_impl(T&& value) : m_index(index_helper<T>::value), m_storage{} {
    new (&m_storage) T(std::forward<T>(value));
  }
  template <typename T>
  constexpr var_impl(T& value) : m_index(index_helper<T>::value) {
    new (&m_storage) T(value);
  }
  template <typename T>
  constexpr var_impl& operator=(T&& value) {
    destroy();
    new (&m_storage) T{std::forward<T>(value)};
    m_index = index_helper<T>::value;
    return *this;
  }
  template <typename T>
  constexpr var_impl& operator=(T& value) {
    destroy();
    new (&m_storage) T{value};
    m_index = index_helper<T>::value;
    return *this;
  }

  // -------------------------------------------------------------------------
  // Other functions
  //
  constexpr void destroy() {
    if (m_index != -1) {
      const bool fold_trick[] = {
          (is<Ts>() ? reinterpret_cast<Ts&>(m_storage).~Ts(), true : false)...};
      m_index = -1;
    }
  }
  template <typename T>
  operator T() = delete;

  template <typename T>
  constexpr T* get_if() noexcept {
    if (m_index == -1 || not is<T>())
      return nullptr;
    else
      return reinterpret_cast<T*>(&m_storage);
  }

  constexpr inline bool is_empty() const noexcept { return m_index == -1; }

  template <typename T>
  constexpr inline bool is() const noexcept {
    return m_index ==
           te::eval_pipe_<te::ts_<Ts...>, te::find_if_<te::same_as_<T>>,
                          first>::value;
  }

  // STATIC ASSERTION
  // Mandatory but doesn't contribute to readability
  static_assert(te::eval_pipe_<te::ts_<Ts...>,
                               te::none_of_<te::wraptype_<std::is_array>>>::value,
                "No arrays accepted");
  static_assert(
      te::eval_pipe_<te::ts_<Ts...>,
                     te::none_of_<te::wraptype_<std::is_reference>>>::value,
      "No reference accepted");
  static_assert(te::eval_pipe_<te::ts_<Ts...>,
                               te::none_of_<te::same_as_<void>>>::value,
                "No reference accepted");
};

// VAR
template <typename... Ts>
using var =
    te::eval_pipe_<te::ts_<Ts...>, te::unique, te::wrap_<te::var_impl>>;
template <typename T>
using opt = te::var<T>;

}  // namespace te
#endif
