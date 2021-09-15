//     Copyright 2020 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)
#ifndef TYPE_EXPR_VARIANT_HPP
#define TYPE_EXPR_VARIANT_HPP

// C++11 can't be constexpr
#if __cplusplus < 201402L
#define TE_VARIANT_CTOR_CONSTEXPR 
#else
#define TE_VARIANT_CTOR_CONSTEXPR constexpr
#endif

#include <new>

#include "type_expr.hpp"

namespace te{

    template<typename TypeList,bool TrivialDtor>
    struct base_variant;

    template<typename ... Ts, bool TrivialDtor>
        struct base_variant<te::ls_<Ts...>,TrivialDtor>{
            using storage_t =typename std::aligned_union<1, Ts...>::type;
            storage_t m_storage;

            template<typename T>
                constexpr void destroy_as()
                {
                    //reinterpret_cast<T&>(&m_storage).~T();
                }

            template<typename T, typename U>
                constexpr base_variant(te::type_identity<T>,U&& u)
                :m_storage{}
            { new (&m_storage) T(std::forward<T>(u));}

            template<typename T>
                constexpr base_variant(te::type_identity<T>)
                :m_storage{}
            { new (&m_storage) T{};}

            ~base_variant()=default;


            template<typename ... Us>
                constexpr inline void destroy(te::input_<Us...> unique)
                {}
        };

    template<typename ... Ts>
        struct base_variant<te::ls_<Ts...>,false>{};

    template<typename T, typename S, typename U>
        constexpr static inline void mem_variant(S& s,U&& u)
        { new (&s) T(std::forward<T>(u));}

    template<typename T, typename S>
        constexpr static inline void mem_variant(S& s)
        { new (&s) T{};}


    template<typename ... Ts> using var_istrivial = te::eval_pipe_<te::ts_<Ts...>,all_of_<trait_<std::is_trivially_destructible>>>;

    template<typename ... Ts>
        struct variant : base_variant<te::ls_<Ts...>,var_istrivial<Ts...>::value>
    {
        using base = base_variant<te::ls_<Ts...>,var_istrivial<Ts...>::value>;
        using storage_t =typename std::aligned_union<1, Ts...>::type;
        constexpr static bool istrivial = var_istrivial<Ts...>::value;
        using unique_fct = te::eval_pipe_<te::input_<Ts...>,te::unique,te::wrap_<te::input_>>;
        template<typename T> using overload_type = te::eval_pipe_<ts_<Ts...>,find_overload<T>>;
        int32_t m_index_types;

        constexpr
            variant() 
            : base{te::eval_pipe_<te::ts_<Ts...>,te::at_c<0>,wrap_<te::type_identity>>{}}
            , m_index_types{0}
        {}
        template<typename U>
            constexpr
            variant(U&& u) 
            : base{te::eval_pipe_<unique_fct,find_overload<U>,wrap_<type_identity>>{},std::forward<U>(u)}
            , m_index_types{te::eval_pipe_<te::ts_<Ts...>,find_index_if_<te::same_as_<overload_type<U>>>>::value}
        {}

#if __cplusplus > 201703
        constexpr
#endif
            ~variant() =default;
        template<typename ... Us>
            constexpr void destroy(te::input_<Us...> unique)
            {
                const bool fold_trick[] = {
                    (holds_alternative<Us>() ? base::template destroy_as<Us>() , true : false)...
                };
            }
        template<typename T>
            constexpr inline bool holds_alternative()const noexcept
            {
                return m_index_types == te::eval_pipe_<te::input_<Ts...>,find_index_if_<same_as_<T>>>::value;
            }

    };
}; // Namespace te

#undef TE_VARIANT_CTOR_CONSTEXPR  
#endif
