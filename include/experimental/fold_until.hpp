namespace detail {
template <typename... As>
struct f_impl_fold_until;
template <typename F, typename T, typename N, typename... As>
struct f_impl_fold_until<F, T, T, N, As...> {
  typedef T type;
};
template <typename F, typename T>
struct f_impl_fold_until<F, T, T> {
  typedef T type;
};
template <typename F, typename T>
struct f_impl_fold_until<F, T> {
  typedef nothing type;
};
template <typename F, typename T, typename A, typename B, typename... Zs>
struct f_impl_fold_until<F, T, A, B, Zs...>
    : f_impl_fold_until<F, T, typename F::template f<B>::type, Zs...> {};
};  // namespace detail

// FOLD_UNTIL_ : Fold until a type is meet
template <typename F, typename Type>
struct fold_until_ {
  template <typename... Ts>
  struct f;
  template <typename T, typename... Ts>
  struct f<T, Ts...>
      : detail::f_impl_fold_until<F, Type, typename F::template f<T>::type,
                                  Ts...> {};
};

