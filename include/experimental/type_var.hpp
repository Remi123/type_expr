//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)
#ifndef TYPE_EXPR_VAR_HPP
#define TYPE_EXPR_VAR_HPP

#include <new>

#include "type_expr.hpp"

namespace te {
        template<typename ... Us>
    union trivial_union_storage;
    template<typename U, typename ... Us>
    union trivial_union_storage<U,Us...>{
        U head; trivial_union_storage<Us...> tail;
        template<typename ... Args>
        constexpr explicit trivial_union_storage(const te::type_identity<U>&,const Args&... u)noexcept
            :head{u...}{}
        template<typename Other,typename ... Args>
        constexpr explicit trivial_union_storage(const te::type_identity<Other>& type,const Args&... u)noexcept
            :tail{type,u...}{}
        constexpr explicit trivial_union_storage()
            :tail{}{}
};
    template<>
    union trivial_union_storage<> {
        template<typename ... Args>
        constexpr trivial_union_storage()noexcept{}
    };
    template<typename ... Us>
    union non_trivial_union_storage;
    template<typename U, typename ... Us>
    union non_trivial_union_storage<U,Us...>{
        U head; non_trivial_union_storage<Us...> tail;
        template<typename ... Args>
        constexpr explicit non_trivial_union_storage(const te::type_identity<U>&,const Args&... u)noexcept
            :head{u...}{}
        template<typename Other,typename ... Args>
        constexpr explicit non_trivial_union_storage(const te::type_identity<Other>& type,const Args&... u)noexcept
            :tail{type,u...}{}
        constexpr explicit non_trivial_union_storage()
            :tail{}{}
        ~non_trivial_union_storage(){}
        constexpr U& get_ref(const te::type_identity<U>&)noexcept{return head;}
        constexpr const U& get_ref(const te::type_identity<U>&)const noexcept{return head;}
        template<typename Other>constexpr U& get_ref(const te::type_identity<Other>& o)noexcept{return tail.get_ref(o);}
        template<typename Other>constexpr const U& get_ref(const te::type_identity<Other>& o)const noexcept{return tail.get_ref(o);}
 
    };
    template<>
    union non_trivial_union_storage<> {
        ~non_trivial_union_storage(){}
        constexpr non_trivial_union_storage()noexcept{}
    };



// VAR_IMPL
template <typename... Ts>
class var_impl {
  int32_t m_index = -1;
      template<typename ... Us> using union_storage = typename std::conditional<std::is_destructible<trivial_union_storage<Us...>>::value,trivial_union_storage<Us...>,non_trivial_union_storage<Us...>>::type;
    using storage_t = te::eval_pipe_<te::input_<Ts...>,te::unique,te::wrap_<union_storage>>;
    storage_t m_storage;
  //using storage_t = typename std::aligned_union<1, Ts...>::type;
  //storage_t m_storage;
template<typename T>
using find_ctor = te::eval_pipe_<te::input_<Ts...>,te::find_if_<te::push_back_<T>,te::wraptype_<std::is_constructible>>,te::second>;

template<typename T>
using find_index = te::eval_pipe_<te::input_<Ts...>,te::find_if_<te::push_back_<T>,te::wraptype_<std::is_constructible>>,te::first>;

 public:
  // -------------------------------------------------------------------------
  // Rule Of Five
  //~var_impl() { destroy(); }  // Dtor
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
      te::ts_<Ts...>, find_index_if_<same_as_<T>>>;
  //
  template <typename T>
  constexpr var_impl(T&& value) : m_index(index_helper<T>::value) 
  ,m_storage{te::type_identity<T>{},value}{
    //new (&m_storage) T(std::forward<T>(value));
  }
  template <typename T>
  constexpr var_impl(T& value) : m_index(index_helper<T>::value)
  ,m_storage{te::type_identity<T>{},value}{
    //new (&m_storage) T(value);
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
  T* get_if() noexcept {
    if (m_index == -1 || not is<T>())
      return nullptr;
    else
      return reinterpret_cast<T*>(&m_storage);
  }

  template <typename T>
  const T* get_if()const noexcept {
    if (m_index == -1 || not is<T>())
      return nullptr;
    else
      return reinterpret_cast<T*>(&m_storage);
  }

  constexpr inline bool is_empty() const noexcept { return m_index == -1; }

  template <typename T>
  constexpr inline bool is() const noexcept {
    return m_index ==
           te::eval_pipe_<te::ts_<Ts...>, te::find_index_if_<te::same_as_<T>>>::value;
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
