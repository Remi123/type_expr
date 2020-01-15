#ifndef TYPE_EXPR_TUP_HPP
#define TYPE_EXPR_TUP_HPP

#include "type_expr.hpp"

namespace type_expr {

        namespace te = type_expr;
// FORWARD DECLARATION 

        // TUP_INST
        template<typename Index, typename Type>
        struct tup_inst;

        // TUP_IMPL
        template<typename ... Zip_Indexes_Types>
        struct tup_impl;

        // TUP
        template<typename ... Ts>
        struct tup ; 

// IMPLEMENTATION

        // TUP_INST
        template<int I, typename T>
        struct tup_inst<te::i<I>, T>
        {
          T data;
        };

        // TUP_IMPL
        template<typename ... Is, typename ... Ts, template<typename ...> class TypeList>
        struct tup_impl<TypeList<Is,Ts>...> : tup_inst<Is,Ts> ...
        {
          
        };

        // TUP
        template<typename ... Ts>
        struct tup : te::eval_pipe_<te::input<Ts...>
                                    , te::zip_index
                                    , te::quote_<tup_impl>>
        {

        };
};

#endif TYPE_EXPR_TUP_HPP
