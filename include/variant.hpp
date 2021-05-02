//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)
#ifndef TYPE_EXPR_VARIANT_HPP
#define TYPE_EXPR_VARIANT_HPP

#if __cplusplus < 201402L
#define TE_VARIANT_CTOR_CONSTEXPR 
#else
#define TE_VARIANT_CTOR_CONSTEXPR constexpr
#endif

#include <new>

#include "type_expr.hpp"

namespace te{

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
			using unique_fct = te::eval_pipe_<te::input_<Ts...>,te::unique,te::wrap_<te::input_>>;
			storage_t m_storage;
			int32_t m_index_types;

      using dft_ctor_type = te::eval_pipe_<unique_fct,te::find_type_if_<te::trait_<std::is_default_constructible>>>;
			
      TE_VARIANT_CTOR_CONSTEXPR 
          variant() 
				: m_index_types{te::eval_pipe_<unique_fct,find_index_if_<te::trait_<std::is_default_constructible>>>::value}
			{
				variant_assign_mem<dft_ctor_type>(m_storage);
			}
			template<typename U>

      TE_VARIANT_CTOR_CONSTEXPR 
				variant(U&& u) 
				: m_index_types{te::eval_pipe_<unique_fct,te::find_index_if_<te::same_as_<U>>>::value}
			{
				using overload = te::eval_pipe_<unique_fct,te::find_overload<U>>;
				variant_assign_mem<overload>(m_storage,std::forward<U>(u));
			}

#if __cplusplus > 201703
			constexpr
#endif
			~variant()
			{
				destroy(unique_fct{});
			}

        using notconst_storage = typename std::remove_const<storage_t>::type;
			template<typename ... Us>
				constexpr void destroy(te::input_<Us...> unique)
				{
					const bool fold_trick[] = {
            (is<Us>() ? reinterpret_cast<Us&>(const_cast<notconst_storage&>(m_storage)).~Us(), true : false)...
          };
				}
			template<typename T>
				constexpr inline bool is()const noexcept
				{
					return m_index_types == te::eval_pipe_<te::input_<Ts...>,find_index_if_<same_as_<T>>>::value;
				}

			template<typename T>
				constexpr 
				T* get_if()  noexcept
				{
					return is<T>() ? 
						  reinterpret_cast<T*>(&const_cast<notconst_storage&>(m_storage))
					: nullptr;
				}
		};
}; // Namespace te

#undef TE_VARIANT_CTOR_CONSTEXPR  
#endif
