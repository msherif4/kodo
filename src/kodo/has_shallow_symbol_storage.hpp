// Copyright Steinwurf ApS 2011-2012.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include "shallow_symbol_storage.hpp"

namespace kodo
{

    /// @ingroup type_traits
    /// Type trait helper allows compile time detection of whether an
    /// encoder / decoder contains the shallow_symbol_storage layer
    ///
    /// Example:
    ///
    /// typedef kodo::full_rlnc8_encoder encoder_t;
    ///
    /// if(kodo::has_shallow_symbol_storage<encoder_t>::value)
    /// {
    ///     // Do something here
    /// }
    ///
    template<class T>
    struct has_shallow_symbol_storage
    {
        template<bool V, class U>
        static uint8_t test(const kodo::shallow_symbol_storage<V,U> *);

        static uint32_t test(...);

        static const bool value = sizeof(test(static_cast<T*>(0))) == 1;
    };

    /// Type trait helper for detection the const_shallow_symbol_storage
    template<class T>
    struct has_const_shallow_symbol_storage
    {
        template<class U>
        static uint8_t test(const kodo::shallow_symbol_storage<true,U> *);

        static uint32_t test(...);

        static const bool value = sizeof(test(static_cast<T*>(0))) == 1;
    };

    /// Type trait helper for detection the mutable_shallow_symbol_storage
    template<class T>
    struct has_mutable_shallow_symbol_storage
    {
        template<class U>
        static uint8_t test(const kodo::shallow_symbol_storage<false,U> *);

        static uint32_t test(...);

        static const bool value = sizeof(test(static_cast<T*>(0))) == 1;
    };


}


