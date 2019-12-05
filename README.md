# rage
meta-programming. C++11 (technically C++0x)

This repo is still very early in the development.

If you are interested to see the code in action, open include/rage.cpp and this serves me as a very small testing unit.
Most of it is unreadable but provide a promising interface to play with types.

```C++
typedef pipe_<input<i<1>>, plus_<i<3>> , is_<i<4>> >::type is_it_4;
static_assert(is_it_4.value,"");
```
