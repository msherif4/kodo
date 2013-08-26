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
    /// decoder has the layer::print_cached_symbol_data(std::ostream&)
    ///
    /// Example:
    ///
    /// typedef kodo::full_rlnc8_decoder decoder_t;
    ///
    /// if(kodo::has_print_cached_symbol_data<decoder_t>::value)
    /// {
    ///     // Do something here
    /// }
    ///
#define MAKE_HAS_ACCESSOR_1(TNAME, FUNC, ARG1)                          \
    template<class T, class Arg1 = ARG1>                                \
    struct TNAME                                                        \
    {                                                                   \
    template<class U>                                                   \
    static uint8_t test(                                                \
        U* data, Arg1* a1 = 0,                                          \
        typename std::enable_if<std::is_void<decltype(                  \
                             data-> FUNC (*a1))>::value>::type* v = 0); \
                                                                        \
    static uint32_t test(...);                                          \
                                                                        \
    static const bool value = sizeof(test((T*)0)) == 1;                 \
    };                                                                  \


    MAKE_HAS_ACCESSOR_1(dummy_has_print_decoder_state, print_decoder_state, std::ostream)

#undef MAKE_HAS_ACCESSOR_1



}


