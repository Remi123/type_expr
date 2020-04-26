#include <cstdio>
#include <algorithm>
#include <functional>
#include <vector>

template <class ForwardIt, typename Cmp>
 void quick_unique_step(ForwardIt first, ForwardIt last, Cmp cmp)
 {
    if(first == last) 
        return;
    auto pivot = *first;
    
    ForwardIt middle1 = std::partition(first, last, 
            [pivot,cmp](const auto& em){ return cmp(pivot,em); });
    {quick_unique_step(middle1, last,cmp);}    
 }

template<typename ForwardIt, typename Cmp >
 ForwardIt quick_unique(ForwardIt begin, ForwardIt end, Cmp cmp )
 {
     quick_unique_step(begin,end,
     cmp
     );
     return std::unique(begin,end,cmp);
 }
 template<typename ForwardIt>
 ForwardIt quick_unique(ForwardIt begin, ForwardIt end)
 {
    //typedef decltype(*begin) value_type;
     return quick_unique(begin,end,std::equal_to<>{});
 }
