//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#ifndef TYPE_EXPR_TUP_HPP
#define TYPE_EXPR_TUP_HPP

#include <type_traits>
#include <utility>

#include "type_expr.hpp"

// FORWARD DECLARATION
namespace te {

	// TUP_INST
	template <typename Index, typename Type>
		struct tup_element;

	// TUP_IMPL
	template <typename... Zip_Indexes_Types>
		struct tup_impl;

	// TUP
	template<typename ... Ts>
		struct tup ;
}

namespace std{
	template<typename > struct tuple_size;
	template<typename ... Ts>
		struct tuple_size<te::tup<Ts...>> : public std::integral_constant<int,te::tup<Ts...>::size>{};
	template<std::size_t I, typename ... Ts>
		struct tuple_element<I,te::tup<Ts...>> {
			using type = te::eval_pipe_<te::ts_<Ts...>,te::at_c<I>>;
		};
}

namespace te {

	// TUP_ELEMENT
	template <int I, typename T>
		struct tup_element<te::i<I>, T> {
			constexpr tup_element() : data{}{} 
			constexpr tup_element(const T& t) : data{t} {} // Ref
			template<typename U>
				constexpr tup_element(U &&t) : data{std::forward<U>(t)} {} // Move or copy
#if __cplusplus > 201703
			constexpr
#endif
			~tup_element() = default;

			constexpr tup_element(const tup_element<te::i<I>,T>& other) : data{other.data} {}
			constexpr tup_element(te::tup_element<te::i<I>,T>&& o) : data{std::forward<T>(o.data)}{}
			constexpr T& get() const
			{
				return const_cast<T&>(data);
			}
			protected:
			T data;
		};
	template<int I>
		struct tup_element<te::i<I>,void>{
			/*	The goal of this class is to not make the compiler explode when instantiating a void element.
			 *	A void is a perfectly valid tup element. Accessing it should be forbidden
			 * */
			constexpr tup_element() {} 
			template<typename T>
				constexpr tup_element(const T& t) {} 
			template<typename U>
				constexpr tup_element(U &&t){} // Move or copy
#if __cplusplus > 201703
			constexpr
#endif
			~tup_element() = default;
			template<typename T>
				constexpr tup_element(const tup_element<te::i<I>,T>& other) {}
			template<typename T>
				constexpr tup_element(te::tup_element<te::i<I>,T>&& o) {}

		};

	template<int Index,typename ... IElem>
		using nth_tup_element_ = 
		te::eval_pipe_<te::ts_<IElem...>
		,te::nth_element_<te::i<Index>,te::transform_<te::unwrap,te::second,te::size>,te::greater_<>>
		>;

	// TUP_IMPL
	template <typename... Is, typename... Ts>
		struct tup_impl<tup_element<Is, Ts>...> 
		//: nth_tup_element_<Is::value,tup_element<Is,Ts>...>... 
		: tup_element<Is,Ts>...
		{
			constexpr tup_impl()  : tup_element<Is,Ts>{}... {} 
			constexpr tup_impl(const Ts& ... ts) : tup_element<Is, Ts>(ts)... {}
			template<typename ... Us>
				constexpr tup_impl(Us&&... ts) : tup_element<Is, Ts>{std::forward<Us>(ts)}... {}
#if __cplusplus > 201703
			constexpr
#endif
			~tup_impl() = default;

			constexpr tup_impl(const tup_impl<tup_element<Is,Ts>...>& o) : tup_element<Is, Ts>{o}... {}
			constexpr tup_impl(tup_impl<tup_element<Is,Ts>...>&& o) : tup_element<Is,Ts>{std::forward<tup_element<Is,Ts>>(o)}...{}

			template <std::size_t I>
				constexpr auto get() const -> te::eval_pipe_<
				te::ts_<te::ls_<Is,Ts>...>
				,te::filter_<te::unwrap,te::first,te::same_as_<te::i<I>>>,te::unwrap,te::second>&  
				{
					return te::eval_pipe_<te::ts_<te::ls_<Is,Ts>...>,te::filter_<te::unwrap,te::first,te::same_as_<i<I>>>,te::unwrap,te::wrap_<tup_element>>::get();
				}
			template<typename T, int I = 0>
				constexpr auto get() -> T&
				{
					return 
						te::eval_pipe_<te::ts_<te::tup_element<Is,Ts>...>
						, te::filter_<te::unwrap,te::second,te::same_as_<T>>
						, te::at_c<I>
						>::get();
				}
		};

	template <typename... Ts>
		using te_tup_metafunction =
		te::eval_pipe_<te::ts_<Ts...>, te::zip_index,te::transform_<wrap_<tup_element>>, te::wrap_<tup_impl>>;

	// TUP
	template <typename... Ts>
		struct tup : te_tup_metafunction<Ts...>{
			static constexpr std::size_t size = sizeof...(Ts);
			using _base = te_tup_metafunction<Ts...> ;
#if __cplusplus > 201703
			constexpr
#endif
			~tup() = default;
			constexpr tup(){
				using dft_ctor = te::trait_<std::is_nothrow_default_constructible>;
				static_assert(te::eval_pipe_<te::ts_<Ts...>,te::all_of_<te::trait_<std::is_nothrow_default_constructible>>>::value
						,"All Types are not default constructible");
			}
			constexpr tup(const Ts& ... ts) : _base(ts...) {}
			template<typename ... Us>
				constexpr tup(Ts&&... us) : _base(std::forward<Ts>(us)...) {}
			constexpr tup(const te::tup<Ts...>& o) : _base{(_base)o} {}
			constexpr tup(te::tup<Ts...>&& o) : _base{std::forward<_base>(o)} {}
		};

	template <class T>
		struct unwrap_refwrapper{using type = T;};
	template <class T>
		struct unwrap_refwrapper<std::reference_wrapper<T>> {using type = T&;};
	template <class T>
		using unwrap_decay_t = typename unwrap_refwrapper<typename std::decay<T>::type>::type;

	template <class... Types>
		static inline constexpr // since C++14
		te::tup<unwrap_decay_t<Types>...> make_tup(Types&&... args)
		{
			return te::tup<unwrap_decay_t<Types>...>{std::forward<Types>(args)...};
		}

	template <typename... Args>
		constexpr // since C++14
		std::tuple<Args&...> tie(Args&... args) noexcept {
			return {args...};
		}

	template <class... Types>
		constexpr tup<Types &&...> forward_as_tup(Types &&... args) noexcept {
			return te::tup<Types &&...>(std::forward<Types>(args)...);
		}

	template<typename ... Ts, int ... Is>
		constexpr te::eval_pipe_<te::input_<Ts...>,te::fork_<te::at_c<Is>...>,te::wrap_<tup>> tup_get(std::integer_sequence<int,Is...>,te::tup<Ts...>& tup)  
		{
			return te::make_tup(std::move(std::get<Is>(tup))...);
		}

	// TUP_CAT
	// Given multiple tup, concatenate them using the same expension trick than Eric
	// Niebler The trick is to get two std::integer_sequence which are the cartesian
	// cartesian of each indexes of the tups with their respective index in the
	// concatenation function. given tup< tup<A,B,C>, tup<D>, tup<>, tup<E,F> > //
	// Tup_of_Tups index_of_tup           = std::integer_sequence<int,0,0,0,1,3,3>
	// index inside each tup  = std::integer_sequence<int,0,1,2,0,0,1>
	//
	namespace detail {
		template <int... Is, int... Ks, typename... Ts, typename Tup_of_Tups>
			constexpr auto tup_cat_impl(std::integer_sequence<int, Is...>, 
					std::integer_sequence<int, Ks...>,
					Tup_of_Tups &&tpls) 
			-> decltype(te::make_tup(tpls.template get<Is>().template get<Ks>()...))
			{
				return te::make_tup(
						// get<0>().get<0>(), get<0>().get<1>(), ...
						tpls.template get<Is>().template get<Ks>()...);
			}
template<typename ... Ts>
		using tup_cat_return = te::eval_pipe_<te::ts_<Ts...>,te::transform_<te::unwrap>,te::flatten,te::wrap_<te::tup>>;
	};  // namespace detail

		template <typename... Tups>
		constexpr detail::tup_cat_return<Tups...> tup_cat(Tups &&... tups) {
			// This do the magic of getting the cartesian cartesian of each tup's types
			// with the index inside
			using zip_indexes = te::eval_pipe_<
				te::ts_<Tups...>, te::transform_<te::trait_<std::remove_reference>,te::trait_<std::tuple_size>, te::mkseq_<>>,
				te::zip_index, transform_<cartesian>
					, te::flatten
					, te::unzip>;

			using tup_index =
				te::eval_pipe_<zip_indexes, te::first, te::wrap_std_integer_sequence_<int>>;
			using types_index =
				te::eval_pipe_<zip_indexes, te::second, te::wrap_std_integer_sequence_<int>>;
			return detail::tup_cat_impl(
					tup_index{},    // int_seq
					types_index{},  // int_seq
					te::forward_as_tup(std::forward<Tups>(tups)...));
		};

};  // namespace te

namespace std{

	template <unsigned int I, typename... Ts>
		constexpr auto get(te::tup<Ts...> &t) -> decltype(t.template get<I>()) {
			return t.template get<I>();
		}
	template <typename T,unsigned int I = 0, typename... Ts>
		constexpr auto get(te::tup<Ts...> &t) -> decltype(t.template get<T,I>()) {
			return t.template get<T,I>();
		}

};


#endif  // TYPE_EXPR_TUP_HPP
