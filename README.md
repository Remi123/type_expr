# Type_expr : C++11 Meta-programming library. 
**This repo is still very early in the development.**

<details>
  <summary>State of the library (click to expand)</summary>
  
  > Early feedback was very positive. The syntax is very nice.
  > The implementation doesn't have the most state of the art optimization, but we are in the design and prototype phase.
  > The tests write themselves but they are in the wrong folder.
  > The CMakelist.txt is not good at all.
  > The wiki is well started but I'm trying to give the correct definition and it's harder than initially thought.
</details>
<details>
  <summary>Incoming breaking changes (click to expand)</summary>
  Most of the unary and binary meta-expression will now transition from having an meta-expression type to a variadic pack of meta-expression. The idea is to ease the parsing of multiples `f < te::input_< Ts... >, input_< Us... >` and requiring a lot less `te::pipe_< unpack, Fs... >` . This is a breaking change and most meta-expression weren't exhaustively tested. Some functions that required two informations like `te::replace_if_< P, F >` were left as such since I don't know if I should expand the F...` 
</details>

## Code in Actions :
```C++
namespace te = type_expr;
template<int N>
using int_c = std::integral_constant<int,N>;
te::eval_pipe_<te::input<int_c<1>>, te::plus_<int_c<3>> > is_it_4; // is_it_4 is an std::integral_constant<int,4>
static_assert(is_it_4.value == 4,"");
```

If you want to ... | Then ... 
--- | ---
See the code in action | open include/type_expr.cpp as this serves me as a very small testing unit.
Easily playtest this library on Compiler Explorer | You can just write `#include <https://raw.githubusercontent.com/Remi123/type_expr/master/include/type_expr.hpp>` which is the raw text of my file and it should be fine under a C++11 compiler
Look at the doc | Unfortunately the doc is not complete since I think of adding some more breaking changes in the near future. You can check the wiki. My meta-expression names are trying to be very similar to kvasir::mpl's ones.

## How to Debug and Develop in Meta-Programming :
My favorite method to debug is to write a class with no conversion operator like this : `struct B{};` and then create an instance of a type like so `te::eval_pipe_<te::input_<int>, te::push_back_<float>> varname_not_important = B{};`
 This will not compile, but the compiler will happily give you this error or something similar : 
 `conversion from 'B' to non-scalar type type_expr::eval_pipe_<type_expr::input_<int>, type_expr::push_back_<float> >' {aka 'type_expr::input_<int, float>'} requested`
 The rest is just me iteratively adding meta-expression until it work.
 ```C++   
  >te::eval_pipe_<te::input_<int>, te::push_back_<float>, is_<int,float>> varname_not_important = B{}; 
  >// Cannot convert type B to std::integral_constant<bool,true>
```
___

## The Goal of this Library :
Implement a meta-programming library
Reimplement std::tuple, std::optional and std::variant and then rework the visitation pattern.
Provide some tutorial videos and improve the documention in the wiki.
Any other ideas that come along the way

___

## Why Another Meta-Programming Library :
 I think I've finally found a way to provide the interface I wanted to play with types. All the other similar library use a lot of nesting to parse the meta-functions with the form of `G(F())`which call G() with the output of F(). Type_expr allows you to do the same with a syntax similar to  `F() | G(g_args) | H() | I()...` 
Obviously, since this is template meta-programming the real form is `eval_pipe_<input_<Ts...>, F, G<g_args>, H, I>;` but let's agree that this is better than `call< F< G<g_args,H< I<>>>>,Ts...>;`

This nicer syntax allows for easier concatenation of meta-expressions to be more readable. You can also alias another meta-expression under te::pipe_<Fs...> to use it later
```C++
   // This example was inspired by Arthur O'Dwyer' s example at 
   // https://quuxplusone.github.io/blog/2019/12/28/metaprogramming-n-ways/
  struct Z{}; // Empty class

  using Metafunction_example = te::pipe_<
    te::remove_if_<te::lift_<std::is_empty>>,
    te::sort_<te::transform_<te::size>,te::greater_<>>,
    te::quote_<Typelist> >;
    
  static_assert(std::is_same<
    te::eval_pipe_<te::input_<Z,int[4],Z, int[1],Z,int[2], int[3]>, Metafunction_example>
    ,Typelist<int[4],int[3],int[2],int[1]>
    >::value);
```
___

## Some Comparison :
Author | Link  
--- | ---: 
Eric Niebler | http://ericniebler.com/2014/11/13/tiny-metaprogramming-library/ 
Remi | https://github.com/Remi123/type_expr/blob/master/include/type_tup.hpp#L69 

___

Everything is work in progress. To be continued
