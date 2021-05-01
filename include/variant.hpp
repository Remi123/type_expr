//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)
#ifndef TYPE_EXPR_VARIANT_HPP
#define TYPE_EXPR_VARIANT_HPP

#include <new>

#include "type_expr.hpp"

namespace te{

	template <typename T, typename... Ts>
		struct Overloader : T, Overloader<Ts...> {
			using T::operator();
			using Overloader<Ts...>::operator();
		};

	template <typename T> struct Overloader<T> : T {
		using T::operator();
	};

	template<typename T>
		struct v_elem{
			constexpr v_elem() = default;
			template<typename Storage>
			constexpr v_elem(Storage& s) 
			{ 
				static_assert(std::is_default_constructible<T>::value,"");
				new (&s) T();
			}
			template<typename TT, typename Storage>
				constexpr v_elem(TT&& t,Storage& s)
				{
					new (&s) T(std::forward<T>(t));
				}
		};

	template<typename T, typename S, typename U>
	constexpr static inline void variant_assign_mem(S& s,U&& u)
	{ new (&s) T(std::forward<T>(u));}

	template<typename T, typename S>
	constexpr static inline void variant_assign_mem(S& s)
	{ new (&s) T{};}



	template<typename ... Ts>
		struct variant 
		{
			using storage_t =typename std::aligned_union<1, Ts...>::type;
			template<typename ... Us>
				struct vimpl : v_elem<Us>...
			{
				storage_t m_storage;
				int32_t m_index_types;
				int32_t m_index_array;
				constexpr vimpl() 
				:m_index_types{te::eval_pipe_<te::input_<Us...>,find_if_<te::trait_<std::is_default_constructible>>,first>::value},
				m_index_array{0}
				{
					using first_dft_ctor_type = te::eval_pipe_<te::input_<Us...>,te::find_if_<te::trait_<std::is_default_constructible>>,te::second>;
					variant_assign_mem<first_dft_ctor_type>(m_storage);
				}
				template<typename U>
					constexpr vimpl(U&& u) 
					: m_index_types{te::eval_pipe_<te::input_<Us...>,te::find_if_<te::same_as_<U>>,first>::value}
				, m_index_array{0}
				{
					variant_assign_mem<U>(m_storage,std::forward<U>(u));
				}
				constexpr void destroy()
				{
					const bool fold_trick[] = {
						(is<Us>() ? reinterpret_cast<Ts&>(m_storage).~Ts(), true : false)...};
				}
				template<typename T>
					constexpr inline bool is()const noexcept{
						return m_index_types == te::eval_pipe_<te::input_<Us...>,find_if_<same_as_<T>>,first>::value;
					}
			};

			using unique_types = te::eval_pipe_<te::ts_<Ts...>,te::unique,te::wrap_<vimpl>>;
			unique_types base;

			template<typename T> constexpr inline bool is() {return base.template  is<T>();}

			constexpr variant() : base{}
			{
				static_assert(te::eval_pipe_<te::input_<unique_types>, te::any_of_<te::trait_<std::is_default_constructible>>>::value,"");
			}
			template<typename T>
				constexpr variant(T&& t) : base{std::forward<T>(t)}
			{}



			//Dtor
		};


}; // Namespace te

#endif
