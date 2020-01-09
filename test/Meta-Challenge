#include <https://raw.githubusercontent.com/Remi123/type_expr/master/include/type_expr.hpp>

namespace te = type_expr;

struct Z{};

using ArthurODwyer = te::pipe_t<
    te::input<Z,int[4],Z, int[1],Z,int[2], int[3]>,
    te::remove_if_<te::lift_<std::is_empty>>,
    te::sort<te::pipe_<te::transform_<te::size>,te::greater_<>>>,
    te::listify>;
static_assert(std::is_same<ArthurODwyer, te::ls_<int[4],int[3],int[2],int[1]>>::value,"");



struct unzip
{
    template<typename ...> struct f;
    template<template<typename ...> class F, typename ... Fs, typename ... Gs>
            struct f<F<Fs,Gs>...> : te::input<te::input<Fs...>,te::input<Gs...>>{};
};

#include <tuple>
template<typename ...Ts> using tup = std::tuple<Ts...>;

using EricNiebler = te::pipe_t<
    te::input<tup<int,float,char>,tup<int*,char*>,tup<>>
    ,te::transform_<te::pipe_<te::unwrap,te::length,te::mkseq,te::listify>>
    ,te::zip_index
    ,te::transform_<te::product>
    ,te::flatten,unzip>;

