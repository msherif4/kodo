// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#ifndef KODO_HAS_DEEP_SYMBOL_STORAGE_HPP
#define KODO_HAS_DEEP_SYMBOL_STORAGE_HPP

#include "deep_symbol_storage.hpp"

namespace kodo
{

    template<typename> struct any_return { typedef void type; };


    template<typename B, template <class...> class D, typename Sfinae = void>
    struct has: std::false_type {};


    template<class T>
    void has_helper(T *){}

    template<template <class> class T, class Args>
    void has_helper(T<Args> *){}

    template<template <class, class> class T, class Arg1, class Arg2>
    void has_helper(T<Arg1, Arg2> *){}

    template
    <
        template <class, class, class> class T,
        class Arg1, class Arg2, class Arg3
        >
    void has_helper(T<Arg1, Arg2, Arg3> *){}

    template<typename B, template <class...> class  D>
    struct has<B, D,
        typename any_return< decltype( has_helper<D>(static_cast<B*>( nullptr )) ) >::type
        >: std::true_type {};


    /// Type trait helper allows compile time detection of whether an
    /// encoder / decoder contains the deep_symbol_storage layer
    ///
    /// Example:
    ///
    /// typedef kodo::full_rlnc8_encoder encoder_t;
    ///
    /// if(kodo::has_deep_symbol_storage<encoder_t>::value)
    /// {
    ///     // Do something here
    /// }
    ///
    // template<class T>
    // struct has_deep_symbol_storage
    // {
    //     template<template <class> class V, class U>
    //     static uint8_t test(const kodo::shallow_symbol_storage<V,U> *);

    //     static uint32_t test(...);

    //     static const bool value = sizeof(test(static_cast<T*>(0))) == 1;
    // };

    // template<class E, class T>
    // struct has
    // {
    //     static uint8_t test(const T *);

    //     static uint32_t test(...);

    //     static const bool value = sizeof(test(static_cast<E*>(0))) == 1;
    // };

    
    // template<class E, template <class...> class T>
    // struct has
    // {
    //     template<class... Args>
    //     static uint8_t test(const T<Args...> *);

    //     static uint32_t test(...);

    //     static const bool value = sizeof(test(static_cast<E*>(0))) == 1;
    // };

    // template<class E, template <class, class> class T>
    // struct has
    // {
    //     template<class U, class V>
    //     static uint8_t test(const T<U, V> *);

    //     static uint32_t test(...);

    //     static const bool value = sizeof(test(static_cast<E*>(0))) == 1;

    // };

    // template<class E, template <class,class> class T>
    // struct has
    // {
    //     template<class U, class V>
    //     static uint8_t test(const T<U, V> *);

    //     static uint32_t test(...);

    //     static const bool value = sizeof(test(static_cast<E*>(0))) == 1;
    // };

}

#endif

