# type_expr

meta-programming. C++11

This repo is still very early in the development.

If you are interested to see the code in action, open include/type_expr.cpp and this serves me as a very small testing unit.
Most of it is unreadable but provide a promising interface to play with types. I'm also writing the wiki at the same time.

```C++
using namespace type_expr;
eval_pipe_<input<i<1>>, plus_<i<3>> , is_<i<4>> > is_it_4; // is_it_4 is an std::integral_constant<bool,true>
static_assert(is_it_4.value,"");
```

State of the library :
Early feedback was very positive. The syntax is very nice.
The implementation doesn't have the most state of the art optimization, but we are in the design and prototype phase.
The tests write themselves but they are in the wrong folder.
The CMake is not good at all.
The wiki is well started but I'm trying to give the correct definition and it's harder than initially thought.

The Goal of this library :
Implement a meta-programming library
Reimplement std::tuple, std::optional and std::variant and then rework the visitation pattern.
Provide some tutorial videos.
Any other ideas that come along the way

