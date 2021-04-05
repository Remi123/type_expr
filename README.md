# Type_expr : C++11 Meta-programming library. 
**This repo is still very early in the development.**

![C/C++ CI](https://github.com/Remi123/type_expr/workflows/C/C++%20CI/badge.svg?branch=master)

To test this library, you can simply write in Compiler Explorer 
```
#include <https://raw.githubusercontent.com/Remi123/type_expr/master/include/type_expr.hpp>
```
and it should be fine under a C++11 compiler

This library can be considered a fork of kvasir::mpl, but the core of the parsing is completly different.
>The major difference is "range-like" meta-expression requiring much fewer nesting than most other libraries.

## BEFORE YOU DOWNLOAD THIS LIBRARY !!
kvasir.mpl is faster, no question. The only advantage my library have is an nicer syntax, an easier continuation implementation of meta-function, automatic unpacking of the type te::ts_<Ts...> and the wild-card meta-expression write_<Es...>.

If you want a taste of my features in terms of kvasir.mpl, you can write this in compiler explorer after importing kvasir.mpl:
```
namespace kv = kvasir::mpl;

template<typename T,typename C>
struct closure_monad { using type = typename C::template f<T>;};

 template<typename ... Cs> // All meta-closure
using callpipe = kv::call<kv::fold_left<kv::cfl<closure_monad>>
                                ,kv::nothing,Cs...>; // Start with kv::nothing as the first input, then do all the closure
                                
static_assert(
    std::is_same<
        callpipe<kv::always<int>,kv::add_pointer<>, kv::add_const<>, kv::add_pointer<kv::add_pointer<>> , kv::add_const<>>
        , int*const**const
    >::value,"");
//        One of the meta-closure is nested which meant that all previous axioms of kvasir::mpl are respected, 
//        but nested meta-closure will remains slightly faster ( C1<C2<>> is faster than C1<>,C2<>)
    
// The problem is when you want to play with list, I hope that you like writing kv::unpack
static_assert(
      std::is_same<
          callpipe<   kv::always<kv::list<int,float>>                
                    , kv::unpack<kv::transform<kv::add_pointer<>>>
                    , kv::unpack<kv::transform<kv::add_const<>>>>
          ,kvasir::mpl::list<int*const, float*const>
      >::value,"");
//        unpacking everytime is tedious, and none of the meta-closure of kvasir is made to support this interface
//        To solve this issue, you can append this specialization to the closure_monad structure reviously defined

template<typename T,typename C>
struct closure_monad { using type = typename C::template f<T>;};
template<typename ... Ts,typename C>
struct closure_monad<kv::list<Ts...>,C> { using type = typename C::template f<Ts...>;}; // basically an unbind
/* ... callpipe definition ... */
static_assert(
     std::is_same<
          callpipe<   kv::always<kv::list<int,float>>
                    , kv::transform<kv::add_pointer<>>      // No unpacking
                    , kv::transform<kv::add_const<>>>       // No unpacking
          ,kvasir::mpl::list<int*const, float*const>
      >::value,"");
     
// This convenience has unfortunately the property of completely changing the axioms of kvasir, which is why I decided
// to create this library, so that I can write code that support this kind of interface without worrying about 
// kvasir::mpl interface and implementation. 
```


## Code in Actions :
```C++
namespace te = type_expr;

template<int N> // alias because std::integral_constant<int,N> is long to write
using int_c = std::integral_constant<int,N>;

using result = te::eval_pipe_<      // Evaluate the following expression
          te::input_<int_c<1>>      // Start with integral_constant<int,1> as input
          ,te::plus_<int_c<1>>      // Add integral_constant<int,1> which result in integral_constant<int,2>
          ,te::plus_<int_c<2>>      // You can continue adding meta-expression
        >;                          // result is type std::integral_constant<int,4>
static_assert(result::value == 4,"");

// ------------------------------------------------------------------------------------
// But compile time arithmetic was solved with constexpr, let's do something more fancy.
// This example was inspired by Arthur O'Dwyer' s example at 
// https://quuxplusone.github.io/blog/2019/12/28/metaprogramming-n-ways/

template<typename ... Ts> struct Typelist;
struct Z{}; // Empty class

// Let's "declare" a meta-function expression
using Metafunction_example = te::pipe_<       // pipe_ doesn't evaluate yet
    te::remove_if_<te::lift_<std::is_empty>>, // Remove every empty type (Z is empty)
    te::sort_<                                // Expect an meta-expression binary predicate
      te::transform_<te::size>,               // transform both types in an integral_constant of their sizeof(T)
      te::greater_<>                          // "return" an integral_constant<bool,(first > second)>
      >,                                      // End of sort_<...BinaryPredicate>, all types are now sorted
    te::quote_<Typelist>                      // Wrap the results in the Typelist class
    >;                                        // End of the meta-function

using starting_input          = te::input_<Z,int[3],Z, int[1],Z,int[2], int[4]>;
using expected_resulting_type = Typelist<int[4],int[3],int[2],int[1]>;

static_assert(std::is_same<
    te::eval_pipe_<starting_input, Metafunction_example> // Now evaluate the function with the starting inputs
    ,expected_resulting_type                             // Should give the expected result
    >::value, "Proof");
    
// This is cool but other meta-programming libraries can do similar things,
// let's do something simple that they can't without some chores.

template<int ... Ints>
using summationThenX2_constant =              // We want to add all ints and multiply the result by two
te::eval_pipe_< te::input_<int_c<0>>      // Start at int_c<0>
                , plus_<int_c<Ints>>...   // Expend the meta-fcts plus_ with ...
                , te::multiply_<int_c<2>> // Multiply by two
              >;
static_assert(summationThenX2_constant<1,2,3>::value == 12,"Proof of work");
// We are allowed to use fold-meta-expression at compile time in a C++11 compiler.
// 6 years before C++17 in run-time function.
// The subtile difference is that we are expanding the meta-expression 
// te::plus_<int_c<1>>, te::plus_<int_c<2>>, ...
// The low level of nesting required in the type_expr library allow us to do new things
```
**_And if you think that the syntax is similar to the range algorithm but with commas instead of `|`, it's by design_**
___

If you want to ... | Then ... 
--- | ---
See the code in action |  The test/ folder is where to go
Look at the doc | Unfortunately the doc is not complete since I think of adding some more breaking changes in the near future. You can check the wiki. My meta-expression names are trying to be very similar to kvasir::mpl's ones.

___

## How to Debug and Develop in Meta-Programming :
My favorite method to debug is to write a class with no conversion operator like this : `struct B{};` and then create an instance of a type like so `te::eval_pipe_<te::input_<int>, te::push_back_<float>> varname_not_important = B{};`
 This will not compile, but the compiler will happily give you this error or something similar : 
 `conversion from 'B' to non-scalar type type_expr::eval_pipe_<type_expr::input_<int>, type_expr::push_back_<float> >' {aka 'type_expr::input_<int, float>'} requested`
 The rest is just me iteratively adding meta-expression until it work.
 ```C++   
  te::eval_pipe_<te::input_<int>, te::push_back_<float>, is_<int,float>> varname_not_important = B{}; 
  // Cannot convert type B to std::integral_constant<bool,true>
```
___

## The Goal of this Library :
1. Implement a pure type-based meta-programming library
> I believe the base is solid enough to be reviewed by peers. I need to see what  Boost.Mp11, metal, kvasir.Mpl, Boost.Hana and other are doing that I'm not yet able. 
2. I'll see how I'll be able to provide a complementary library to reproduce some of the stuff in Boost.Fusion.
> Not started yet.
3. Any other funny idea that come along the way.
> There might be surprise... ;)
___

## Why Another Meta-Programming Library :
 I think I've finally found a way to provide the interface I wanted to play with types. All the other similar library use a lot of nesting to parse the meta-functions with the form of `G(F())`which call G() with the output of F(). Type_expr allows you to do the same with a syntax similar to  `F() | G(g_args) | H() | I()...` 
Obviously, since this is template meta-programming the real form is `eval_pipe_<input_<Ts...>, F, G<g_args>, H, I>;` but let's agree that this is better than nested meta-functions monster.

This nicer syntax allows for easier concatenation of meta-expressions to be more readable. You can also alias another meta-expression under te::pipe_<Fs...> to use it later.

Or even crazier, you can use meta-expression to create another meta-expression! Look at the function write_<Es...> for more info. 

___

<details>
  <summary>State of the library (click to expand)</summary>
  
  Early feedback was very positive. The syntax is very nice.
  The implementation doesn't have the most state of the art optimization, but we are in the design and prototype phase.
  The tests write themselves but they are in the wrong folder.
  The CMakelist.txt is not good at all.
  The wiki is well started but I'm trying to give the correct definition and it's harder than initially thought.
</details>
<details>
  <summary>Incoming breaking changes (click to expand)</summary>
  Most of the unary and binary meta-expression will now transition from having an meta-expression type to a variadic pack of meta-expression. The idea is to ease the parsing of multiples `f < te::input_< Ts... >, input_< Us... >` and requiring a lot less `te::pipe_< unpack, Fs... >` . This is a breaking change and most meta-expression weren't exhaustively tested. Some functions that required two informations like `te::replace_if_< P, F >` were left as such since I don't know if I should expand the F... or the P...` 
As of writing this, only fold_left_ is left to adapt.
</details>
  
___

## Some Comparison :
Author | Link  
--- | ---: 
Eric Niebler | http://ericniebler.com/2014/11/13/tiny-metaprogramming-library/ 
Remi | https://github.com/Remi123/type_expr/blob/master/include/type_tup.hpp#L69 

___

Everything is work in progress. To be continued
