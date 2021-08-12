//     Copyright 2021 Remi Drolet (drolet.remi@gmail.com)
// Distributed under the Boost Software License, Version 1.0.
//      (See accompanying file LICENSE or copy at
//        http://www.boost.org/LICENSE_1_0.txt)

#ifndef TYPE_EXPR_MDARRAY_HPP
#define TYPE_EXPR_MDARRAY_HPP

#include "type_expr.hpp"

#include <initializer_list>
#include <iterator>
#include <utility>
#include <array>


namespace te{

template<typename T, std::size_t ... Is>
    struct mdarray 
    {
        // ----------------------------------- Container Concept typedef
        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reference = value_type&;
        using const_reference = const value_type&;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using iterator = value_type*;
        using const_iterator = const value_type*;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const iterator>;

    private:
        template<size_type S> using s = std::integral_constant<size_type,S>;
        static constexpr size_type max_size_v = te::eval_pipe_<te::input_<s<1>>,multiply_<s<Is>>...>::value;
        using subarray = std::array<T,max_size_v>;
        using detail_strides_t = te::eval_pipe_<te::input_<s<max_size_v>,s<Is>...>,fold_left_list_<divide_<>>,te::listify>;
        template<typename ... Js> static constexpr size_type div_remainder[] = {Js::value...}; 
        template<typename ... Js> static constexpr std::array<size_type,sizeof...(Js)> div_remainder2 = {Js::value...}; 
        static constexpr size_type shapes[sizeof...(Is)] = {(size_type)Is...};
        template<typename U, typename V> using to_multiarrays = U[V::value];
        template<typename V> struct add_array { template<typename U> struct f : ts_<U[V::value]>{};};
        template<typename ... RestDiv>
        constexpr const std::array<size_type,sizeof...(RestDiv)> _op2(te::ls_<RestDiv...>) const{return div_remainder2<RestDiv...>;}
    
        alignas(T) value_type sub_array[max_size_v];
    public:


        using multibuffer = te::eval_pipe_<te::input_<s<Is>...,value_type>,te::fold_right_<te::wrap_<to_multiarrays>>>;
        // ----------------------------------- MultiArray Concept functions 
        static constexpr size_type num_dimensions() {return sizeof...(Is);}
        static constexpr const std::array<size_type,sizeof...(Is)> shape() {return std::array<size_type,sizeof...(Is)>{Is...};}
        constexpr const std::array<size_type,sizeof...(Is)> strides() const {return _op2(detail_strides_t{});}
        constexpr const size_type extent(size_type index) const {return shape()[index];}
        static constexpr const size_type max_size(){return max_size_v;}

        //  -----------------------------------Array functions
        //using subarray::subarray;   // Ctor 
        constexpr size_type size() const noexcept {return max_size_v;}
        constexpr bool empty() const {return max_size_v;}
        void fill ( const_reference value ){auto it = begin();while(it != end()) *it++=value;};
        constexpr iterator                  begin() noexcept {return &data()[0];}
        constexpr const_iterator            begin() const noexcept{return &data()[0];}
        constexpr const_iterator            cbegin()const noexcept{return &data()[0];}
        constexpr reverse_iterator          rbegin()const noexcept {return reverse_iterator{end()};}
        constexpr const_reverse_iterator    crbegin()const noexcept {return reverse_iterator{end()};}
        constexpr iterator                  end() noexcept {return &data()[max_size_v];}
        constexpr const_iterator            end()const noexcept{return &data()[max_size_v];}
        constexpr const_iterator            cend()const {return &data()[max_size_v];}
        constexpr reverse_iterator          rend() noexcept {return reverse_iterator{begin()};}
        constexpr const_reverse_iterator    crend()const noexcept {return reverse_iterator{begin()};}
        constexpr pointer                   data() noexcept {return &sub_array[0];}
        constexpr const_pointer             data() const noexcept{return &sub_array[0];}
        constexpr reference                 front()noexcept{return data()[0];}
        constexpr const_reference           front()const noexcept{return data()[0];}
        constexpr reference                 back()noexcept{return data()[max_size_v-1];}
        constexpr const_reference           back()const noexcept{return data()[max_size_v-1];}

        constexpr mdarray& operator=(const mdarray& other)
        {
            auto itL = other.begin(); auto it = begin();
            while(it != end())
            {
                *it++ = *itL++; 
            }
            return *this;
        }

        //  ----------------------------------- Constructor functions
        constexpr mdarray() = default;
        template<typename ... Args>
        constexpr mdarray(Args&&... args) : 
        sub_array{std::forward<Args>(args)...}
        {}
        template<typename ... Args>
        constexpr mdarray(const Args&... args) :  
        sub_array{args...}
        {}
        template<size_type ... Js>
        constexpr mdarray(mdarray<T,Js...>&& other) : sub_array{}        
        {
            static_assert(other.size() == max_size_v,"Must be the same max_size");
            auto itL = other.begin(); auto it = begin();
            while(it != end())
            {
                *it++ = *itL++; 
            }
        }
        template<size_type ... Js>
        constexpr mdarray(const mdarray<T,Js...>& other) : sub_array{}
        {
            static_assert(other.size() == max_size_v,"Must be the same max_size");
            auto itL = other.begin(); auto it = begin();
            while(it != end())
            {
                *it++ = *itL++; 
            }
        }

        //  ----------------------------------- Comparaison operators
        template<size_type ... Js>
        constexpr friend bool operator==(const mdarray& lhs, const mdarray<value_type,Js...>& rhs) 
        {
            if(lhs.size() != rhs.size()) return false;
            auto lit = lhs.cbegin();
            auto rit = rhs.cbegin();

            for(;lit != lhs.cend();++lit,++rit)
            {
                if ((*lit) != (*rit))
                {
                    return false;
                }
            }
            return true;
        }
        template<size_type ... Js>
        constexpr friend bool operator!=(const mdarray& lhs, const mdarray<T,Js...>& rhs)
        {
            return !(lhs == rhs);
        }
        template<size_type ... Js>
        constexpr friend bool operator<(const mdarray& lhs,const  mdarray<T,Js...>& rhs)
        {
            if(lhs.size() != rhs.size()) return false;
            auto lit = lhs.cbegin();
            auto rit = rhs.cbegin();

            for(;lit != lhs.cend();++lit,++rit)
            {
                if (!(*lit < *rit))
                {
                    return false;
                }
            }
            return true;
        }
        template<size_type ... Js>
        constexpr friend bool operator<=(const mdarray& lhs, const mdarray<T,Js...>& rhs)
        {
            return !(rhs < lhs);
        }
        template<size_type ... Js>
        constexpr friend bool operator>(const mdarray& lhs, const mdarray<T,Js...>& rhs)
        {
            return rhs < lhs;
        }
        template<size_type ... Js>
        constexpr friend bool operator>=(const mdarray& lhs, const mdarray<T,Js...>& rhs)
        {
            return !(lhs < rhs);
        }

        //  ----------------------------------- Access operators
        constexpr reference operator[](size_type index) noexcept
        {
            return data()[index];
        }
        constexpr const_reference operator[](size_type index) const noexcept 
        {
            return data()[index];
        }
        constexpr reference operator[](std::initializer_list<size_type> index) noexcept
        {
            return data()[get_index(index)];
        }
        constexpr const_reference operator[](const std::initializer_list<size_type>& index) const noexcept
        {
            return data()[get_index(index)];
        }
        constexpr reference at(std::initializer_list<size_type> index)
        {
            if(index.size() > num_dimensions())
                throw;
            return data()[get_index_safe(index)];
        }
        constexpr const_reference at(std::initializer_list<size_type> index)const
        {
            if(index.size() > num_dimensions())
                throw;
            return data()[get_index_safe(index)];
        }
        constexpr size_type get_index(const std::initializer_list<size_type>& indices) const noexcept
        {
            auto index = 0;
            auto it = indices.begin();
            for(auto i = 0; i < num_dimensions();++i)
            {
                index += *it * strides()[i];
                ++it;
            }
            return index;
        }
        constexpr size_type get_index_safe(const std::initializer_list<size_type>& indices) const
        {
            auto index = 0;
            auto it = indices.begin();
            for(auto i = 0; i < num_dimensions();++i)
            {
                if(*it > shape()[i]-1) throw std::out_of_range("mdarray Out of range");
                index += *it * strides()[i];
                ++it;
            }
            return index;
        }
        //  ----------------------------------- Swap 
        void swap( mdarray& other ) noexcept
        {
            std::swap(static_cast<value_type[max_size_v]>(*this),static_cast<value_type[max_size_v]>(other));
        }
    };

}

#endif  // TYPE_EXPR_MDARRAY_HPP