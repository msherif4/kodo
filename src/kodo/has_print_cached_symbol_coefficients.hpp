// Copyright Steinwurf ApS 2011-2013.
// Distributed under the "STEINWURF RESEARCH LICENSE 1.0".
// See accompanying file LICENSE.rst or
// http://www.steinwurf.com/licensing

#pragma once

#include <iostream>
#include "debug_cached_symbol_decoder.hpp"

namespace kodo
{

    /// @ingroup type_traits
    /// Type trait helper allows compile time detection of whether a
    /// decoder has the layer::print_cached_symbol_coefficients(std::ostream&)
    ///
    /// Example:
    ///
    /// typedef kodo::full_rlnc8_decoder decoder_t;
    ///
    /// if(kodo::has_print_cached_symbol_coefficients<decoder_t>::value)
    /// {
    ///     // Do something here
    /// }
    ///
    template<typename T>
    class has_print_cached_symbol_coefficients
    {
        class yes { char m; };
        class no { yes m[2]; };

        struct base_mixin
        {
            void print_cached_symbol_coefficients(std::ostream&);
        };

        struct base : public T, public base_mixin {};
        template <typename U, U t>  class helper{};

        template <typename U>
        static no deduce(U*, helper<void (base_mixin::*)(std::ostream&),
                             &U::print_cached_symbol_coefficients>* = 0);

        static yes deduce(...);

    public:

        static const bool value =
            sizeof(yes) == sizeof(deduce(static_cast<base*>(0)));
    };

}


