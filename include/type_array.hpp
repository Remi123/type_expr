//     Copyright 2021 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#ifndef TYPE_EXPR_MDARRAY_HPP
#define TYPE_EXPR_MDARRAY_HPP

#include "type_expr.hpp"
#include <stdexcept>


namespace te{

    template<typename T, std::size_t ... Is>
        struct array{
            template<typename ... Ts>
                struct array_value_constant
                {
                    constexpr std::size_t operator[](const std::size_t& n)const{
                        constexpr std::size_t val[] = {(std::size_t)Ts::value...};
                        return val[n];}
                };


            static_assert(sizeof...(Is)>=1,"Must have dimension");
            //  Typedef
            using value_type = T; using size_type = std::size_t; using difference_type = std::ptrdiff_t;
            using reference = value_type&; using const_reference = const value_type&;
            using iterator = T*; using const_iterator = const T*;
            using pointer = value_type*; using const_pointer = const pointer;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;
            //  Additional function
            static constexpr inline std::size_t dimension_size()noexcept
            {return sizeof...(Is);}
            static constexpr inline std::size_t size()noexcept
            {return te::eval_pipe_<te::input_<te::i<1>>, te::multiply_<te::i<Is>>...>::value;   }
            static constexpr const std::size_t shapes(const std::size_t n)noexcept 
            {   
                constexpr std::size_t shapes_[] = {Is...};
                return shapes_[n];
            }

            static constexpr inline const std::size_t strides(const std::size_t n)noexcept 
            {
                using fct = typename std::conditional<(size() > 0),te::pipe_<te::fold_left_list_<te::divide_<>>>,te::drop_front>::type;
                using strides_ = te::eval_pipe_<
                    te::input_<std::integral_constant<std::size_t,size()>,std::integral_constant<std::size_t,Is>...>, 
                    fct, te::wrap_<array_value_constant>>; 
                constexpr strides_ v{};// 
                return v[n];
            }
            static constexpr inline std::size_t max_size()noexcept{return size();}
            value_type buffer[size()]; // storage

            //  data
            constexpr pointer data()noexcept{return size() >0 ? &buffer[0]:nullptr;}
            constexpr const_iterator data()const noexcept{return size() >0 ?&buffer[0]:nullptr;}
            //  front/back
            constexpr reference front()noexcept{return &buffer[0];}
            constexpr const_reference front()const noexcept{return buffer[0];}
            constexpr reference back()noexcept{return &buffer[size()-1];}
            constexpr const_reference back()const noexcept{return buffer[size()-1];}
            //  begin/end
            constexpr iterator begin() noexcept{return &buffer[0];}
            constexpr const_iterator begin()const noexcept{return &buffer[0];}
            constexpr const_iterator cbegin() const noexcept{return &buffer[0];}
            constexpr reverse_iterator rbegin() noexcept{return &buffer[size()];}
            constexpr const_reverse_iterator rbegin()const noexcept{return &buffer[size()];}
            constexpr const_reverse_iterator rcbegin() const noexcept{return &buffer[size()];}
            constexpr iterator end() noexcept{return &buffer[size()];}
            constexpr const_iterator end()const noexcept{return &buffer[size()];}
            constexpr const_iterator cend() const noexcept{return &buffer[size()];}
            constexpr reverse_iterator rend() noexcept{return &buffer[0];}
            constexpr const_reverse_iterator rend()const noexcept{return &buffer[0];}
            constexpr const_reverse_iterator rcend() const noexcept{return &buffer[0];}

            // at
            template<typename ... Size_t>
                constexpr reference at(Size_t ... index )noexcept{
                    static_assert(sizeof...(Size_t) == dimension_size(),"Number of argument must equal the number of dimensions");
                    std::size_t indices[] = {(std::size_t)index...};
                    std::size_t result = 0;
                    for(std::size_t i = 0; i < dimension_size();++i )
                        if(indices[i] < shapes(i)) {result += indices[i] * strides(i);}
                        else {throw std::out_of_range("array member access is higher than the maximum of its dimension");}
                    return buffer[result];
                }
            template<typename ... Size_t>
                constexpr const_reference at(Size_t ... index )const noexcept{
                    static_assert(sizeof...(Size_t) == dimension_size(),"Number of argument must equal the number of dimensions");
                    std::size_t indices[] = {(std::size_t)index...};
                    std::size_t result = 0;
                    for(std::size_t i = 0; i < dimension_size();++i )
                        if(indices[i] < shapes(i)) result += indices[i] * strides(i);
                        else throw std::out_of_range("array member access is higher than the maximum of its dimension");
                    return buffer[result];
                }
            // fill
            void fill( const value_type& value )
            { for(auto& it : buffer) it = value;}
            //empty
            constexpr bool empty() const noexcept{return begin() == end();}

            // Comparaison operator
            friend constexpr bool operator==(const array& lhs,const array& rhs)
            {   for(auto Li = lhs.cbegin(), Ri = rhs.cbegin();Li != lhs.end() || Ri != rhs.end() ;++Li,++Ri)
                if(*Li != *Ri)return false;
                return true;        }
            friend constexpr bool operator<(const array& lhs,const array& rhs)
            {   for(auto Li = lhs.cbegin(), Ri = rhs.cbegin();Li != lhs.end() || Ri != rhs.end() ;++Li,++Ri)
                if(!(*Li < *Ri))return false;
                return true;        }
            friend constexpr bool operator!=(const array& lhs,const array& rhs){return !(lhs==rhs);}
            friend constexpr bool operator>(const array& lhs,const array& rhs){return rhs < lhs;}
            friend constexpr bool operator<=(const array& lhs,const array& rhs){return !(lhs>rhs);}
            friend constexpr bool operator>=(const array& lhs,const array& rhs){return !(lhs<rhs);}

            // Operator []
            
            template <bool Pred = sizeof...(Is) == 1>
                constexpr auto operator[](std::size_t n) noexcept
                -> std::enable_if_t<Pred, reference>
                {
                    return buffer[n];
                }
            template <bool Pred = sizeof...(Is) == 1>
                constexpr auto operator[](std::size_t n) const noexcept
                -> std::enable_if_t<Pred, const_reference>
                {
                    return buffer[n];
                }
            private:
            // Member access private class to make it work with arr[0][1][2]
            template<std::size_t I,std::size_t N = array::dimension_size()-1>
                struct expr_tmp_mem_acc_op{
                    const value_type* data;
                    std::size_t index;
                    template<typename U>operator U() = delete;
                    expr_tmp_mem_acc_op & operator=(const expr_tmp_mem_acc_op&) = delete;
                    expr_tmp_mem_acc_op(const expr_tmp_mem_acc_op&) = delete;
                    expr_tmp_mem_acc_op() = delete;
                    constexpr expr_tmp_mem_acc_op<I+1,N> operator[](std::size_t n)noexcept
                    {return {data,index+n*array::strides(I)};}
                    constexpr const expr_tmp_mem_acc_op<I+1,N> operator[](std::size_t n)const noexcept
                    {return {data,index+n*array::strides(I)};}
                };
            template<std::size_t N>
                struct expr_tmp_mem_acc_op<N,N>
                {   const value_type* data;
                    std::size_t index;
                    template<typename U>operator U() = delete;
                    expr_tmp_mem_acc_op & operator=(const expr_tmp_mem_acc_op&) = delete;
                    expr_tmp_mem_acc_op(const expr_tmp_mem_acc_op&) = delete;
                    expr_tmp_mem_acc_op() = delete;
                    constexpr reference operator[](std::size_t n)noexcept
                    {return const_cast<reference>(data[index + n*array::strides(N)]);}
                    constexpr const_reference operator[](std::size_t n)const noexcept
                    {return data[index + n*array::strides(N)];}
                };
            public:

            template <bool Pred = (sizeof...(Is) > 1)>
                constexpr auto operator[](std::size_t n) noexcept
                -> std::enable_if_t<Pred, expr_tmp_mem_acc_op<1>>
                { return {buffer,n * array::strides(0)};}
            template <bool Pred = (sizeof...(Is) > 1)>
                constexpr auto operator[](std::size_t n) const noexcept
                -> std::enable_if_t<Pred, const expr_tmp_mem_acc_op<1>>
                { return {buffer,n * array::strides(0)};}
        };
}
namespace std{
    template< class T, std::size_t N >
        void swap( array<T,N>& lhs, array<T,N>& rhs )
        {
            using std::swap;
            for (auto& i=lhs.begin(),j = rhs.begin();i<lhs.size();++i,++j)
                swap(*i,*j);
        }
}

#endif  // TYPE_EXPR_MDARRAY_HPP
