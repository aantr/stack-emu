#include <type_traits>
#pragma once 

// https://stackoverflow.com/questions/16399346/c11-static-assert-for-equality-comparable-type
template<typename T, typename = void>
struct is_equality_comparable : std::false_type
{ };

template<typename T>
struct is_equality_comparable<T,
    typename std::enable_if<
        true, 
        decltype(std::declval<T&>() == std::declval<T&>(), (void)0)
        >::type
    > : std::true_type
{
};

template<typename T, typename = void>
struct is_inequality_comparable : std::false_type
{ };

template<typename T>
struct is_inequality_comparable<T,
    typename std::enable_if<
        true, 
        decltype(std::declval<T&>() != std::declval<T&>(), (void)0)
        >::type
    > : std::true_type
{
};